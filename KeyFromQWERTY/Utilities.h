/*	Utilities.h
	
	Utility code for KeyToQWERTY, shared between generator and test case.
	
	
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


void Printv(NSString *format, va_list args);
void Print(NSString *format, ...);

void Fail(NSString *format, ...) __attribute__((noreturn));
void FailWithError(NSString *message, OSStatus err) __attribute__((noreturn));

BOOL IsAcceptableKey(uint32_t value);
BOOL IsAcceptableKeyString(NSString *string);
NSString *EscapeCharLiteral(char value);

// Not a very friendly function - exits if there's no KCHR data available
void **GetCurrentKCHR(NSString **outName);


typedef struct JAKeyTranslateData JAKeyTranslateData, *JAKeyTranslateDataRef;
