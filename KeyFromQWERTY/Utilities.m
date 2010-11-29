/*	Utilities.c
	
	
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


void Printv(NSString *format, va_list args)
{
	NSString *msg = [[NSString alloc] initWithFormat:format arguments:args];
	printf("%s", msg.UTF8String);
}


void Print(NSString *format, ...)
{
	va_list args;
	va_start (args, format);
	Printv(format, args);
	va_end(args);
}


inline void Fail(NSString *format, ...)
{
	va_list args;
	va_start (args, format);
	Printv(format, args);
	va_end(args);
	
	exit(EXIT_FAILURE);
}


void FailWithError(NSString *message, OSStatus err)
{
	Fail(@"%@ (Error: %i, %s)\n", message, err, GetMacOSStatusErrorString(err));
}


BOOL IsAcceptableKey(uint32_t value)
{
	if (value >= 128)  return NO;	// Not ASCII
	if (value == 0x10)  return NO;	// DLE, apparently used for unassigned keys
	return YES;
}


BOOL IsAcceptableKeyString(NSString *string)
{
	if (string.length != 1)  return NO;
	return IsAcceptableKey([string characterAtIndex:0]);
}


NSString *EscapeCharLiteral(char value)
{
	
	switch (value)
	{
		case '\a': return @"\\a";
		case '\b': return @"\\b";
		case '\t': return @"\\t";
		case '\n': return @"\\n";
		case '\v': return @"\\v";
		case '\f': return @"\\f";
		case '\r': return @"\\r";
		case '\'': return @"\\'";
		case '\\': return @"\\\\";
	}
	if (value >= ' ' && value < 0x7F)  return [NSString stringWithFormat:@"%c", value];
	return [NSString stringWithFormat:@"\\x%.2X", value];
}


void **GetCurrentKCHR(NSString **outName)
{
	OSStatus				err;
	void					*result = NULL;
	NSString				*name = nil;
	KeyboardLayoutRef		layout = NULL;
	
	// Get current keyboard layout
	err = KLGetCurrentKeyboardLayout(&layout);
	if (err != noErr)  FailWithError(@"Could not get keyboard layout.", err);
	
	// Get keyboard layout name
	err = KLGetKeyboardLayoutProperty(layout, kKLName, (const void **)&name);
	[name autorelease];
	if (name == nil)  name = @"<error getting name>";
	
	// Get key mapping data
	err = KLGetKeyboardLayoutProperty(layout, kKLKCHRData, (const void **)&result);
	if (err != noErr || result == NULL)
	{
		if (err == resNotFound)  Fail(@"The keyboard layout \"%@\" is Unicode-only. Only old-style keyboard layouts are supported by this hack.\n", name);
		FailWithError(@"Could not get KCHR data.", err);
	}
	
	if (outName != NULL)  *outName = name;
	return result;
}
