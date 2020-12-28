#include "nokia5110.h"

#include "spi.h"
#include "gpio.h"

#define NOKIA5110_COL_PER_CHAR  5

void nokia5110_delay_ms(uint32_t millis);

uint16_t display_pos = 0;

const uint8_t characters[][NOKIA5110_COL_PER_CHAR] = {
    // First 32 characters (0x00-0x19) are ignored. These are non-displayable, control characters.
    {0x00, 0x00, 0x00, 0x00, 0x00},  // 0x20
    {0x00, 0x00, 0x5f, 0x00, 0x00},  // 0x21 !
    {0x00, 0x07, 0x00, 0x07, 0x00},  // 0x22 "
    {0x14, 0x7f, 0x14, 0x7f, 0x14},  // 0x23 #
    {0x24, 0x2a, 0x7f, 0x2a, 0x12},  // 0x24 $
    {0x23, 0x13, 0x08, 0x64, 0x62},  // 0x25 %
    {0x36, 0x49, 0x55, 0x22, 0x50},  // 0x26 &
    {0x00, 0x05, 0x03, 0x00, 0x00},  // 0x27 '
    {0x00, 0x1c, 0x22, 0x41, 0x00},  // 0x28 (
    {0x00, 0x41, 0x22, 0x1c, 0x00},  // 0x29 )
    {0x14, 0x08, 0x3e, 0x08, 0x14},  // 0x2a *
    {0x08, 0x08, 0x3e, 0x08, 0x08},  // 0x2b +
    {0x00, 0x50, 0x30, 0x00, 0x00},  // 0x2c ,
    {0x08, 0x08, 0x08, 0x08, 0x08},  // 0x2d -
    {0x00, 0x60, 0x60, 0x00, 0x00},  // 0x2e .
    {0x20, 0x10, 0x08, 0x04, 0x02},  // 0x2f /
    {0x3e, 0x51, 0x49, 0x45, 0x3e},  // 0x30 0
    {0x00, 0x42, 0x7f, 0x40, 0x00},  // 0x31 1
    {0x42, 0x61, 0x51, 0x49, 0x46},  // 0x32 2
    {0x21, 0x41, 0x45, 0x4b, 0x31},  // 0x33 3
    {0x18, 0x14, 0x12, 0x7f, 0x10},  // 0x34 4
    {0x27, 0x45, 0x45, 0x45, 0x39},  // 0x35 5
    {0x3c, 0x4a, 0x49, 0x49, 0x30},  // 0x36 6
    {0x01, 0x71, 0x09, 0x05, 0x03},  // 0x37 7
    {0x36, 0x49, 0x49, 0x49, 0x36},  // 0x38 8
    {0x06, 0x49, 0x49, 0x29, 0x1e},  // 0x39 9
    {0x00, 0x36, 0x36, 0x00, 0x00},  // 0x3a :
    {0x00, 0x56, 0x36, 0x00, 0x00},  // 0x3b ;
    {0x08, 0x14, 0x22, 0x41, 0x00},  // 0x3c <
    {0x14, 0x14, 0x14, 0x14, 0x14},  // 0x3d =
    {0x00, 0x41, 0x22, 0x14, 0x08},  // 0x3e >
    {0x02, 0x01, 0x51, 0x09, 0x06},  // 0x3f ?
    {0x32, 0x49, 0x79, 0x41, 0x3e},  // 0x40 @
    {0x7e, 0x11, 0x11, 0x11, 0x7e},  // 0x41 A
    {0x7f, 0x49, 0x49, 0x49, 0x36},  // 0x42 B
    {0x3e, 0x41, 0x41, 0x41, 0x22},  // 0x43 C
    {0x7f, 0x41, 0x41, 0x22, 0x1c},  // 0x44 D
    {0x7f, 0x49, 0x49, 0x49, 0x41},  // 0x45 E
    {0x7f, 0x09, 0x09, 0x09, 0x01},  // 0x46 F
    {0x3e, 0x41, 0x49, 0x49, 0x7a},  // 0x47 G
    {0x7f, 0x08, 0x08, 0x08, 0x7f},  // 0x48 H
    {0x00, 0x41, 0x7f, 0x41, 0x00},  // 0x49 I
    {0x20, 0x40, 0x41, 0x3f, 0x01},  // 0x4a J
    {0x7f, 0x08, 0x14, 0x22, 0x41},  // 0x4b K
    {0x7f, 0x40, 0x40, 0x40, 0x40},  // 0x4c L
    {0x7f, 0x02, 0x0c, 0x02, 0x7f},  // 0x4d M
    {0x7f, 0x04, 0x08, 0x10, 0x7f},  // 0x4e N
    {0x3e, 0x41, 0x41, 0x41, 0x3e},  // 0x4f O
    {0x7f, 0x09, 0x09, 0x09, 0x06},  // 0x50 P
    {0x3e, 0x41, 0x51, 0x21, 0x5e},  // 0x51 Q
    {0x7f, 0x09, 0x19, 0x29, 0x46},  // 0x52 R
    {0x46, 0x49, 0x49, 0x49, 0x31},  // 0x53 S
    {0x01, 0x01, 0x7f, 0x01, 0x01},  // 0x54 T
    {0x3f, 0x40, 0x40, 0x40, 0x3f},  // 0x55 U
    {0x1f, 0x20, 0x40, 0x20, 0x1f},  // 0x56 V
    {0x3f, 0x40, 0x38, 0x40, 0x3f},  // 0x57 W
    {0x63, 0x14, 0x08, 0x14, 0x63},  // 0x58 X
    {0x07, 0x08, 0x70, 0x08, 0x07},  // 0x59 Y
    {0x61, 0x51, 0x49, 0x45, 0x43},  // 0x5a Z
    {0x00, 0x7f, 0x41, 0x41, 0x00},  // 0x5b [
    {0x02, 0x04, 0x08, 0x10, 0x20},  // 0x5c \ (keep this to escape the backslash)
    {0x00, 0x41, 0x41, 0x7f, 0x00},  // 0x5d ]
    {0x04, 0x02, 0x01, 0x02, 0x04},  // 0x5e ^
    {0x40, 0x40, 0x40, 0x40, 0x40},  // 0x5f _
    {0x00, 0x01, 0x02, 0x04, 0x00},  // 0x60 `
    {0x20, 0x54, 0x54, 0x54, 0x78},  // 0x61 a
    {0x7f, 0x48, 0x44, 0x44, 0x38},  // 0x62 b
    {0x38, 0x44, 0x44, 0x44, 0x20},  // 0x63 c
    {0x38, 0x44, 0x44, 0x48, 0x7f},  // 0x64 d
    {0x38, 0x54, 0x54, 0x54, 0x18},  // 0x65 e
    {0x08, 0x7e, 0x09, 0x01, 0x02},  // 0x66 f
    {0x0c, 0x52, 0x52, 0x52, 0x3e},  // 0x67 g
    {0x7f, 0x08, 0x04, 0x04, 0x78},  // 0x68 h
    {0x00, 0x44, 0x7d, 0x40, 0x00},  // 0x69 i
    {0x20, 0x40, 0x44, 0x3d, 0x00},  // 0x6a j
    {0x7f, 0x10, 0x28, 0x44, 0x00},  // 0x6b k
    {0x00, 0x41, 0x7f, 0x40, 0x00},  // 0x6c l
    {0x7c, 0x04, 0x18, 0x04, 0x78},  // 0x6d m
    {0x7c, 0x08, 0x04, 0x04, 0x78},  // 0x6e n
    {0x38, 0x44, 0x44, 0x44, 0x38},  // 0x6f o
    {0x7c, 0x14, 0x14, 0x14, 0x08},  // 0x70 p
    {0x08, 0x14, 0x14, 0x18, 0x7c},  // 0x71 q
    {0x7c, 0x08, 0x04, 0x04, 0x08},  // 0x72 r
    {0x48, 0x54, 0x54, 0x54, 0x20},  // 0x73 s
    {0x04, 0x3f, 0x44, 0x40, 0x20},  // 0x74 t
    {0x3c, 0x40, 0x40, 0x20, 0x7c},  // 0x75 u
    {0x1c, 0x20, 0x40, 0x20, 0x1c},  // 0x76 v
    {0x3c, 0x40, 0x30, 0x40, 0x3c},  // 0x77 w
    {0x44, 0x28, 0x10, 0x28, 0x44},  // 0x78 x
    {0x0c, 0x50, 0x50, 0x50, 0x3c},  // 0x79 y
    {0x44, 0x64, 0x54, 0x4c, 0x44},  // 0x7a z
    {0x00, 0x08, 0x36, 0x41, 0x00},  // 0x7b {
    {0x00, 0x00, 0x7f, 0x00, 0x00},  // 0x7c |
    {0x00, 0x41, 0x36, 0x08, 0x00},  // 0x7d }
    {0x10, 0x08, 0x08, 0x10, 0x08},  // 0x7e ~
    {0x78, 0x46, 0x41, 0x46, 0x78},  // 0x7f DEL
};

uint8_t screen_buffer[NOKIA5110_BYTES_NR] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xC0, 0x60, 0x30, 0x18, 0x84, 0xC2, 0xA3, 0x22, 0x32, 0x54, 0x44, 0xA4, 0x28,
    0x48, 0x88, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x88, 0x88, 0xC8, 0x68, 0x68,
    0x38, 0x1C, 0x1C, 0x9C, 0xFE, 0xFE, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0xFC, 0x00, 0x07, 0x18, 0x21, 0x26, 0x28,
    0x58, 0x5C, 0x6F, 0xDC, 0xF4, 0xDF, 0x60, 0x30, 0x30, 0x98, 0x8C, 0xCC, 0xFE, 0xFE, 0xFF, 0xFF,
    0xFF, 0x00, 0x00, 0x1E, 0x9F, 0xC3, 0xC3, 0x61, 0x3F, 0x13, 0x19, 0x1F, 0xFF, 0xFF, 0x07, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xE0, 0x98, 0xC6, 0x61, 0x60, 0x30,
    0x90, 0x98, 0x8C, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0x7C, 0x00, 0x80, 0x99, 0xD9, 0xF8, 0xFF,
    0xFF, 0xF9, 0x19, 0x0F, 0x07, 0x06, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0xFF, 0xFC, 0x30, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFE, 0xFF, 0xFF, 0xFF,
    0x30, 0x10, 0x18, 0x99, 0xD9, 0xC0, 0x60, 0x3F, 0xFF, 0xF9, 0xFD, 0xFF, 0xFE, 0x02, 0x03, 0x01,
    0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x03, 0xFC, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x0F, 0x3F, 0x4F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x60, 0x18, 0x07,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x07, 0x04, 0x04, 0x00, 0x08,
    0x0D, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x10, 0x10, 0x20, 0x20, 0x7F, 0xFF, 0xFF, 0x7F, 0x20, 0x20,
    0x10, 0x10, 0x08, 0x0F, 0x0F, 0x0F, 0x0F, 0x08, 0x08, 0x04, 0x04, 0x04, 0x03, 0x03, 0x01, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

/**
 * PORTA0 DC
 * PORTA1 RST
 * PORTA4 Chip Select
 */
void nokia5110_setup(void) {
	uint8_t buffer[4] = {0};

	gpio_setup(GPIO_PORTA, 0, GPIO_MODE_OUTPUT_50, GPIO_CFG_OUT_PUSH_PULL); // DC
	gpio_setup(GPIO_PORTA, 1, GPIO_MODE_OUTPUT_50, GPIO_CFG_OUT_PUSH_PULL); // RST
	gpio_setup(GPIO_PORTA, 4, GPIO_MODE_OUTPUT_50, GPIO_CFG_OUT_PUSH_PULL); // CS

	gpio_clr(GPIOA, 0); // Clear DC
	gpio_set(GPIOA, 1); // Set RST
	gpio_set(GPIOA, 4); // Set Chip Select

    // Reset Pulse
	gpio_clr(GPIOA, 1); // Clr RST
	nokia5110_delay_ms(10);
	gpio_set(GPIOA, 1); // Set RST

    // LCD setup
	gpio_clr(GPIOA, 0); // DC = 0 --> Command
	buffer[0] = 0x21;
	buffer[1] = 0x90;
	buffer[2] = 0x20;
	buffer[3] = 0x0C;
	gpio_clr(GPIOA, 4); // Clear CS
	spi_trx(SPI_BUS_1, buffer, 4);
	gpio_set(GPIOA, 4); // Set CS

    nokia5110_clear_screen();
	nokia5110_move_cursor(0, 0);
}

void nokia5110_move_cursor(uint8_t x, uint8_t y) {
	uint8_t buffer[3] = {0};

    // Updates display position to copy the sent chars on screen buffer
    display_pos = x + y * NOKIA5110_MAX_COL_NR;

	gpio_clr(GPIOA, 0); // DC = 0 --> Command
	buffer[0] = 0x20;
	buffer[1] = 0x40 | y; // Linha
	buffer[2] = 0x80 | x; // Coluna
	gpio_clr(GPIOA, 4); // Clear CS
	spi_trx(SPI_BUS_1, buffer, 3);
	gpio_set(GPIOA, 4); // Set CS
}

void nokia5110_clear_screen(void) {
	uint16_t i = 0;
	uint8_t buffer = 0;

    // Limpa a tela
	gpio_set(GPIOA, 0); // DC = 1 --> Data
	gpio_clr(GPIOA, 4); // Clear CS
	for (i = 0; i < NOKIA5110_BYTES_NR; i++) {
		buffer = 0;
		spi_trx(SPI_BUS_1, &buffer, 1);
	}
	gpio_set(GPIOA, 4); // Set CS
}

void nokia5110_char(char character) {
    // Adds 1 blank collumn after the char
	uint8_t buffer[NOKIA5110_COL_PER_CHAR + 1] = {0};
    uint8_t i = 0;

	gpio_set(GPIOA, 0); // DC = 1 --> Data
    for (i = 0; i < NOKIA5110_COL_PER_CHAR; i++) {
        buffer[i] = characters[character - 0x20][i];
        screen_buffer[display_pos++] = buffer[i];
    }
	gpio_clr(GPIOA, 4); // Clear CS
	spi_trx(SPI_BUS_1, buffer, NOKIA5110_COL_PER_CHAR + 1);
	gpio_set(GPIOA, 4); // Set CS

    // Keeps count of the added blank collumn
    screen_buffer[display_pos++] = 0;
}

void nokia5110_char_at(char character, uint8_t x, uint8_t y) {
    nokia5110_move_cursor(x, y);
    nokia5110_char(character);
}

void nokia5110_string(char* string) {
    uint16_t i = 0;

    while (string[i] != '\0') {
        nokia5110_char(string[i]);
        i++;
    }
}

void nokia5110_string_at(char* string, uint8_t x, uint8_t y) {
    nokia5110_move_cursor(x, y);
    nokia5110_string(string);
}

void nokia5110_update_screen(void) {
    uint16_t i = 0;
    uint8_t buffer = 0;

    nokia5110_move_cursor(0, 0);
    
	gpio_set(GPIOA, 0); // DC = 1 --> Data
	gpio_clr(GPIOA, 4); // Clear CS
    for (i = 0; i < NOKIA5110_BYTES_NR; i++) {
        buffer = screen_buffer[i];
        spi_trx(SPI_BUS_1, &buffer, 1);
    }
	gpio_set(GPIOA, 4); // Set CS
}

void nokia5110_clear_buffer(void) {
    uint16_t i = 0;

    for (i = 0; i < NOKIA5110_BYTES_NR; i++) {
        screen_buffer[i] = 0;
    }
}

/**
 *  0 <= x < 84
 *  0 <= y < 48
 */
void nokia5110_set_pixel(uint8_t x, uint8_t y) {
    uint16_t buffer_pos = (y / 8) * NOKIA5110_MAX_COL_NR + x;

    screen_buffer[buffer_pos] |= (1 << (y % 8));
}

void nokia5110_clr_pixel(uint8_t x, uint8_t y) {
    uint16_t buffer_pos = (y / 8) * NOKIA5110_MAX_COL_NR + x;

    screen_buffer[buffer_pos] &= ~(1 << (y % 8));
}

void nokia5110_draw_rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    uint8_t i = 0;

    for (i = x1; i <= x2; i++) {
        // Lado de cima
        nokia5110_set_pixel(i, y1);
        // Lado de baixo
        nokia5110_set_pixel(i, y2);
    }
    for (i = y1; i <= y2; i++) {
        // Lado esquerdo
        nokia5110_set_pixel(x1, i);
        // Lado direito
        nokia5110_set_pixel(x2, i);
    }
}

void nokia5110_clear_rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    uint8_t i = 0;

    for (i = x1; i <= x2; i++) {
        // Lado de cima
        nokia5110_clr_pixel(i, y1);
        // Lado de baixo
        nokia5110_clr_pixel(i, y2);
    }
    for (i = y1; i <= y2; i++) {
        // Lado esquerdo
        nokia5110_clr_pixel(x1, i);
        // Lado direito
        nokia5110_clr_pixel(x2, i);
    }
}

void nokia5110_delay_ms(uint32_t millis) {
	volatile uint32_t a = 0;

	for (; millis > 0; millis--) {
		for	(a = 0; a < 800; a++);
	}
}