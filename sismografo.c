#include <stdint.h>
#include <math.h>
#include "clock.h"
#include "console.h"
#include "sdram.h"
#include "lcd-spi.h"
#include "gfx.h"
#include <stdio.h>

#define BATTERY_LEVEL 75  // Nivel de batería de ejemplo (75%)
#define USB_CONNECTED 1    // 1 para habilitado, 0 para deshabilitado

void draw_battery_level(int level);
void draw_xyz_values(int x, int y, int z);
void draw_usb_status(int connected);

int main(void) {
    // Inicialización
    clock_setup();
    console_setup(115200);
    sdram_init();
    lcd_spi_init();
    gfx_init(lcd_draw_pixel, 240, 320);
    
    // Pantalla inicial
    gfx_fillScreen(LCD_BLACK);
    
    // Dibujar interfaz
    draw_battery_level(BATTERY_LEVEL);
    draw_xyz_values(123, -45, 67); // Valores de ejemplo para los ejes X, Y, Z
    draw_usb_status(USB_CONNECTED);
    
    // Mostrar el contenido en la pantalla
    lcd_show_frame();
    
    while (1) {
        // Aquí se podría actualizar la pantalla con nuevos valores de sensores
        // o recibir comandos por USB/Serial para interactuar con el osciloscopio.
    }

    return 0;
}

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
}

void draw_xyz_values(int x, int y, int z) {
    gfx_setTextColor(LCD_WHITE, LCD_BLACK);
    gfx_setTextSize(2);
    
    gfx_setCursor(10, 80);
    gfx_puts("X-axis:");
    char x_text[10];
    snprintf(x_text, sizeof(x_text), "%d", x);
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

void draw_usb_status(int connected) {
    gfx_setTextColor(LCD_WHITE, LCD_BLACK);
    gfx_setTextSize(2);
    gfx_setCursor(10, 180);
    gfx_puts("USB:");

    gfx_setCursor(10, 210);
    if (connected) {
        gfx_puts("Connected");
        gfx_fillCircle(200, 220, 10, LCD_GREEN);  // Indicador de conexión
    } else {
        gfx_puts("Disconnected");
        gfx_fillCircle(200, 220, 10, LCD_RED);    // Indicador de desconexión
    }
}

