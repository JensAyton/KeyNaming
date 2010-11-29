/*	TestCase.c
	
	Test case for KeyToQWERTY.
	
	
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


#import "KeyFromQWERTY.h"
#import <Foundation/Foundation.h>
#import <Carbon/Carbon.h>
#import "Utilities.h"
#import "JAKeyTranslator.h"

/*	Test for constant expressions – a dynamic-sized struct can’t be declared
	here, so if this compiles KEY_FROM_QWERTY() is being treated as a true
	constant expression. This won’t work in compilers without
	__builtin_constant_p().
*/
#if KEY_FROM_QWERTY_USE_BUILTIN_CONSTANT_P
typedef struct { char array[KEY_FROM_QWERTY('b')]; } ConstantExpressionTest;
#else
#warning Unknown compiler, __builtin_constant_p() not being used.
#endif


int main(int argc, const char * argv[])
{
	char testChars[] =
	{
		'a', 'b', 'c', 'D', 'E', '0', '1', '[', '\\'
	};
	
	JAKeyTranslator			*keyTranslator = nil;
	unsigned				i, count = sizeof testChars / sizeof testChars[0];
	char					cc;
	uint8_t					vkc;
	
	[[NSAutoreleasePool alloc] init];
	
	keyTranslator = [JAKeyTranslator keyTranslatorForCurrentKeyboardLayout];
	
	/*	Test for constant expressions: – KeyFromQWERTYDynamic should not
		appear before the first Print in disassembly, even in unoptimized
		builds.
	*/
	vkc = KEY_FROM_QWERTY('a');
	Print(@"Constant expression VKC: %u\n", vkc);
	
	/*	Test for constant folding – KeyFromQWERTYDynamic should not appear
		before the second Print in disassembly in optimized builds.
	*/
	const char kTestConst = 'a';
	vkc = KEY_FROM_QWERTY(kTestConst);
	Print(@"Hopefully constant-folded VKC: %u\n", vkc);
	
	/*	Side effect safety test: this should print the vkc for 'b',
		incrementing it only once.
	*/
	cc = 'a';
	vkc = KEY_FROM_QWERTY(++cc);
	Print(@"Hopefully %u: %u\n\n", KEY_FROM_QWERTY('b'), vkc);
	
	for (i = 0; i != count; i++)
	{
		cc = testChars[i];
		vkc = KEY_FROM_QWERTY(cc);
		
		Print(@"%c -> %u -> %@\n", cc, vkc, [keyTranslator translatedKeyForVKC:vkc]);
	}
}
