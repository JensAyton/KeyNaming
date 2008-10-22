/*
	KeyNaming.cp
	Keynaming 2.2 implementation
	© 2001-2008 Jens Ayton <jens@ayton.se>, except where otherwise noted.
	
	Copyright © 2001–2008 Jens Ayton
 
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/


#include "KeyNaming.h"
#include <Carbon/Carbon.h>


#ifdef KEYNAMING_HID
	#warning "KEYNAMING_HID has been renamed KEYNAMING_ENABLE_HID."
#endif


#pragma mark * Constants

#define	kKey_Command			CFSTR("Command")
#define	kKey_Shift				CFSTR("Shift")
#define	kKey_Option				CFSTR("Option")
#define	kKey_Control			CFSTR("Control")
#define	kKey_RightCommand		CFSTR("Right Command")
#define	kKey_RightShift			CFSTR("Right Shift")
#define	kKey_RightOption		CFSTR("Right Option")
#define	kKey_RightControl		CFSTR("Right Control")
#define	kKey_Caps				CFSTR("Caps Lock")
#define	kKey_Tab				CFSTR("Tab")
#define	kKey_Space				CFSTR("Space")
#define	kKey_Left				CFSTR("Left Arrow")
#define	kKey_Right				CFSTR("Right Arrow")
#define	kKey_Up					CFSTR("Up Arrow")
#define	kKey_Down				CFSTR("Down Arrow")
#define	kKey_Clear				CFSTR("Clear")
#define	kKey_PgUp				CFSTR("Page Up")
#define	kKey_PgDn				CFSTR("Page Down")
#define	kKey_Home				CFSTR("Home")
#define	kKey_End				CFSTR("End")
#define	kKey_Esc				CFSTR("Escape")
#define	kKey_Return				CFSTR("Return")
#define	kKey_Enter				CFSTR("Enter")
#define	kKey_Backspace			CFSTR("Backspace")
#define	kKey_Delete				CFSTR("Delete")
#define	kKey_Keypad_X			CFSTR("Keypad %@")
#define	kKey_F1					CFSTR("F1")
#define	kKey_F2					CFSTR("F2")
#define	kKey_F3					CFSTR("F3")
#define	kKey_F4					CFSTR("F4")
#define	kKey_F5					CFSTR("F5")
#define	kKey_F6					CFSTR("F6")
#define	kKey_F7					CFSTR("F7")
#define	kKey_F8					CFSTR("F8")
#define	kKey_F9					CFSTR("F9")
#define	kKey_F10				CFSTR("F10")
#define	kKey_F11				CFSTR("F11")
#define	kKey_F12				CFSTR("F12")
#define	kKey_F13				CFSTR("F13")
#define	kKey_F14				CFSTR("F14")
#define	kKey_F15				CFSTR("F15")
#define	kKey_F16				CFSTR("F16")
#define kKey_F17				CFSTR("F17")
#define kKey_F18				CFSTR("F18")
#define kKey_F19				CFSTR("F19")
#define kKey_F20				CFSTR("F20")
#define	kKey_Help				CFSTR("Help")
#define kKey_Fn					CFSTR("Fn")
#define kKey_Menu				CFSTR("Menu")
#define kKey_Mute				CFSTR("Mute")
#define kKey_VolumeDown			CFSTR("Volume Down")
#define kKey_VolumeUp			CFSTR("Volume Up")

#if KEYNAMING_ENABLE_HID

#define	kKey_Unknown_N			CFSTR("Unknown (%x)")
#define kKey_RightCommand		CFSTR("Right Command")
#define kKey_Power				CFSTR("Power")
#define kKey_Eject				CFSTR("Eject")
#define kKey_F21				CFSTR("F21")
#define kKey_F22				CFSTR("F22")
#define kKey_F23				CFSTR("F23")
#define kKey_F24				CFSTR("F24")

#endif

#define kSeparator				CFSTR("  ")


static CFStringRef	sStringTable = NULL;
static CFBundleRef	sStringBundle = NULL;
static CFStringRef	sUnknownCodeString = NULL;

static CFStringRef UnknownCodeString(void);


#pragma mark * Virtual Key Codes

enum
{
	/*	Virtual key codes handled specially (from: Inside Macintosh: Text, Appendix C)
		
		Function keys. These are handled by loading the releveant string IFF the
		character code 10 (kCC_FKey) is generated. This allows the function keys
		to be remapped.
		
		The Menu/Application key on Windows keyboards also generates the character
		code 10 by default. It isn't immediately clear whether the VKC is used for
		an F key in other circumstances, but apps generally seem to interpret it as
		a normal key. For now, I'm defining it as Menu Key. It's possible Unix-
		oriented keyboards use it with a different key cap, e.g. Meta.
	*/
	kVKC_F1				= 122,
	kVKC_F2				= 120,
	kVKC_F3				= 99,
	kVKC_F4				= 118,
	kVKC_F5				= 96,
	kVKC_F6				= 97,
	kVKC_F7				= 98,
	kVKC_F8				= 100,
	kVKC_F9				= 101,
	kVKC_F10			= 109,
	kVKC_F11			= 103,
	kVKC_F12			= 111,
	kVKC_F13			= 105,
	kVKC_F14			= 107,
	kVKC_F15			= 113,
	kVKC_F16			= 106,
	kVKC_Menu			= 110,
	
	/*	Escape and clear are like the F keys, using the character code kCC_EscClr. */
	kVKC_Esc			= 53,
	kVKC_Clear			= 71,
	
	/*	The following are handled directy by recognising the virtual code. */
	kVKC_Space			= 49,
	kVKC_CapsLock		= 57,
	kVKC_Shift			= 56,
	kVKC_Option			= 58,
	kVKC_Control		= 59,
	kVKC_rShift			= 60,		/*	Right-hand modifiers; not implemented */
	kVKC_rOption		= 61,
	kVKC_rControl		= 62,
	kVKC_Command		= 55,
	kVKC_Return			= 36,
	kVKC_Backspace		= 51,		/*	Left delete */
	kVKC_Delete			= 117,		/*	right delete */
	kVKC_Help			= 114,
	kVKC_Home			= 115,
	kVKC_PgUp			= 116,
	kVKC_PgDn			= 121,
	kVKC_End			= 119,
	kVKC_LArrow			= 123,
	kVKC_RArrow			= 124,
	kVKC_UArrow			= 126,
	kVKC_DArrow			= 125,
	kVKC_KpdEnter		= 76,		/*	"normal" enter key */
	kVKC_KbdEnter		= 52,		/*	Powerbooks (and some early Macs) */
	kVKC_Fn				= 63,
	
	/*	Keypad keys. These are named by loading the string "Keypad %@" and
		replacing the %@ with the key's character name. The constant names
		do not correspond to the key caps in any KCHR that I'm aware of;
		they're just used to recognise the set of keys. Note that Enter and
		Clear aren't handled this way. */
	kVKC_Kpd_0			= 81,
	kVKC_Kpd_1			= 75,
	kVKC_Kpd_2			= 67,
	kVKC_Kpd_3			= 89,
	kVKC_Kpd_4			= 91,
	kVKC_Kpd_5			= 92,
	kVKC_Kpd_6			= 78,
	kVKC_Kpd_7			= 86,
	kVKC_Kpd_8			= 87,
	kVKC_Kpd_9			= 88,
	kVKC_Kpd_A			= 69,
	kVKC_Kpd_B			= 83,
	kVKC_Kpd_C			= 84,
	kVKC_Kpd_D			= 85,
	kVKC_Kpd_E			= 82,
	kVKC_Kpd_F			= 65,
	
	/* 2.1b5: values from new list in Event.h in OS X 10.5 */
	kVKC_VolumeUp		= 72,
	kVKC_VolumeDown		= 73,
	kVKC_Mute			= 74,
	
	kVKC_F17			= 64,
	kVKC_F18			= 79,
	kVKC_F19			= 80,
	kVKC_F20			= 90,
	
	#if KEYNAMING_ENABLE_HID
	
	/*	Fake VKCs. These are used for HID usages that, to my knowledge, have
		no corresponding VKC. I use the range 0x6000 up; GetKeys() can't return
		in this range, and other VKC usages probably won't. */
	kFKC_base_			= 0x6000,
	kFKC_rCommand,
/*	kFKC_Mute,			*/
/*	kFKC_VolumeDown,	*/
/*	kFKC_VolumeUp,		*/
	kFKC_Power			= 0x6005,
	kFKC_Eject,
/*	kFKC_F17,			*/
/*	kFKC_F18,			*/
/*	kFKC_F19,			*/
/*	kFKC_F20,			*/
	kFKC_F21			= 0x600B,
	kFKC_F22,
	kFKC_F23,
	kFKC_F24,
	
	#endif
	
	kEnumAtTheEndToSatisfyIrritableCompilers
};


#pragma mark * Character Codes
enum
{
	/*	Character codes. */
	kCC_FKey			= 16,	/* see above section on F-keys */
	kCC_EscClr			= 27,	/* see above section on escape and clear */
	
	/*	These are handled by loading the corresponding string. */
	kCC_Tab				= 9,
	kCC_Space			= 32	/* Also recognised by VKC */
};


enum
{	
	/* Mode constants for Name1Key */
	kNoSpecialHandling = 0,
	kEscapeClearHandling,
	kFKeyHandling,
	kKeypadHandling,
	kUnknownHandling,
	
	kHandlingModeCount
};


#if KEYNAMING_ENABLE_HID

#pragma mark * HID to VKC Table

/*	Table mapping HID usages in the KeyboardOrKeypad page to virtual key codes.
	Names given for keys refer to US layout.
	May be copied freely.
*/
#define kVKC_Unknown		0xFFFF

static const uint16_t	kHID2VKC[] =
{
	kVKC_Unknown,		/* Reserved (no event indicated) */
	kVKC_Unknown,		/* ErrorRollOver */
	kVKC_Unknown,		/* POSTFail */
	kVKC_Unknown,		/* ErrorUndefined */
	0x00,				/* a and A */
	0x0B,				/* b and B */
	0x08,				/* ... */
	0x02,
	0x0E,
	0x03,
	0x05,
	0x04,
	0x22,
	0x26,
	0x28,
	0x25,
	0x2E,
	0x2D,
	0x1F,
	0x23,
	0x0C,
	0x0F,
	0x01,
	0x11,
	0x20,
	0x09,
	0x0D,
	0x07,
	0x10,
	0x06,				/* z and Z */
	0x12,				/* 1 */
	0x13,				/* 2 */
	0x14,				/* ... */
	0x15,
	0x17,
	0x16,
	0x1A,
	0x1C,
	0x19,				/* 9 */
	0x1D,				/* 0 */
	kVKC_Return,		/* Keyboard Return (ENTER) */
	kVKC_Esc,			/* Escape */
	kVKC_Backspace,		/* Delete (Backspace) */
	0x30,				/* Tab */
	kVKC_Space,			/* Space bar */
	0x1B,				/* - and _ */
	0x18,				/* = and + */
	0x21,				/* [ and { */
	0x1E,				/* ] and } */
	0x2A,				/* \ and | */
	kVKC_Unknown,		/* "Non-US # and ~" ?? */
	0x29,				/* ; and : */
	0x27,				/* ' and " */
	0x32,				/* ` and ~ */
	0x2B,				/* , and < */
	0x2F,				/* . and > */
	0x2C,				/* / and ? */
	kVKC_CapsLock,		/* Caps Lock */
	kVKC_F1,			/* F1 */
	kVKC_F2,			/* ... */
	kVKC_F3,
	kVKC_F4,
	kVKC_F5,
	kVKC_F6,
	kVKC_F7,
	kVKC_F8,
	kVKC_F9,
	kVKC_F10,
	kVKC_F11,
	kVKC_F12,			/* F12 */
	kVKC_Unknown,		/* Print Screen */
	kVKC_Unknown,		/* Scroll Lock */
	kVKC_Unknown,		/* Pause */
	kVKC_Unknown,		/* Insert */
	kVKC_Home,			/* Home */
	kVKC_PgUp,			/* Page Up */
	kVKC_Delete,		/* Delete Forward */
	kVKC_End,			/* End */
	kVKC_PgDn,			/* Page Down */
	kVKC_RArrow,		/* Right Arrow */
	kVKC_LArrow,		/* Left Arrow */
	kVKC_DArrow,		/* Down Arrow */
	kVKC_UArrow,		/* Up Arrow */
	kVKC_Clear,			/* Keypad Num Lock and Clear */
	0x4B,				/* Keypad / */
	0x43,				/* Keypad * */
	0x4E,				/* Keypad - */
	0x45,				/* Keypad + */
	kVKC_KpdEnter,		/* Keypad ENTER */
	0x53,				/* Keypad 1 */
	0x54,				/* Keypad 2 */
	0x55,				/* Keypad 3 */
	0x56,				/* Keypad 4 */
	0x57,				/* Keypad 5 */
	0x58,				/* Keypad 6 */
	0x59,				/* Keypad 7 */
	0x5B,				/* Keypad 8 */
	0x5C,				/* Keypad 9 */
	0x52,				/* Keypad 0 */
	0x41,				/* Keypad . */
	0x32,				/* "Keyboard Non-US \ and |" */
	kVKC_Unknown,		/* "Keyboard Application" (Windows key for Windows 95, and "Compose".) */
	kFKC_Power,			/* Keyboard Power (status, not key... but Apple doesn't seem to have read the spec properly) */
	0x51,				/* Keypad = */
	kVKC_F13,			/* F13 */
	kVKC_F14,			/* ... */
	kVKC_F15,
	kVKC_F16,
	kVKC_F17,
	kVKC_F18,
	kVKC_F19,
	kVKC_F20,
	kFKC_F21,
	kFKC_F22,
	kFKC_F23,
	kFKC_F24,			/* F24 */
	kVKC_Unknown,		/* Keyboard Execute */
	kVKC_Help,			/* Keyboard Help */
	kVKC_Unknown,		/* Keyboard Menu */
	kVKC_Unknown,		/* Keyboard Select */
	kVKC_Unknown,		/* Keyboard Stop */
	kVKC_Unknown,		/* Keyboard Again */
	kVKC_Unknown,		/* Keyboard Undo */
	kVKC_Unknown,		/* Keyboard Cut */
	kVKC_Unknown,		/* Keyboard Copy */
	kVKC_Unknown,		/* Keyboard Paste */
	kVKC_Unknown,		/* Keyboard Find */
	kVKC_Mute,			/* Keyboard Mute */
	kVKC_VolumeUp,		/* Keyboard Volume Up */
	kVKC_VolumeDown,	/* Keyboard Volume Down */
	kVKC_CapsLock,		/* Keyboard Locking Caps Lock */
	kVKC_Unknown,		/* Keyboard Locking Num Lock */
	kVKC_Unknown,		/* Keyboard Locking Scroll Lock */
	0x41,				/*	Keypad Comma ("Keypad Comma is the appropriate usage for the Brazilian
							keypad period (.) key. This represents the closest possible  match, and
							system software should do the correct mapping based on the current locale
							setting." If strange stuff happens on a (physical) Brazilian keyboard,
							I'd like to know about it. */
	0x51,				/* Keypad Equal Sign ("Used on AS/400 Keyboards.") */
	kVKC_Unknown,		/* Keyboard International1 (Brazilian / and ? key? Kanji?) */
	kVKC_Unknown,		/* Keyboard International2 (Kanji?) */
	kVKC_Unknown,		/* Keyboard International3 (Kanji?) */
	kVKC_Unknown,		/* Keyboard International4 (Kanji?) */
	kVKC_Unknown,		/* Keyboard International5 (Kanji?) */
	kVKC_Unknown,		/* Keyboard International6 (Kanji?) */
	kVKC_Unknown,		/* Keyboard International7 (Kanji?) */
	kVKC_Unknown,		/* Keyboard International8 (Kanji?) */
	kVKC_Unknown,		/* Keyboard International9 (Kanji?) */
	kVKC_Unknown,		/* Keyboard LANG1 (Hangul/English toggle) */
	kVKC_Unknown,		/* Keyboard LANG2 (Hanja conversion key) */
	kVKC_Unknown,		/* Keyboard LANG3 (Katakana key) */		// kVKC_Kana?
	kVKC_Unknown,		/* Keyboard LANG4 (Hirigana key) */
	kVKC_Unknown,		/* Keyboard LANG5 (Zenkaku/Hankaku key) */
	kVKC_Unknown,		/* Keyboard LANG6 */
	kVKC_Unknown,		/* Keyboard LANG7 */
	kVKC_Unknown,		/* Keyboard LANG8 */
	kVKC_Unknown,		/* Keyboard LANG9 */
	kVKC_Unknown,		/* Keyboard Alternate Erase ("Example, Erase-Eaze™ key.") */
	kVKC_Unknown,		/* Keyboard SysReq/Attention */
	kVKC_Unknown,		/* Keyboard Cancel */
	kVKC_Unknown,		/* Keyboard Clear */
	kVKC_Unknown,		/* Keyboard Prior */
	kVKC_Unknown,		/* Keyboard Return */
	kVKC_Unknown,		/* Keyboard Separator */
	kVKC_Unknown,		/* Keyboard Out */
	kVKC_Unknown,		/* Keyboard Oper */
	kVKC_Unknown,		/* Keyboard Clear/Again */
	kVKC_Unknown,		/* Keyboard CrSel/Props */
	kVKC_Unknown,		/* Keyboard ExSel */
	kVKC_Unknown,		/* Reserved */
	kVKC_Unknown,		/* Reserved */
	kVKC_Unknown,		/* Reserved */
	kVKC_Unknown,		/* Reserved */
	kVKC_Unknown,		/* Reserved */
	kVKC_Unknown,		/* Reserved */
	kVKC_Unknown,		/* Reserved */
	kVKC_Unknown,		/* Reserved */
	kVKC_Unknown,		/* Reserved */
	kVKC_Unknown,		/* Reserved */
	kVKC_Unknown,		/* Reserved */
	kVKC_Unknown,		/* Keypad 00 */
	kVKC_Unknown,		/* Keypad 000 */
	kVKC_Unknown,		/* Thousands Separator */
	kVKC_Unknown,		/* Decimal Separator */
	kVKC_Unknown,		/* Currency Unit */
	kVKC_Unknown,		/* Currency Sub-unit */
	kVKC_Unknown,		/* Keypad ( */
	kVKC_Unknown,		/* Keypad ) */
	kVKC_Unknown,		/* Keypad { */
	kVKC_Unknown,		/* Keypad } */
	kVKC_Unknown,		/* Keypad Tab */
	kVKC_Unknown,		/* Keypad Backspace */
	kVKC_Unknown,		/* Keypad A */
	kVKC_Unknown,		/* Keypad B */
	kVKC_Unknown,		/* Keypad C */
	kVKC_Unknown,		/* Keypad D */
	kVKC_Unknown,		/* Keypad E */
	kVKC_Unknown,		/* Keypad F */
	kVKC_Unknown,		/* Keypad XOR */
	kVKC_Unknown,		/* Keypad ^ */
	kVKC_Unknown,		/* Keypad % */
	kVKC_Unknown,		/* Keypad < */
	kVKC_Unknown,		/* Keypad > */
	kVKC_Unknown,		/* Keypad & */
	kVKC_Unknown,		/* Keypad && */
	kVKC_Unknown,		/* Keypad | */
	kVKC_Unknown,		/* Keypad || */
	kVKC_Unknown,		/* Keypad : */
	kVKC_Unknown,		/* Keypad # */
	kVKC_Unknown,		/* Keypad Space */
	kVKC_Unknown,		/* Keypad @ */
	kVKC_Unknown,		/* Keypad ! */
	kVKC_Unknown,		/* Keypad Memory Store */
	kVKC_Unknown,		/* Keypad Memory Recall */
	kVKC_Unknown,		/* Keypad Memory Clear */
	kVKC_Unknown,		/* Keypad Memory Add */
	kVKC_Unknown,		/* Keypad Memory Subtract */
	kVKC_Unknown,		/* Keypad Memory Multiply */
	kVKC_Unknown,		/* Keypad Memory Divide */
	kVKC_Unknown,		/* Keypad +/- */
	kVKC_Unknown,		/* Keypad Clear */
	kVKC_Unknown,		/* Keypad Clear Entry */
	kVKC_Unknown,		/* Keypad Binary */
	kVKC_Unknown,		/* Keypad Octal */
	kVKC_Unknown,		/* Keypad Decimal */
	kVKC_Unknown,		/* Keypad Hexadecimal */
	kVKC_Unknown,		/* Reserved */
	kVKC_Unknown,		/* Reserved */
	kVKC_Control,		/* Keyboard LeftControl */
	kVKC_Shift,			/* Keyboard LeftShift */
	kVKC_Option,		/* Keyboard LeftAlt */
	kVKC_Command,		/* Keyboard LeftGUI */
	kVKC_rControl,		/* Keyboard RightControl */
	kVKC_rShift,		/* Keyboard RightShift */
	kVKC_rOption,		/* Keyboard RightAlt */
	kFKC_rCommand		/* Keyboard RightGUI */
};


enum { kHID2VKCSize = sizeof kHID2VKC / sizeof kHID2VKC[0] };

#endif // KEYNAMING_ENABLE_HID


#pragma mark * Code


#define CopyLocalizedString(str) CFBundleCopyLocalizedString(sStringBundle, str, str, sStringTable)


enum
{
	kResType_UCKeyboardLayout		= 'uchr'
};


/*	If making state cache public, remember to refresh it when the input method is changed
	(check GetScriptVariable(GetScriptManagerVariable(smKeyScript), smScriptKeys), or
	just refresh it before handling relevant events). It's pretty lightweight anyway.
	
	TODO: use lots of system version checking and dynamic loading to do this using the
	non-deprecated path on each system revision, or at least not get warnings even if
	there is no non-deprecated way.
*/
typedef struct StateCache
{
	Ptr					KCHR;			/* Old-style keyboard mapping for KeyTranslate() */
	UCKeyboardLayout	*uchr;			/* New-style keyboard mapping for UCKeyTranslate() */
	TextEncoding		encoding;		/* Encoding for converting KeyTranslate() chars to CFString */
	uint8_t				keyboardType;	/* Magic Number needed by UCKeyTranslate() */
} StateCache;


static void InitStateCache(StateCache *outState)
{
	Boolean				doneWithKL = false;
	KeyboardLayoutRef	layout;
	Handle				rsrc;
	ResID				keyLayoutID;
	SInt16				keyScript;
	
	if (NULL == sStringTable)
	{
		sStringTable = CFSTR("KeyNaming");
		CFRetain(sStringTable);
	}
	if (NULL == sStringBundle)
	{
		sStringBundle = CFBundleGetMainBundle();
		CFRetain(sStringBundle);
	}
	
	if (NULL != KLGetCurrentKeyboardLayout)
	{
		/* Mac OS X 10.2 Keyboard Layout Services */
		if (!KLGetCurrentKeyboardLayout(&layout))
		{
			KLGetKeyboardLayoutProperty(layout, kKLKCHRData, (const void **)&outState->KCHR);
			KLGetKeyboardLayoutProperty(layout, kKLuchrData, (const void **)&outState->uchr);
			
			if (!outState->KCHR) outState->KCHR = (Ptr)GetScriptManagerVariable(smKCHRCache);
			
			doneWithKL = true;
		}
	}
	
	if (!doneWithKL)
	{
		outState->KCHR = (Ptr)GetScriptManagerVariable(smKCHRCache);
		
		/* See IM:Supporting Unicode Input */
		keyScript = GetScriptManagerVariable(smKeyScript);
		keyLayoutID = GetScriptVariable(keyScript, smScriptKeys);
		rsrc = GetResource(kResType_UCKeyboardLayout, keyLayoutID);
		
		/* It's OK if this is NULL, we'll fall back on KeyTranslate */
		outState->uchr = *(UCKeyboardLayout	**)rsrc;
	}
	outState->encoding = CreateTextEncoding(GetScriptManagerVariable(smKeyScript), kTextEncodingDefaultVariant, kTextEncodingDefaultFormat);
	outState->keyboardType = LMGetKbdType();
}


static OSStatus NameOneKey(uint16_t inVKeyCode, CFStringRef *outString, const StateCache *inState, uint8_t *outHandlingMode);


#if KEYNAMING_ENABLE_VKC

CFStringRef KeyNamingCopyOneKeyName(uint16_t inVirtualKeyCode)
{
	OSStatus			err = noErr;
	CFStringRef			result = NULL;
	StateCache			state;
	
	InitStateCache(&state);
	err = NameOneKey(inVirtualKeyCode, &result, &state, NULL);
	
	if (err && NULL != result)
	{
		CFRelease(result);
		result = NULL;
	}
	
	return result;
}


CFStringRef KeyNamingCopyKeyNames(uint32_t inCount, const uint16_t * const inVirtualKeyCodes)
{
	OSStatus			err = noErr;
	uint32_t			iter;
	CFMutableStringRef	result = NULL;
	CFStringRef			curr;
	StateCache			state;
	
	if (0 == inCount) return CFSTR("");
	if (NULL == inVirtualKeyCodes) return NULL;
	
	result = CFStringCreateMutable(kCFAllocatorDefault, 0);
	if (NULL == result) return NULL;
	
	InitStateCache(&state);
	
	for (iter = 0; iter != inCount; ++iter)
	{
		if (0 != iter)
		{
			CFStringAppend(result, kSeparator);
		}
		
		err = NameOneKey(inVirtualKeyCodes[iter], &curr, &state, NULL);
		if (err) break;
		
		CFStringAppend(result, curr);
		CFRelease(curr);
	}
	
	if (err && NULL != result)
	{
		CFRelease(result);
		result = NULL;
	}
	
	return result;
}


CFArrayRef KeyNamingCopyKeyNamesAsArray(uint32_t inCount, const uint16_t * const inVirtualKeyCodes)
{
	OSStatus			err = noErr;
	uint32_t			iter;
	CFMutableArrayRef	result = NULL;
	CFStringRef			curr;
	StateCache			state;
	
	if (0 != inCount && NULL == inVirtualKeyCodes) return NULL;
	
	result = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
	if (NULL == result) return NULL;
	if (0 == inCount) return result;
	
	InitStateCache(&state);
		
	for (iter = 0; iter != inCount; ++iter)
	{
		err = NameOneKey(inVirtualKeyCodes[iter], &curr, &state, NULL);
		if (err) break;
		
		CFArrayAppendValue(result, curr);
		CFRelease(curr);
	}
	
	if (err && NULL != result)
	{
		CFRelease(result);
		result = NULL;
	}
	
	return result;
}

#endif	/* KEYNAMING_ENABLE_VKC */


#if KEYNAMING_ENABLE_HID

CFStringRef KeyNamingCopyOneKeyNameHID(int32_t inUsage)
{
	OSStatus			err = noErr;
	CFStringRef			result = NULL;
	StateCache			state;
	uint16_t			vkc;
	
	if (inUsage < 0 || kHID2VKCSize <= inUsage) vkc = kVKC_Unknown;
	else vkc = kHID2VKC[inUsage];
	
	if (kVKC_Unknown != vkc)
	{
		InitStateCache(&state);
		err = NameOneKey(vkc, &result, &state, NULL);
		
		if (err && NULL != result)
		{
			CFRelease(result);
			result = NULL;
		}
	}
	
	if (NULL == result)
	{
		result = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, UnknownCodeString(), inUsage);
	}
	
	return result;
}


CFStringRef KeyNamingCopyKeyNamesHID(uint32_t inCount, const int32_t * const inUsages)
{
	OSStatus			err = noErr;
	uint32_t			iter;
	CFMutableStringRef	result = NULL;
	CFStringRef			curr;
	StateCache			state;
	int32_t				usage;
	uint16_t			vkc;
	
	if (0 == inCount) return CFSTR("");
	if (NULL == inUsages) return NULL;
	
	result = CFStringCreateMutable(kCFAllocatorDefault, 0);
	if (NULL == result) return NULL;
	
	InitStateCache(&state);
	
	for (iter = 0; iter != inCount; ++iter)
	{
		if (0 != iter)
		{
			CFStringAppend(result, kSeparator);
		}
		
		usage = inUsages[iter];
		if (usage < 0 || kHID2VKCSize <= usage) vkc = kVKC_Unknown;
		else vkc = kHID2VKC[usage];
		
		if (kVKC_Unknown != vkc)
		{
			err = NameOneKey(vkc, &curr, &state, NULL);
			
			if (err && NULL != curr)
			{
				CFRelease(curr);
				curr = NULL;
			}
		}
		
		if (NULL == curr)
		{
			curr = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, UnknownCodeString(), usage);
		}
		
		if (NULL != curr)
		{
			CFStringAppend(result, curr);
			CFRelease(curr);
		}
	}
	
	if (err && NULL != result)
	{
		CFRelease(result);
		result = NULL;
	}
	
	return result;
}


CFArrayRef KeyNamingCopyKeyNamesAsArrayHID(uint32_t inCount, const int32_t * const inUsages)
{
	OSStatus			err = noErr;
	uint32_t			iter;
	CFMutableArrayRef	result = NULL;
	CFStringRef			curr;
	StateCache			state;
	int32_t				usage;
	uint16_t			vkc;
	
	if (0 != inCount && NULL == inUsages) return NULL;
	
	result = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
	if (NULL == result) return NULL;
	if (0 == inCount) return result;
	
	InitStateCache(&state);
	
	for (iter = 0; iter != inCount; ++iter)
	{
		usage = inUsages[iter];
		if (usage < 0 || kHID2VKCSize <= usage) vkc = kVKC_Unknown;
		else vkc = kHID2VKC[usage];
		
		if (kVKC_Unknown != vkc)
		{
			err = NameOneKey(vkc, &curr, &state, NULL);
			
			if (err && NULL != curr)
			{
				CFRelease(curr);
				curr = NULL;
			}
		}
		
		if (NULL == curr)
		{
			curr = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, UnknownCodeString(), usage);
		}
		
		if (NULL != curr)
		{
			CFArrayAppendValue(result, curr);
			CFRelease(curr);
		}
	}
	
	if (err && NULL != result)
	{
		CFRelease(result);
		result = NULL;
	}
	
	return result;
}

#endif	/* KEYNAMING_ENABLE_HID */


static OSStatus NameOneKey(uint16_t inVKeyCode, CFStringRef *outString, const StateCache *inState, uint8_t *outHandlingMode)
{
	enum { kLength = 255 };
	UInt32				state,
						value;
	OSStatus			err = noErr;
	UInt8				charCode;
	CFStringRef			special = NULL;
	UInt16				handlingMode = kNoSpecialHandling;
	CFStringRef			result = NULL;
	CFMutableStringRef	tempStr;
	UniChar				string[kLength];
	UniCharCount		length;
	UInt8				pascalString[2];
	
	/*	Sanity checking is the responsibility of the caller. */
	
	/*	Get the character code */
	state = 0;
	value = KeyTranslate(inState->KCHR, inVKeyCode, &state);
	/*	Magic for dead keys (combining accent keys) */
	if (!value) value = KeyTranslate(inState->KCHR, inVKeyCode, &state);
	
	charCode = value & 0xFF;
	
	/*	Handle the various special cases. There are lots of special cases. */
	switch (charCode)
	{
		case kCC_FKey:			handlingMode = kFKeyHandling;			break;
		case kCC_EscClr:		handlingMode = kEscapeClearHandling;	break;
		case kCC_Tab:			special = kKey_Tab;						break;
		case kCC_Space:			special = kKey_Space;					break;
	}
	
	
	if (NULL == special && kNoSpecialHandling == handlingMode) switch (inVKeyCode)
	{
		case kVKC_CapsLock:		special = kKey_Caps;					break;
		case kVKC_Shift:		special = kKey_Shift;					break;
		case kVKC_Control:		special = kKey_Control;					break;
		case kVKC_Option:		special = kKey_Option;					break;
		case kVKC_rShift:		special = kKey_RightShift;				break;
		case kVKC_rControl:		special = kKey_RightControl;			break;
		case kVKC_rOption:		special = kKey_RightOption;				break;
		case kVKC_Command:		special = kKey_Command;					break;
		case kVKC_Return:		special = kKey_Return;					break;
		case kVKC_Backspace:	special = kKey_Backspace;				break;
		case kVKC_Delete:		special = kKey_Delete;					break;
		case kVKC_Help:			special = kKey_Help;					break;
		case kVKC_Home:			special = kKey_Home;					break;
		case kVKC_PgUp:			special = kKey_PgUp;					break;
		case kVKC_PgDn:			special = kKey_PgDn;					break;
		case kVKC_End:			special = kKey_End;						break;
		case kVKC_LArrow:		special = kKey_Left;					break;
		case kVKC_RArrow:		special = kKey_Right;					break;
		case kVKC_UArrow:		special = kKey_Up;						break;
		case kVKC_DArrow:		special = kKey_Down;					break;
		case kVKC_Fn:			special = kKey_Fn;						break;
		case kVKC_KpdEnter:		special = kKey_Enter;					break;
		case kVKC_KbdEnter:		special = kKey_Enter;					break;
		case kVKC_Space:		special = kKey_Space;					break;
			
		case kVKC_Mute:			special = kKey_Mute;					break;
		case kVKC_VolumeDown:	special = kKey_VolumeDown;				break;
		case kVKC_VolumeUp:		special = kKey_VolumeUp;				break;
		case kVKC_F17:			special = kKey_F17;						break;
		case kVKC_F18:			special = kKey_F18;						break;
		case kVKC_F19:			special = kKey_F19;						break;
		case kVKC_F20:			special = kKey_F20;						break;
		
		#if KEYNAMING_ENABLE_HID
		
		case kFKC_rCommand:		special = kKey_RightCommand;			break;
		case kFKC_Power:		special = kKey_Power;					break;
		case kFKC_Eject:		special = kKey_Eject;					break;
		case kFKC_F21:			special = kKey_F21;						break;
		case kFKC_F22:			special = kKey_F22;						break;
		case kFKC_F23:			special = kKey_F23;						break;
		case kFKC_F24:			special = kKey_F24;						break;
		
		#endif
		
		case kVKC_Kpd_0: case kVKC_Kpd_1: case kVKC_Kpd_2: case kVKC_Kpd_3:
		case kVKC_Kpd_4: case kVKC_Kpd_5: case kVKC_Kpd_6: case kVKC_Kpd_7:
		case kVKC_Kpd_8: case kVKC_Kpd_9: case kVKC_Kpd_A: case kVKC_Kpd_B:
		case kVKC_Kpd_C: case kVKC_Kpd_D: case kVKC_Kpd_E: case kVKC_Kpd_F:
			handlingMode = kKeypadHandling;
		break;
	}
	
	switch (handlingMode)
	{
		case kEscapeClearHandling:
			if (kVKC_Esc == inVKeyCode) special = kKey_Esc;
			else special = kKey_Clear;
			break;
		
		case kFKeyHandling:
			switch (inVKeyCode)
			{
				case kVKC_F1:	special = kKey_F1;		break;
				case kVKC_F2:	special = kKey_F2;		break;
				case kVKC_F3:	special = kKey_F3;		break;
				case kVKC_F4:	special = kKey_F4;		break;
				case kVKC_F5:	special = kKey_F5;		break;
				case kVKC_F6:	special = kKey_F6;		break;
				case kVKC_F7:	special = kKey_F7;		break;
				case kVKC_F8:	special = kKey_F8;		break;
				case kVKC_F9:	special = kKey_F9;		break;
				case kVKC_F10:	special = kKey_F10;		break;
				case kVKC_F11:	special = kKey_F11;		break;
				case kVKC_F12:	special = kKey_F12;		break;
				case kVKC_F13:	special = kKey_F13;		break;
				case kVKC_F14:	special = kKey_F14;		break;
				case kVKC_F15:	special = kKey_F15;		break;
				case kVKC_F16:	special = kKey_F16;		break;
				case kVKC_F17:	special = kKey_F17;		break;
				case kVKC_F18:	special = kKey_F18;		break;
				case kVKC_F19:	special = kKey_F19;		break;
				case kVKC_F20:	special = kKey_F20;		break;
					
				#if KEYNAMING_ENABLE_HID
				
				case kFKC_F21:	special = kKey_F21;		break;
				case kFKC_F22:	special = kKey_F22;		break;
				case kFKC_F23:	special = kKey_F23;		break;
				case kFKC_F24:	special = kKey_F24;		break;
				
				#endif
				
				case kVKC_Menu:	special = kKey_Menu;	break;
				default:
					result = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, UnknownCodeString(), 0x1000 | inVKeyCode);
			}
			break;
	}
	
	if (NULL != special)
	{
		result = CopyLocalizedString(special);
	}
	
	if (NULL == result || CFEqual(CFSTR(""), result))
	{
		/*	Generic handling
			  If there's a 'uchr' resource for the key script, we use UCKeyTranslate to create a
			Unicode string for the key. Otherwise we convert the old-style KeyTranslate char code to
			a Pascal string and then to a CFString.
			
			  Then, we capitalize the string. (We want to have an 'A' key rather than an 'a' key,
			for instance.) To allow this, we use a temporary mutable CFString.
			
			  Note that UCKeyTranslate is allowed to return anything from zero to 255 characters.
		*/
		tempStr = CFStringCreateMutable(kCFAllocatorDefault, 0);
		
		if (NULL != tempStr)
		{
			if (inState->uchr)
			{
				/* Use the Unicode key-mapping table to name the key */
				err = UCKeyTranslate(inState->uchr, inVKeyCode, kUCKeyActionDisplay, 0, inState->keyboardType, kUCKeyTranslateNoDeadKeysMask, &state, kLength, &length, string);
				
				if (noErr == err) CFStringAppendCharacters(tempStr, string, length);
			}
			
			if (NULL == inState->uchr || noErr != err)
			{
				/* Use the old-fashioned charCode + script to name the key */
				pascalString[0] = 1;
				pascalString[1] = charCode;
				
				CFStringAppendPascalString(tempStr, pascalString, inState->encoding);
			}
			
			CFStringUppercase(tempStr, NULL);
			result = tempStr;
		}
		else
		{
			/* CFStringCreateMutable() failed */
			err = memFullErr;
		}
	}
	
	if (handlingMode == kKeypadHandling)
	{
		/*
			Handle generic keypad keys, such as 0, 1, + etc, for which we want a
			string of the form "Keypad X", where X is the relevant charcode.
			We've already got the key's name through generic handling above.
			Now we prepend "Keypad" using a string of the form "Keypad %@".
		*/
		
		CFStringRef keypad_X;
		keypad_X = CopyLocalizedString(kKey_Keypad_X);
		
		if (keypad_X)
		{
			CFStringRef temp = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, keypad_X, result);
			if (temp)
			{
				CFRelease(result);
				result = temp;
			}
		}
	}
	
	if (result)
	{
		if (err)
		{
			CFRelease(result);
		}
		else
		{
			*outString = result;
		}
	}
	else err = memFullErr;
	
	if (outHandlingMode) *outHandlingMode = handlingMode;
	
	return err;
}


void KeyNamingSetStringTable(CFStringRef inTableName)
{
	KeyNamingSetStringTableAndBundle(inTableName, NULL);
}


void KeyNamingSetStringTableAndBundle(CFStringRef inTableName, CFBundleRef inBundle)
{
	if (NULL != inTableName) CFRetain(inTableName);	/* NULL is OK; we'll re-init to "KeyNaming" in InitCache */
	if (NULL != sStringTable) CFRelease(sStringTable);
	if (NULL != sStringBundle) CFRelease(sStringBundle);
	if (NULL != sUnknownCodeString)
	{
		CFRelease(sUnknownCodeString);
		sUnknownCodeString = NULL;
	}
	
	sStringTable = inTableName;
	sStringBundle = inBundle;
}


static CFStringRef UnknownCodeString(void)
{
	if (NULL == sUnknownCodeString) sUnknownCodeString = CopyLocalizedString(kKey_Unknown_N);
	return sUnknownCodeString;
}


#pragma mark Test rig

#ifdef KEYNAMING_MAKE_TEST_RIG

/*	Simple command line test for KeyNaming. Also acts as a utility to find key codes for previously-
	unknown/undocumented keys.
	
	Use control-C to quit.
*/

#define KEYMAP_GET(m, index) ((((uint8_t*)(m))[(index) >> 3] & (1L << ((index) & 7))) ? 1 : 0)

int main(void)
{
	KeyMap				map, oldMap = {0};
	struct timespec		sleepTime = { 0, 100000000 };
	CFStringRef			keyName, string;
	OSStatus			err = noErr;
	StateCache			state;
	uint8_t				i, mode;
	CFStringRef			handlingModes[kHandlingModeCount] =
						{
							CFSTR("normal"),
							CFSTR("escape/clear"),
							CFSTR("F# key"),
							CFSTR("keypad"),
							CFSTR("unknown")
						};
	
	for (;;)
	{
		GetKeys(map);
		for (i = 0; i < 128; ++i)
		{
			InitStateCache(&state);
			if (KEYMAP_GET(map, i) && !KEYMAP_GET(oldMap, i))
			{
				err = NameOneKey(i, &keyName, &state, &mode);
				if (noErr == err)
				{
					if (kHandlingModeCount <= mode) mode = kUnknownHandling - 1;
					string = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("  0x%.2X  (%i) = \"%@\", method = %@"), i, i, keyName, handlingModes[mode]);
					CFRelease(keyName);
				}
				else
				{
					string = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("  0x%.2X  (%i): error %i getting name"), i, i, err);
				}
				CFShow(string);
				CFRelease(string);
			}
		}
		
		bcopy(map, oldMap, sizeof map);
		nanosleep(&sleepTime, NULL);
	}
	
	return 0;
}


#endif	/* KEYNAMING_MAKE_TEST_RIG */
