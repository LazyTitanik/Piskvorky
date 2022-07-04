#ifndef TWO_PLAYERS_GAME
#define TWO_PLAYERS_GAME

#define FIELD_SIZE 18
#define LCD_WIDTH 480
#define LCD_HEIGH 320
#define PIECE_EMPTY 0
#define PIECE_X 1
#define PIECE_O 2

#include <stdint.h>


#define _POSIX_C_SOURCE 200112L

#define _POSIX_C_SOURCE 200112L

/*
 * Next macros provides location of knobs and LEDs peripherals
 * implemented in MZ_APO FPGA design.
 *
 * The complete list of peripheral implemented in the design
 * can be found on the page
 *   https://cw.fel.cvut.cz/wiki/courses/b35apo/documentation/mz_apo/start
 */

/*
 * Base address of the region used for mapping of the knobs and LEDs
 * peripherals in the ARM Cortex-A9 physical memory address space.
 */
#define SPILED_REG_BASE_PHYS 0x43c40000

/* Valid address range for the region */
#define SPILED_REG_SIZE      0x00004000

/*
 * Byte offset of the register which controls individual LEDs
 * in the row of 32 yellow LEDs. When the corresponding bit
 * is set (value 1) then the LED is lit.
 */
#define SPILED_REG_LED_LINE_o           0x004

/*
 * The register to control 8 bit RGB components of brightness
 * of the first RGB LED
 */
#define SPILED_REG_LED_RGB1_o           0x010

/*
 * The register to control 8 bit RGB components of brightness
 * of the second RGB LED
 */
#define SPILED_REG_LED_RGB2_o           0x014

/*
 * The register which combines direct write to RGB signals
 * of the RGB LEDs, write to the keyboard scan register
 * and control of the two additional individual LEDs.
 * The direct write to RGB signals is orred with PWM
 * signal generated according to the values in previous
 * registers.
 */
#define SPILED_REG_LED_KBDWR_DIRECT_o   0x018

/*
 * Register providing access to unfiltered encoder channels
 * and keyboard return signals.
 */
#define SPILED_REG_KBDRD_KNOBS_DIRECT_o 0x020

/*
 * The register representing knobs positions as three
 * 8-bit values where each value is incremented
 * and decremented by the knob relative turning.
 */
#define SPILED_REG_KNOBS_8BIT_o         0x024

union led {
        struct {
                uint8_t b,g,r;
        };
        uint32_t data;
};

union pixel {
        struct {
                unsigned b : 5;
                unsigned g : 6;
                unsigned r : 5;
        };
        uint16_t d;
};

extern unsigned char *parlcd_reg_base;

int two_players_game(union pixel buffer[LCD_WIDTH][LCD_HEIGH], int x);

#endif
