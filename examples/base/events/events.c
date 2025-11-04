/* See UNLICENSE file for copyright and license details. */
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

#define MG_IMPLEMENTATION
#include "minigraphics.h"

static const char *keynames[] = { "UNKNOWN: 0x00", "UNKNOWN: 0x01", "UNKNOWN: 0x02", "UNKNOWN: 0x03", "MG_KEY_A", "MG_KEY_B", "MG_KEY_C", "MG_KEY_D", "MG_KEY_E", "MG_KEY_F", "MG_KEY_G", "MG_KEY_H", "MG_KEY_I", "MG_KEY_J", "MG_KEY_K", "MG_KEY_L", "MG_KEY_M", "MG_KEY_N", "MG_KEY_O", "MG_KEY_P", "MG_KEY_Q", "MG_KEY_R", "MG_KEY_S", "MG_KEY_T", "MG_KEY_U", "MG_KEY_V", "MG_KEY_W", "MG_KEY_X", "MG_KEY_Y", "MG_KEY_Z", "MG_KEY_1", "MG_KEY_2", "MG_KEY_3", "MG_KEY_4", "MG_KEY_5", "MG_KEY_6", "MG_KEY_7", "MG_KEY_8", "MG_KEY_9", "MG_KEY_0", "MG_KEY_RETURN", "MG_KEY_ESCAPE", "MG_KEY_BACKSPACE", "MG_KEY_TAB", "MG_KEY_SPACE", "MG_KEY_MINUS", "MG_KEY_EQUALS", "MG_KEY_LEFTBRACKET", "MG_KEY_RIGHTBRACKET", "MG_KEY_BACKSLASH", "MG_KEY_NONUSHASH", "MG_KEY_SEMICOLON", "MG_KEY_APOSTROPHE", "MG_KEY_GRAVE", "MG_KEY_COMMA", "MG_KEY_PERIOD", "MG_KEY_SLASH", "MG_KEY_CAPSLOCK", "MG_KEY_F1", "MG_KEY_F2", "MG_KEY_F3", "MG_KEY_F4", "MG_KEY_F5", "MG_KEY_F6", "MG_KEY_F7", "MG_KEY_F8", "MG_KEY_F9", "MG_KEY_F10", "MG_KEY_F11", "MG_KEY_F12", "MG_KEY_PRINTSCREEN", "MG_KEY_SCROLLLOCK", "MG_KEY_PAUSE", "MG_KEY_INSERT", "MG_KEY_HOME", "MG_KEY_PAGEUP", "MG_KEY_DELETE", "MG_KEY_END", "MG_KEY_PAGEDOWN", "MG_KEY_RIGHT", "MG_KEY_LEFT", "MG_KEY_DOWN", "MG_KEY_UP", "MG_KEY_NUMLOCKCLEAR", "MG_KEY_KP_DIVIDE", "MG_KEY_KP_MULTIPLY", "MG_KEY_KP_MINUS", "MG_KEY_KP_PLUS", "MG_KEY_KP_ENTER", "MG_KEY_KP_1", "MG_KEY_KP_2", "MG_KEY_KP_3", "MG_KEY_KP_4", "MG_KEY_KP_5", "MG_KEY_KP_6", "MG_KEY_KP_7", "MG_KEY_KP_8", "MG_KEY_KP_9", "MG_KEY_KP_0", "MG_KEY_KP_PERIOD", "MG_KEY_NONUSBACKSLASH", "MG_KEY_APPLICATION", "MG_KEY_POWER", "MG_KEY_KP_EQUALS", "MG_KEY_F13", "MG_KEY_F14", "MG_KEY_F15", "MG_KEY_F16", "MG_KEY_F17", "MG_KEY_F18", "MG_KEY_F19", "MG_KEY_F20", "MG_KEY_F21", "MG_KEY_F22", "MG_KEY_F23", "MG_KEY_F24", "MG_KEY_EXECUTE", "MG_KEY_HELP", "MG_KEY_MENU", "MG_KEY_SELECT", "MG_KEY_STOP", "MG_KEY_AGAIN", "MG_KEY_UNDO", "MG_KEY_CUT", "MG_KEY_COPY", "MG_KEY_PASTE", "MG_KEY_FIND", "MG_KEY_MUTE", "MG_KEY_VOLUMEUP", "MG_KEY_VOLUMEDOWN", "MG_KEY_LOCKINGCAPSLOCK", "MG_KEY_LOCKINGNUMLOCK", "MG_KEY_LOCKINGSCROLLLOCK", "MG_KEY_KP_COMMA", "MG_KEY_KP_EQUALSAS400", "MG_KEY_INTERNATIONAL1", "MG_KEY_INTERNATIONAL2", "MG_KEY_INTERNATIONAL3", "MG_KEY_INTERNATIONAL4", "MG_KEY_INTERNATIONAL5", "MG_KEY_INTERNATIONAL6", "MG_KEY_INTERNATIONAL7", "MG_KEY_INTERNATIONAL8", "MG_KEY_INTERNATIONAL9", "MG_KEY_LANG1", "MG_KEY_LANG2", "MG_KEY_LANG3", "MG_KEY_LANG4", "MG_KEY_LANG5", "MG_KEY_LANG6", "MG_KEY_LANG7", "MG_KEY_LANG8", "MG_KEY_LANG9", "MG_KEY_ALTERASE", "MG_KEY_SYSREQ", "MG_KEY_CANCEL", "MG_KEY_CLEAR", "MG_KEY_PRIOR", "MG_KEY_RETURN2", "MG_KEY_SEPARATOR", "MG_KEY_OUT", "MG_KEY_OPER", "MG_KEY_CLEARAGAIN", "MG_KEY_CRSEL", "MG_KEY_EXSEL", "UNKNOWN: 0xa5", "UNKNOWN: 0xa6", "UNKNOWN: 0xa7", "UNKNOWN: 0xa8", "UNKNOWN: 0xa9", "UNKNOWN: 0xaa", "UNKNOWN: 0xab", "UNKNOWN: 0xac", "UNKNOWN: 0xad", "UNKNOWN: 0xae", "UNKNOWN: 0xaf", "MG_KEY_KP_00", "MG_KEY_KP_000", "MG_KEY_THOUSANDSSEPARATOR", "MG_KEY_DECIMALSEPARATOR", "MG_KEY_CURRENCYUNIT", "MG_KEY_CURRENCYSUBUNIT", "MG_KEY_KP_LEFTPAREN", "MG_KEY_KP_RIGHTPAREN", "MG_KEY_KP_LEFTBRACE", "MG_KEY_KP_RIGHTBRACE", "MG_KEY_KP_TAB", "MG_KEY_KP_BACKSPACE", "MG_KEY_KP_A", "MG_KEY_KP_B", "MG_KEY_KP_C", "MG_KEY_KP_D", "MG_KEY_KP_E", "MG_KEY_KP_F", "MG_KEY_KP_XOR", "MG_KEY_KP_POWER", "MG_KEY_KP_PERCENT", "MG_KEY_KP_LESS", "MG_KEY_KP_GREATER", "MG_KEY_KP_AMPERSAND", "MG_KEY_KP_DBLAMPERSAND", "MG_KEY_KP_VERTICALBAR", "MG_KEY_KP_DBLVERTICALBAR", "MG_KEY_KP_COLON", "MG_KEY_KP_HASH", "MG_KEY_KP_SPACE", "MG_KEY_KP_AT", "MG_KEY_KP_EXCLAM", "MG_KEY_KP_MEMSTORE", "MG_KEY_KP_MEMRECALL", "MG_KEY_KP_MEMCLEAR", "MG_KEY_KP_MEMADD", "MG_KEY_KP_MEMSUBTRACT", "MG_KEY_KP_MEMMULTIPLY", "MG_KEY_KP_MEMDIVIDE", "MG_KEY_KP_PLUSMINUS", "MG_KEY_KP_CLEAR", "MG_KEY_KP_CLEARENTRY", "MG_KEY_KP_BINARY", "MG_KEY_KP_OCTAL", "MG_KEY_KP_DECIMAL", "MG_KEY_KP_HEXADECIMAL", "UNKNOWN: 0xde", "UNKNOWN: 0xdf", "MG_KEY_LCTRL", "MG_KEY_LSHIFT", "MG_KEY_LALT", "MG_KEY_LGUI", "MG_KEY_RCTRL", "MG_KEY_RSHIFT", "MG_KEY_RALT", "MG_KEY_RGUI" };

static const char *buttonnames[] = {
	"no button",
	"left mouse button",
	"middle mouse button",
	"right mouse button",
	"side mouse button",
	"extra mouse button"
};

int
main(void)
{
	struct mg_event event;
	jmp_buf env;

	/* if a library error happens, a longjmp() to here will happen. */
	if (setjmp(env)) {
		fprintf(stderr, "mg error: %s\n", mg_errstring());
		mg_quit();
		return 1;
	}

	/* create a 640x480 window */
	mg_init(640, 480, "events.c", env);

	for (;;) {
		/* clear event struct */
		memset(&event, 0, sizeof(struct mg_event));

		if (mg_waiteventfor(&event, 5000)) {
			if (event.events & MG_QUIT) {
				puts(  "recieved event: MG_QUIT");
				break;
			}
			if (event.events & MG_RESIZE)
				printf("recieved event: MG_RESIZE:      width = %d, height = %d\n", mg_width, mg_height);
			if (event.events & MG_REDRAW)
				puts(  "recieved event: MG_REDRAW");
			if (event.events & MG_KEYDOWN)
				printf("recieved event: MG_KEYDOWN:     key = %s\n", keynames[event.keydown]);
			if (event.events & MG_KEYUP)
				printf("recieved event: MG_KEYUP:       key = %s\n", keynames[event.keyup]);
			if (event.events & MG_MOUSEDOWN)
				printf("recieved event: MG_MOUSEDOWN:   btn = %s\n", buttonnames[event.mousedown]);
			if (event.events & MG_MOUSEUP)
				printf("recieved event: MG_MOUSEUP:     btn = %s\n", buttonnames[event.mouseup]);
			if (event.events & MG_MOUSEMOTION)
				printf("recieved event: MG_MOUSEMOTION: x = %d, y = %d\n", event.mousemotion_x, event.mousemotion_y);
		} else {
			puts("5 seconds passed without an event");
		}
	}

	/* cleanup */
	mg_quit();
	return 0;
}
