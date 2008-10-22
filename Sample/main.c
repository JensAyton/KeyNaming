/*
	This is, I admit, a particularly bad piece of sample code.
	
	Run it from within XCode. In the run log, press keys and see them described. When you get bored,
	click Terminate. You could try some different keyboard layouts, except the XCode run log shows
	Unicode stuff as Losless ASCII.
*/

#include <Carbon/Carbon.h>
#include <stdlib.h>
#include "KeyNaming.h"


#define KEYMAP_GET(m, index) !!(((UInt8*)(m))[(index) >> 3] & (1L << ((index) & 7)))


typedef struct
{
	UnsignedWide		uw;
	unsigned long long	ull;
} U64;


static void GetInfo(void);


int main(void)
{
	KeyMap				map, oldMap = {0};
	CFStringRef			keyName, string;
	U64					absTime, delta;
	
	absTime.uw = UpTime();
	delta.ull = 50000000;	// 1/20 secs
	delta.uw = NanosecondsToAbsolute(delta.uw);
	
	GetInfo();
	
	for (;;)
	{
		UInt16 i;
		
		GetKeys(map);
		for (i = 0; i < 128; i++)
		{
			if (KEYMAP_GET(map, i) && !KEYMAP_GET(oldMap, i))
			{
				keyName = KeyNamingCopyOneKeyName(i);
				string = CFStringCreateWithFormat(NULL, NULL, CFSTR("0x%.2X (%i) = %@\n"), i, i, keyName);
				if (NULL != string)
				{
					CFShow(string);
					CFRelease(string);
				}
				if (NULL != keyName) CFRelease(keyName);
			}
		}
		
		BlockMoveData(map, oldMap, sizeof(KeyMap));
		
		absTime.ull += delta.ull;
		MPDelayUntil(&absTime.uw);
	}
	
	return EXIT_SUCCESS;
}


static CFStringRef GetKeyboardLayoutName(void);


static void GetInfo(void)
{
	CFStringRef layoutName = GetKeyboardLayoutName();
	if (layoutName != NULL)  CFShow(layoutName);
}


#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
static CFStringRef GetKeyboardLayoutName(void)
{
	TISInputSourceRef	inputSource = NULL;
	CFStringRef			layoutName = NULL;
	
	inputSource = TISCopyCurrentKeyboardLayoutInputSource();
	if (inputSource != NULL)  layoutName = TISGetInputSourceProperty(inputSource, kTISPropertyLocalizedName);
	
	return layoutName;
}
#else
static CFStringRef GetKeyboardLayoutName(void)
{
	OSStatus			err;
	KeyboardLayoutRef	layout = NULL;
	CFStringRef			layoutName = NULL;
	
	err = KLGetCurrentKeyboardLayout(&layout);
	if (!err)  err = KLGetKeyboardLayoutProperty(layout, kKLLocalizedName, (const void **)&layoutName);
	if (err)  layoutName = NULL;
	
	return layoutName;
}
#endif
