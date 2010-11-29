/*	JAKeyTranslator.h
	
	Somewhat simplistic wrapper around UCKeyTranslate and KeyTranslate.
	
	
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

#import <Cocoa/Cocoa.h>


@interface JAKeyTranslator: NSObject
{
	struct OpaqueKeyboardLayoutRef	*_layout;
	void							*_translateData;
	NSString						*_name;
	NSString						*_langCode;
	int32_t							_identifier;
#ifndef __LP64__
	BOOL							_isUchr;
	BOOL							_hasIdentifier;
#endif
}

+ (id) keyTranslatorForCurrentKeyboardLayout;

- (id) initWithKeyboardLayout:(struct OpaqueKeyboardLayoutRef *)layout;

- (NSString *) translatedKeyForVKCSequence:(uint16_t *)vkcs count:(NSUInteger)count;
- (NSString *) translatedKeyForVKCSequence:(NSArray *)vkcsAsNumbers;
- (NSString *) translatedKeyForVKC:(uint16_t)vkc;

- (NSString *) layoutName;
- (NSString *) layoutLanguageCode;
- (NSNumber *) layoutIdentifier;	// May be nil.

@end
