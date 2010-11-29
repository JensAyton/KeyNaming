/*	KeyFromQWERTY.h
	
	Macros to convert ASCII characters to virtual key codes, using a fixed
	keyboard layout. This *explicitly does not* use the active keyboard
	layout.
	
	Two macros intended for public consumption are provided:
		KEY_FROM_QWERTY(charCode)
		KEY_FROM_NUMPAD(charCode)
	
	The distinction is needed because KEY_FROM_QWERTY('1') returns the VKC for
	the 1 key on the main part of the keyboard.
	
	If passed a constant-foldable expression, the macros are themselves
	constant expressions (assuming you’re using gcc or clang).
	
	Additionally, the following macros provide information about the keyboard
	layout used to generate the header:
		KEY_FROM_QWERTY_LAYOUT_NAME
		KEY_FROM_QWERTY_LAYOUT_LANGUAGE_CODE
		KEY_FROM_QWERTY_LAYOUT_IDENTIFIER
	These should be "U.S.", "en_US" and 0 in the distributed version, but you
	can build a version for any keyboard layout using the provided tools.
	
	
	© 2008 Jens Ayton
	
	Permission is hereby granted, free of charge, to any person obtaining a
	copy of this software and associated documentation files (the "Software"),
	to deal in the Software without restriction, including without limitation
	the rights to use, copy, modify, merge, publish, distribute, sublicense,
	and/or sell copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
	DEALINGS IN THE SOFTWARE.
*/

#ifndef INCLUDED_KEYFROMQWERTY_h
#define INCLUDED_KEYFROMQWERTY_h

#include <stdint.h>


#ifndef KEY_FROM_QWERTY_USE_BUILTIN_CONSTANT_P
#if defined(__GNUC__) || defined(__clang__)
#define KEY_FROM_QWERTY_USE_BUILTIN_CONSTANT_P	1
#else
#define KEY_FROM_QWERTY_USE_BUILTIN_CONSTANT_P	0
#endif
#endif


#if KEY_FROM_QWERTY_USE_BUILTIN_CONSTANT_P
#define KEY_FROM_QWERTY(cc) \
	(__builtin_constant_p(cc) ? \
		KEY_FROM_QWERTY_BODY(cc) : \
		KeyFromQWERTYDynamic(cc))
#else
#define KEY_FROM_QWERTY(cc) KeyFromQWERTYDynamic(cc)
#endif

uint8_t KeyFromQWERTYDynamic(char cc);


#if KEY_FROM_QWERTY_USE_BUILTIN_CONSTANT_P
#define KEY_FROM_NUMPAD(cc) \
	(__builtin_constant_p(cc) ? \
		KEY_FROM_NUMPAD_BODY(cc) : \
		KeyFromNumpadDynamic(cc))
#else
#define KEY_FROM_NUMPAD(cc) KeyFromNumpadDynamic(cc)
#endif

uint8_t KeyFromNumpadDynamic(char cc);


/*	Note that the use of the ternary operator makes KEY_FROM_QWERTY_BODY()
	a constant expresion, if cc is a constant expression.
	
	NOTE: beyond this point, this header is automatically generated.
*/
#define KEY_FROM_QWERTY_LAYOUT_NAME				"U.S."
#define KEY_FROM_QWERTY_LAYOUT_LANGUAGE_CODE	"en_US"
#define KEY_FROM_QWERTY_LAYOUT_IDENTIFIER		0

#define KEY_FROM_QWERTY_BODY(cc) \
	((cc == ' ') ? 0x31 : \
	((cc == ',') ? 0x2B : \
	((cc == '-') ? 0x1B : \
	((cc == '.') ? 0x2F : \
	((cc == '/') ? 0x2C : \
	((cc == '0') ? 0x1D : \
	((cc == '1') ? 0x12 : \
	((cc == '2') ? 0x13 : \
	((cc == '3') ? 0x14 : \
	((cc == '4') ? 0x15 : \
	((cc == '5') ? 0x17 : \
	((cc == '6') ? 0x16 : \
	((cc == '7') ? 0x1A : \
	((cc == '8') ? 0x1C : \
	((cc == '9') ? 0x19 : \
	((cc == ';') ? 0x29 : \
	((cc == '=') ? 0x18 : \
	((cc == 'A') ? 0x00 : \
	((cc == 'B') ? 0x0B : \
	((cc == 'C') ? 0x08 : \
	((cc == 'D') ? 0x02 : \
	((cc == 'E') ? 0x0E : \
	((cc == 'F') ? 0x03 : \
	((cc == 'G') ? 0x05 : \
	((cc == 'H') ? 0x04 : \
	((cc == 'I') ? 0x22 : \
	((cc == 'J') ? 0x26 : \
	((cc == 'K') ? 0x28 : \
	((cc == 'L') ? 0x25 : \
	((cc == 'M') ? 0x2E : \
	((cc == 'N') ? 0x2D : \
	((cc == 'O') ? 0x1F : \
	((cc == 'P') ? 0x23 : \
	((cc == 'Q') ? 0x0C : \
	((cc == 'R') ? 0x0F : \
	((cc == 'S') ? 0x01 : \
	((cc == 'T') ? 0x11 : \
	((cc == 'U') ? 0x20 : \
	((cc == 'V') ? 0x09 : \
	((cc == 'W') ? 0x0D : \
	((cc == 'X') ? 0x07 : \
	((cc == 'Y') ? 0x10 : \
	((cc == 'Z') ? 0x06 : \
	((cc == '[') ? 0x21 : \
	((cc == '\'') ? 0x27 : \
	((cc == '\\') ? 0x2A : \
	((cc == '\b') ? 0x33 : \
	((cc == '\f') ? 0x79 : \
	((cc == '\r') ? 0x24 : \
	((cc == '\t') ? 0x30 : \
	((cc == '\v') ? 0x74 : \
	((cc == '\x01') ? 0x73 : \
	((cc == '\x03') ? 0x4C : \
	((cc == '\x04') ? 0x77 : \
	((cc == '\x05') ? 0x72 : \
	((cc == '\x1B') ? 0x47 : \
	((cc == '\x1C') ? 0x7B : \
	((cc == '\x1D') ? 0x7C : \
	((cc == '\x1E') ? 0x7E : \
	((cc == '\x1F') ? 0x7D : \
	((cc == '\x7F') ? 0x75 : \
	((cc == ']') ? 0x1E : \
	((cc == '`') ? 0x32 : \
	((cc == 'a') ? 0x00 : \
	((cc == 'b') ? 0x0B : \
	((cc == 'c') ? 0x08 : \
	((cc == 'd') ? 0x02 : \
	((cc == 'e') ? 0x0E : \
	((cc == 'f') ? 0x03 : \
	((cc == 'g') ? 0x05 : \
	((cc == 'h') ? 0x04 : \
	((cc == 'i') ? 0x22 : \
	((cc == 'j') ? 0x26 : \
	((cc == 'k') ? 0x28 : \
	((cc == 'l') ? 0x25 : \
	((cc == 'm') ? 0x2E : \
	((cc == 'n') ? 0x2D : \
	((cc == 'o') ? 0x1F : \
	((cc == 'p') ? 0x23 : \
	((cc == 'q') ? 0x0C : \
	((cc == 'r') ? 0x0F : \
	((cc == 's') ? 0x01 : \
	((cc == 't') ? 0x11 : \
	((cc == 'u') ? 0x20 : \
	((cc == 'v') ? 0x09 : \
	((cc == 'w') ? 0x0D : \
	((cc == 'x') ? 0x07 : \
	((cc == 'y') ? 0x10 : \
	((cc == 'z') ? 0x06 : \
	0 \
	))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))\
	)))))))))))))))))


#define KEY_FROM_NUMPAD_BODY(cc) \
	((cc == '*') ? 0x43 : \
	((cc == '+') ? 0x45 : \
	((cc == '-') ? 0x4E : \
	((cc == '.') ? 0x41 : \
	((cc == '/') ? 0x4B : \
	((cc == '0') ? 0x52 : \
	((cc == '1') ? 0x53 : \
	((cc == '2') ? 0x54 : \
	((cc == '3') ? 0x55 : \
	((cc == '4') ? 0x56 : \
	((cc == '5') ? 0x57 : \
	((cc == '6') ? 0x58 : \
	((cc == '7') ? 0x59 : \
	((cc == '8') ? 0x5B : \
	((cc == '9') ? 0x5C : \
	((cc == '=') ? 0x51 : \
	0 \
	))))))))))))))))
	

#endif	/* INCLUDED_KEYFROMQWERTY_h */
