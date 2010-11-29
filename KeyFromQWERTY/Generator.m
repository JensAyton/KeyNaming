/*	Generator.c
	
	Code to generate horrible selection expression for KeyFromQWERTY.h.
	
	
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


#import <Foundation/Foundation.h>
#import <Carbon/Carbon.h>
#import "Utilities.h"
#import "JAKeyTranslator.h"


static inline BOOL IsKeypadKey(uint16_t vkc);


//	#define GENERATE_KEYPAD 1
#if GENERATE_KEYPAD
#define KeyInRange(vkc) IsKeypadKey(vkc)
#else
#define KeyInRange(vkc) (!IsKeypadKey(vkc))
#endif


int main(int argc, const char * argv[])
{
	JAKeyTranslator			*keyTranslator = nil;
	unsigned				i;
	NSMutableDictionary		*mapping = nil;
	NSArray					*keys = nil;
	NSString				*keyDesc = nil;
	
	[[NSAutoreleasePool alloc] init];
	
	keyTranslator = [JAKeyTranslator keyTranslatorForCurrentKeyboardLayout];
	
	Print(@"#define KEY_FROM_QWERTY_LAYOUT_NAME\t\t\t\t\"%@\"\n", keyTranslator.layoutName);
	if (keyTranslator.layoutLanguageCode != nil)
	{
		Print(@"#define KEY_FROM_QWERTY_LAYOUT_LANGUAGE_CODE\t\"%@\"\n", keyTranslator.layoutLanguageCode);
	}
	if (keyTranslator.layoutIdentifier != nil)
	{
		Print(@"#define KEY_FROM_QWERTY_LAYOUT_IDENTIFIER\t\t%@\n\n", keyTranslator.layoutIdentifier);
	}
	Print(@"#define KEY_FROM_QWERTY_BODY(cc) \\\n");
	
	// Build dictionary of char -> VKC relationship, partly for sorting but
	// mostly because some values may occur more than once.
	mapping = [NSMutableDictionary dictionaryWithCapacity:128];
	for (i = 0; i != 0x80; ++i)
	{
		if (!KeyInRange(i))  continue;
		
		keyDesc = [keyTranslator translatedKeyForVKC:i];
		if (IsAcceptableKeyString(keyDesc))
		{
			[mapping setObject:[NSString stringWithFormat:@"0x%.2X", i] forKey:EscapeCharLiteral([keyDesc characterAtIndex:0])];
		}
		
		// Accept uppercase letters as aliases for lowercase equivalents
		keyDesc = keyDesc.uppercaseString;
		if (IsAcceptableKeyString(keyDesc))
		{
			[mapping setObject:[NSString stringWithFormat:@"0x%.2X", i] forKey:EscapeCharLiteral([keyDesc characterAtIndex:0])];
		}
	}
	
	// Print keys
	keys = [[mapping allKeys] sortedArrayUsingSelector:@selector(compare:)];
	for (NSString *key in keys)
	{
		Print(@"\t((cc == \'%@\') ? %@ : \\\n", key, [mapping objectForKey:key]);
	}
	
	// Close all the parentheses
	Print(@"\t0 \\\n\t");
	for (i = 0; i != mapping.count; ++i)
	{
		if ((i % 72) == 0 && i != 0)
		{
			if (i != 0)  Print(@"\\\n\t");
		}
		Print(@")");
	}
	
	return 0;
}


static inline BOOL IsKeypadKey(uint16_t vkc)
{
	switch (vkc)
	{
		// Virtual key codes that correspond to numerical keypad keys
		case 65:
		case 67:
		case 69:
		case 75:
		case 78:
		case 81:
		case 82:
		case 83:
		case 84:
		case 85:
		case 86:
		case 87:
		case 88:
		case 89:
		case 91:
		case 92:
			return YES;
	}
	
	return NO;
}
