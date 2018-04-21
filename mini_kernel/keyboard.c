#include "mini_kernel.h"
#include "keyboard.h"

#define KEY_ESCAPE 27

char kbdmap_default[] = {
	// key, shift, alt, ctrl&alt
	'F', 0xFF, 0xFF, 0xFF, // Esc
	'1', '!', '1', '~',
	'2', '"', '2', 'ˇ',
	'3', '#', '3', '^',
	'4', '$', '4', '˘',
	'5', '%', '5', '°',
	'6', '&', '6', '˛',
	'7', '/', '7', '`',
	'8', '(', '8', '˙',
	'9', ')', '9', '´',
	'0', '=', '0', '˝',
	'\'', '?', '\'', '¨',
	'+', '*', '+', '¸',
	0x08, 0x08, 0x7F, 0x08,	/*      backspace       */
	0x09, 0x09, 0x09, 0x09,	/*      tab     */
	'q', 'Q', 'q', '\\',
	'w', 'W', 'w', '|',
	'e', 'E', 'e', '€',
	'r', 'R', 'r', 'r',
	't', 'T', 't', 't',
	'z', 'Z', 'z', 'z',
	'u', 'U', 'u', 'u',
	'i', 'I', 'i', 'i',
	'o', 'O', 'o', 'o',
	'p', 'P', 'p', 'p',
	'š', 'Š', 'š', 'š',
	'đ', 'Đ', 'đ', 'đ',
	0x0A, 0x0A, 0x0A, 0x0A,	/*      enter   */
	'%', 0xFF, 0xFF, 0xFF,	/*      ctrl    */
	'a', 'A', 'a', 'a',
	's', 'S', 's', 's',
	'd', 'D', 'd', 'd',
	'f', 'F', 'f', '[',
	'g', 'G', 'g', ']',
	'h', 'H', 'h', 'h',
	'j', 'J', 'j', 'j',
	'k', 'K', 'k', 'ł',
	'l', 'L', 'l', 'Ł',
	'č', 'Č', 'č', 'č',
	'ć', 'Ć', 'ć', 'ß',	/*      '"      */
	'¸', '¨', '¸', '¸',	/*      `~      */
	'$', 0xFF, 0xFF, 0xFF,	/*      Lshift  (0x2a)  */
	'ž', 'Ž', 'ž', '¤',
	'y', 'Y', 'y', 'y',
	'x', 'X', 'x', 'x',
	'c', 'C', 'c', 'c',
	'v', 'V', 'v', '@',
	'b', 'B', 'b', '{',
	'n', 'N', 'n', '}',
	'm', 'M', 'm', '§',
	',', ';', ',', ',',
	'.', ':', '.', '.',
	'-', '_', '-', '-',
	'{', 0xFF, 0xFF, 0xFF,	/*      Rshift  (0x36)  */
	'*', '*', '*', '*', // Numpad *
	'!', 0xFF, 0xFF, 0xFF,	/*      (0x38)  */
	' ', ' ', ' ', ' ',	/*      space   */
	'C', 0xFF, 0xFF, 0xFF, // Caps lock
	'F', 0xFF, 0xFF, 0xFF, // F1
	'F', 0xFF, 0xFF, 0xFF, // F2
	'F', 0xFF, 0xFF, 0xFF, // F3
	'F', 0xFF, 0xFF, 0xFF, // F4
	'F', 0xFF, 0xFF, 0xFF, // F5
	'F', 0xFF, 0xFF, 0xFF, // F6
	'F', 0xFF, 0xFF, 0xFF, // F7
	'F', 0xFF, 0xFF, 0xFF, // F8
	'F', 0xFF, 0xFF, 0xFF, // F9
	'F', 0xFF, 0xFF, 0xFF, // F10
	'P', 0xFF, 0xFF, 0xFF, // PAUSE
	'9', 0xFF, 0xFF, 0xFF,	/*      (0x46)  */
	'7', '7', '7', '7',	// (0x47) Numpad 7
	'8', '8', '8', '8',	// (0x48) 8
	'9', '9', '9', '9',	// (0x49) 9
	'-', '-', '-', '-',	// (0x4a) -
	'4', '4', '4', '4',	// (0x4b) 4
	'5', '5', '5', '5',	// (0x4c) 5
	'6', '6', '6', '6',	// (0x4d) 6
	'+', '+', '+', '+',	// (0x4e) numpad +
	'1', '1', '1', '1',
	'2', '2', '2', '2',
	'3', '3', '3', '3',
	'0', '0', '0', '0',	// (0x52) 0
	',', ',', ',', ',',	// (0x53) , (numpad)
	'N', 0xFF, 0xFF, 0xFF,	/*      (0x54)  */
	'O', 0xFF, 0xFF, 0xFF,	/*      (0x55)  */
	'<', '>', '<', '<',
	'F', 0xFF, 0xFF, 0xFF, // F11
	'F', 0xFF, 0xFF, 0xFF, // F12
	'-', 0xFF, 0xFF, 0xFF,
	'_', 0xFF, 0xFF, 0xFF,
	'W', 0xFF, 0xFF, 0xFF, // System (windows) key
	'(', 0xFF, 0xFF, 0xFF,
	'C', 0xFF, 0xFF, 0xFF, // Context menu key
	'=', 0xFF, 0xFF, 0xFF,
	'*', 0xFF, 0xFF, 0xFF, // print screen
	'[', 0xFF, 0xFF, 0xFF,
	']', 0xFF, 0xFF, 0xFF
};

char* kbdmap = kbdmap_default;

void keyboard_handle_interrupt() {
	uint8_t i;
	static int lshift;
	static int rshift;
	static int alt;
	static int ctrl;

	do {
		i = __inbyte(0x64);
	} while ((i & 0x1) == 0);
	i = __inbyte(0x60);
	i--;

	if (i < 0x80) {
		switch (i) {
			case 0x29:
				lshift = 1;
				break;

			case 0x35:
				rshift = 1;
				break;

			case 0x1C:
				ctrl = 1;
				break;

			case 0x37:
				alt = 1;
				break;

			default:
				if (ctrl && alt) {
					event_scancode((uint32_t)i * 4 + 3);
					conin_putchar(kbdmap[i * 4 + 3]);
				} else if (alt) {
					event_scancode((uint32_t)i * 4 + 2);
					conin_putchar(kbdmap[i * 4 + 2]);
				} else if (lshift || rshift) {
					event_scancode((uint32_t)i * 4 + 1);
					conin_putchar(kbdmap[i * 4 + 1]);
				} else {
					event_scancode((uint32_t)i * 4);
					conin_putchar(kbdmap[i * 4]);
				}

				break;
		}
	} else {
		i -= 0x80;
		switch (i) {
			case 0x29:
				lshift = 0;
				break;

			case 0x35:
				rshift = 0;
				break;

			case 0x1C:
				ctrl = 0;
				break;

			case 0x37:
				alt = 0;
				break;
		}
	}

	__outbyte(0x20, 0x20);
	__outbyte(0xA0, 0x20);
}