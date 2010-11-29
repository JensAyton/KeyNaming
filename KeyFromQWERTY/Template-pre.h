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
