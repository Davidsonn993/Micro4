#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dac.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#include <stdint.h>
#include "gyr_spi.c"
#include "gyr_spi.h"
//#include "clock.h"

#include <math.h>
#include "console.h"
#include "sdram.h"
#include "lcd-spi.h"
#include "gfx.h"
#include <stdio.h>

#define LED_DISCO_GREEN_PORT GPIOG
#define LED_DISCO_GREEN_PIN GPIO13

#define USART_CONSOLE USART1




// Declaraciones de funciones
int _write(int file, char *ptr, int len);
void draw_battery_level(int level);
void draw_xyz_values(int x, int y, int z);
void draw_usb_status(int connected);
static void spi_setup(void);
void gpio_setup_for_ledRed(void);

static void usart_setup(void)
{
        usart_set_baudrate(USART_CONSOLE, 115200);
	usart_set_databits(USART_CONSOLE, 8);
	usart_set_stopbits(USART_CONSOLE, USART_STOPBITS_1);
	usart_set_mode(USART_CONSOLE, USART_MODE_TX);
	usart_set_parity(USART_CONSOLE, USART_PARITY_NONE);
	usart_set_flow_control(USART_CONSOLE, USART_FLOWCONTROL_NONE);

	/* Finally enable the USART. */
	usart_enable(USART_CONSOLE);
}

int _write(int file, char *ptr, int len)
{
	int i;

	if (file == STDOUT_FILENO || file == STDERR_FILENO) {
		for (i = 0; i < len; i++) {
			if (ptr[i] == '\n') {
				usart_send_blocking(USART_CONSOLE, '\r');
			}
			usart_send_blocking(USART_CONSOLE, ptr[i]);
		}
		return i;
	}
	errno = EIO;
	return -1;
}

//static void gpio_setup(void)
//{
//	/* Enable GPIOG clock. */
//	rcc_periph_clock_enable(RCC_GPIOG);
//
//	/* Set GPIO13 (in GPIO port G) to 'output push-pull'. */
//	gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT,
//			GPIO_PUPD_NONE, GPIO13);
//}

static void button_setup(void)
{
	/* Enable GPIOA clock. */
	rcc_periph_clock_enable(RCC_GPIOA);

	/* Set GPIO1 (in GPIO port A) to 'input open-drain'. */
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);
}

static void adc_setup(void)
{
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO2);
	

	adc_power_off(ADC1);
	adc_disable_scan_mode(ADC1);
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_3CYC);

	adc_power_on(ADC1);

}


static uint16_t read_adc_naiive(uint8_t channel)
{
	uint8_t channel_array[16];
	channel_array[0] = channel;
	adc_set_regular_sequence(ADC1, 1, channel_array);
	adc_start_conversion_regular(ADC1);
	while (!adc_eoc(ADC1));
	uint16_t reg16 = adc_read_regular(ADC1);
	return reg16;
}

int main(void)
{
	char lcd_out[100];
	int i;
	int16_t x, y, z;
	clock_setup();
	usart_setup();
	printf("hi guys!\n");
	adc_setup();
	button_setup();
	
        gpio_setup_for_ledRed();
	console_setup(115200);
        sdram_init();
        gyr_setup();
        lcd_spi_init();
        gfx_init(lcd_draw_pixel, 240, 320);
    
        // Pantalla inicial
        gfx_fillScreen(LCD_RED);
    
        gfx_setTextColor(LCD_BLACK, LCD_RED);
        gfx_setTextSize(4);
        gfx_setCursor(25, 100);
        gfx_puts("David");
        gfx_setTextSize(3);
        gfx_setCursor(80, 150);
        gfx_puts("sonic");
    
        // Mostrar el contenido en la pantalla
        lcd_show_frame();
	
	/* green led for ticking */
	gpio_mode_setup(LED_DISCO_GREEN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
			LED_DISCO_GREEN_PIN);

	while (1) {
		
		// Leer el valor del ADC y calcular el nivel de batería
        uint16_t input_adc0 = read_adc_naiive(2);
        uint16_t level = input_adc0 / 41;  // Escalar el valor para obtener un rango entre 0 y 100
        uint16_t connected;
        uint16_t alarma;
     
        // Lecturas del acelerómetro
        x = gyr_readX()/3000;
        y = gyr_readY()/1000;
        z = gyr_readZ()/10000;
        
       
        gfx_fillScreen(LCD_BLACK);
       
       // Mostrar lecturas en pantalla
       
        
        draw_xyz_values( x, y, z); 
        
        // Borrar el área anterior de la batería antes de redibujar
        gfx_fillRect(42, 40, 100, 20, LCD_BLACK);  // Borrar la barra anterior
        
        // Dibujar el nivel de batería actualizado
        draw_battery_level(level);
         
        
        //configuro la alerta por si la bateria baja de 7c
        if (level<=42.72){
        alarma=1;
        gpio_toggle(GPIOG, GPIO14);
        for (i = 0; i < 1000000; i++) { /* Wait a bit. */
	  __asm__("NOP");
        }
        }else{
        alarma=0;
        }
        
      //configuro la coneccion serial
       if (gpio_get(GPIOA, GPIO0)) {
        connected=1;
        draw_usb_status(connected);
        
        printf("%d,%d,%d,%d,%d,%d\n", level, x, y ,z ,connected ,alarma);
       
       /* LED on/off */
	gpio_toggle(LED_DISCO_GREEN_PORT, LED_DISCO_GREEN_PIN);

	 for (i = 0; i < 1000000; i++) { /* Wait a bit. */
	  __asm__("NOP");
         }
        }else{
        connected=0;
        draw_usb_status(connected);
        //printf("Disconnected\n");
        printf("0,0,0,0,0,0\n");
        
        }
        
        lcd_show_frame();
        
}
return 0;
}
// Función para dibujar el nivel de batería en la pantalla
void draw_battery_level(int level) {
    gfx_setTextColor(LCD_WHITE, LCD_BLACK);
    gfx_setTextSize(2);
    gfx_setCursor(10, 10);
    gfx_puts("Battery:");
    
    // Dibujar una barra de batería
    gfx_drawRect(42, 40, 100, 20, LCD_WHITE);
    gfx_fillRect(42, 40, level, 20, LCD_GREEN);  // Barra que representa el nivel de batería
    
    // Mostrar porcentaje de batería
    gfx_setCursor(170, 40);
    char battery_text[10];
    snprintf(battery_text, sizeof(battery_text), "%d%%", level);
    gfx_puts(battery_text);
    return level;
}

void draw_xyz_values(int x, int y, int z) {
    gfx_setTextColor(LCD_WHITE, LCD_BLACK);
    gfx_setTextSize(2);
    
    gfx_setCursor(10, 80);
    gfx_puts("X-axis:");
    char x_text[10];
    snprintf(x_text, sizeof(x_text), "%d", (float)x );
    gfx_setCursor(120, 80);
    gfx_puts(x_text);

    gfx_setCursor(10, 110);
    gfx_puts("Y-axis:");
    char y_text[10];
    snprintf(y_text, sizeof(y_text), "%d", y);
    gfx_setCursor(120, 110);
    gfx_puts(y_text);

    gfx_setCursor(10, 140);
    gfx_puts("Z-axis:");
    char z_text[10];
    snprintf(z_text, sizeof(z_text), "%d", z);
    gfx_setCursor(120, 140);
    gfx_puts(z_text);

}

// Función para dibujar el estado de conexión USB en la pantalla
void draw_usb_status(int connected) {
    gfx_setTextColor(LCD_WHITE, LCD_BLACK);
    gfx_setTextSize(2);
    gfx_setCursor(10, 180);
    gfx_puts("USB:");

    gfx_setCursor(10, 210);
    if (connected) {
        gfx_puts("Connected");
        gfx_fillCircle(220, 220, 10, LCD_GREEN);  // Indicador de conexión
    
    } else {
        gfx_puts("Disconnected");
        gfx_fillCircle(220, 220, 10, LCD_RED);    // Indicador de desconexión
    }

}

void gpio_setup_for_ledRed(void) {
    rcc_periph_clock_enable(RCC_GPIOG);
    gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14);
}
