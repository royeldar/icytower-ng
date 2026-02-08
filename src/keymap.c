#include <allegro5/allegro.h>

#include "keymap.h"

const char *get_key_name(int keycode) {
    assert(keycode >= 0 && keycode < ALLEGRO_KEY_MAX);
    switch (keycode) {
    case ALLEGRO_KEY_A:
        return "A";
    case ALLEGRO_KEY_B:
        return "B";
    case ALLEGRO_KEY_C:
        return "C";
    case ALLEGRO_KEY_D:
        return "D";
    case ALLEGRO_KEY_E:
        return "E";
    case ALLEGRO_KEY_F:
        return "F";
    case ALLEGRO_KEY_G:
        return "G";
    case ALLEGRO_KEY_H:
        return "H";
    case ALLEGRO_KEY_I:
        return "I";
    case ALLEGRO_KEY_J:
        return "J";
    case ALLEGRO_KEY_K:
        return "K";
    case ALLEGRO_KEY_L:
        return "L";
    case ALLEGRO_KEY_M:
        return "M";
    case ALLEGRO_KEY_N:
        return "N";
    case ALLEGRO_KEY_O:
        return "O";
    case ALLEGRO_KEY_P:
        return "P";
    case ALLEGRO_KEY_Q:
        return "Q";
    case ALLEGRO_KEY_R:
        return "R";
    case ALLEGRO_KEY_S:
        return "S";
    case ALLEGRO_KEY_T:
        return "T";
    case ALLEGRO_KEY_U:
        return "U";
    case ALLEGRO_KEY_V:
        return "V";
    case ALLEGRO_KEY_W:
        return "W";
    case ALLEGRO_KEY_X:
        return "X";
    case ALLEGRO_KEY_Y:
        return "Y";
    case ALLEGRO_KEY_Z:
        return "Z";
    case ALLEGRO_KEY_0:
        return "0";
    case ALLEGRO_KEY_1:
        return "1";
    case ALLEGRO_KEY_2:
        return "2";
    case ALLEGRO_KEY_3:
        return "3";
    case ALLEGRO_KEY_4:
        return "4";
    case ALLEGRO_KEY_5:
        return "5";
    case ALLEGRO_KEY_6:
        return "6";
    case ALLEGRO_KEY_7:
        return "7";
    case ALLEGRO_KEY_8:
        return "8";
    case ALLEGRO_KEY_9:
        return "9";
    case ALLEGRO_KEY_PAD_0:
        return "0 (Pad)";
    case ALLEGRO_KEY_PAD_1:
        return "1 (Pad)";
    case ALLEGRO_KEY_PAD_2:
        return "2 (Pad)";
    case ALLEGRO_KEY_PAD_3:
        return "3 (Pad)";
    case ALLEGRO_KEY_PAD_4:
        return "4 (Pad)";
    case ALLEGRO_KEY_PAD_5:
        return "5 (Pad)";
    case ALLEGRO_KEY_PAD_6:
        return "6 (Pad)";
    case ALLEGRO_KEY_PAD_7:
        return "7 (Pad)";
    case ALLEGRO_KEY_PAD_8:
        return "8 (Pad)";
    case ALLEGRO_KEY_PAD_9:
        return "9 (Pad)";
    case ALLEGRO_KEY_F1:
        return "F1";
    case ALLEGRO_KEY_F2:
        return "F2";
    case ALLEGRO_KEY_F3:
        return "F3";
    case ALLEGRO_KEY_F4:
        return "F4";
    case ALLEGRO_KEY_F5:
        return "F5";
    case ALLEGRO_KEY_F6:
        return "F6";
    case ALLEGRO_KEY_F7:
        return "F7";
    case ALLEGRO_KEY_F8:
        return "F8";
    case ALLEGRO_KEY_F9:
        return "F9";
    case ALLEGRO_KEY_F10:
        return "F10";
    case ALLEGRO_KEY_F11:
        return "F11";
    case ALLEGRO_KEY_F12:
        return "F12";
    case ALLEGRO_KEY_ESCAPE:
        return "Esc";
    case ALLEGRO_KEY_TILDE:
        return "Tilde";
    case ALLEGRO_KEY_MINUS:
        return "-";
    case ALLEGRO_KEY_EQUALS:
        return "=";
    case ALLEGRO_KEY_BACKSPACE:
        return "Backspace";
    case ALLEGRO_KEY_TAB:
        return "Tab";
    case ALLEGRO_KEY_OPENBRACE:
        return "{";
    case ALLEGRO_KEY_CLOSEBRACE:
        return "}";
    case ALLEGRO_KEY_ENTER:
        return "Enter";
    case ALLEGRO_KEY_SEMICOLON:
        return ";";
    case ALLEGRO_KEY_QUOTE:
        return "\'";
    case ALLEGRO_KEY_BACKSLASH:
        return "\\";
    case ALLEGRO_KEY_BACKSLASH2:
        return "\\";
    case ALLEGRO_KEY_COMMA:
        return ",";
    case ALLEGRO_KEY_FULLSTOP:
        return ".";
    case ALLEGRO_KEY_SLASH:
        return "/";
    case ALLEGRO_KEY_SPACE:
        return "Space";
    case ALLEGRO_KEY_INSERT:
        return "Insert";
    case ALLEGRO_KEY_DELETE:
        return "Delete";
    case ALLEGRO_KEY_HOME:
        return "Home";
    case ALLEGRO_KEY_END:
        return "End";
    case ALLEGRO_KEY_PGUP:
        return "Page Up";
    case ALLEGRO_KEY_PGDN:
        return "Page Down";
    case ALLEGRO_KEY_LEFT:
        return "Left";
    case ALLEGRO_KEY_RIGHT:
        return "Right";
    case ALLEGRO_KEY_UP:
        return "Up";
    case ALLEGRO_KEY_DOWN:
        return "Down";
    case ALLEGRO_KEY_PAD_SLASH:
        return "/ (Pad)";
    case ALLEGRO_KEY_PAD_ASTERISK:
        return "* (Pad)";
    case ALLEGRO_KEY_PAD_MINUS:
        return "- (Pad)";
    case ALLEGRO_KEY_PAD_PLUS:
        return "+ (Pad)";
    case ALLEGRO_KEY_PAD_DELETE:
        return "Delete (Pad)";
    case ALLEGRO_KEY_PAD_ENTER:
        return "Enter (Pad)";
    case ALLEGRO_KEY_PRINTSCREEN:
        return "Print Screen";
    case ALLEGRO_KEY_PAUSE:
        return "Pause";
    case ALLEGRO_KEY_YEN:
        return "Yen";
    case ALLEGRO_KEY_KANA:
        return "Kana";
    case ALLEGRO_KEY_LSHIFT:
        return "Left Shift";
    case ALLEGRO_KEY_RSHIFT:
        return "Right Shift";
    case ALLEGRO_KEY_LCTRL:
        return "Left Ctrl";
    case ALLEGRO_KEY_RCTRL:
        return "Right Ctrl";
    case ALLEGRO_KEY_ALT:
        return "Alt";
    case ALLEGRO_KEY_ALTGR:
        return "Alt Gr";
    case ALLEGRO_KEY_LWIN:
        return "Left Win";
    case ALLEGRO_KEY_RWIN:
        return "Right Win";
    case ALLEGRO_KEY_MENU:
        return "Menu";
    case ALLEGRO_KEY_SCROLLLOCK:
        return "Scroll Lock";
    case ALLEGRO_KEY_NUMLOCK:
        return "Num Lock";
    case ALLEGRO_KEY_CAPSLOCK:
        return "Caps Lock";
    default:
        return "unknown";
    }
}
