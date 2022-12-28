
#include "Windows.h"
#include"kwin.h"
#ifndef NOMINMAX
#    define NOMINMAX
#endif
namespace pj2 {      
	int kmap(const IOin::Key& k) { 
		switch (k)
		{
		/*case IOin::Key::Esc:		   return VK_ESCAPE;
		case IOin::Key::Return:		   return VK_RETURN;
		case IOin::Key::Tab:		   return VK_TAB;
		case IOin::Key::Backspace:	   return VK_BACK;
		case IOin::Key::Space:	   return VK_SPACE;
		case IOin::Key::Up:			   return VK_UP;
		case IOin::Key::Down:		   return VK_DOWN;
		case IOin::Key::Left:		   return VK_LEFT;
		case IOin::Key::Right:		   return VK_RIGHT;
		case IOin::Key::Insert:		   return VK_INSERT;
		case IOin::Key::Delete:		   return VK_DELETE;
		case IOin::Key::Home:		   return VK_HOME;
		case IOin::Key::End:		   return VK_END;
		case IOin::Key::PageUp:		   return VK_PRIOR;
		case IOin::Key::PageDown:	   return VK_NEXT;
		case IOin::Key::Print:		   return VK_SNAPSHOT;
		case IOin::Key::Plus:		   return VK_OEM_PLUS;
		case IOin::Key::Minus:		   return VK_OEM_MINUS;
		case IOin::Key::LeftBracket:	   return VK_OEM_4;
		case IOin::Key::RightBracket:     return VK_OEM_6;
		case IOin::Key::Semicolon:     return VK_OEM_1;
		case IOin::Key::Quote:	       return VK_OEM_7;
		case IOin::Key::Comma:		   return VK_OEM_COMMA;
		case IOin::Key::Period:		   return VK_OEM_PERIOD;
		case IOin::Key::Slash:		   return VK_OEM_2;
		case IOin::Key::Backslash:	   return VK_OEM_5;
		case 	IOin::Key::Tilde:	   return VK_OEM_3;
		case IOin::Key::F1:			   return VK_F1;
		case IOin::Key::F2:			   return VK_F2;
		case IOin::Key::F3:			   return VK_F3;
		case IOin::Key::F4:			   return VK_F4;
		case IOin::Key::F5:			   return VK_F5;
		case IOin::Key::F6:			   return VK_F6;
		case IOin::Key::F7:			   return VK_F7;
		case IOin::Key::F8:			   return VK_F8;
		case IOin::Key::F9:			   return VK_F9;
		case IOin::Key::F10:		   return VK_F10;
		case IOin::Key::F11:		   return VK_F11;
		case IOin::Key::F12:		   return VK_F12;
		case IOin::Key::NumPad0:	   return VK_NUMPAD0;
		case IOin::Key::NumPad1:	   return VK_NUMPAD1;
		case IOin::Key::NumPad2:	   return VK_NUMPAD2;
		case IOin::Key::NumPad3:	   return VK_NUMPAD3;
		case IOin::Key::NumPad4:	   return VK_NUMPAD4;
		case IOin::Key::NumPad5:	   return VK_NUMPAD5;
		case IOin::Key::NumPad6:	   return VK_NUMPAD6;
		case IOin::Key::NumPad7:	   return VK_NUMPAD7;
		case IOin::Key::NumPad8:	   return VK_NUMPAD8;
		case IOin::Key::NumPad9:	   return VK_NUMPAD9;
		case IOin::Key::Key0:		   return int('0');
		case IOin::Key::Key1:		   return int('1');
		case IOin::Key::Key2:		   return int('2');
		case IOin::Key::Key3:		   return int('3');
		case IOin::Key::Key4:		   return int('4');
		case IOin::Key::Key5:		   return int('5');
		case IOin::Key::Key6:		   return int('6');
		case IOin::Key::Key7:		   return int('7');
		case IOin::Key::Key8:		   return int('8');
		case IOin::Key::Key9:		   return int('9');
		case IOin::Key::KeyA:		   return int('A');
		case IOin::Key::KeyB:		   return int('B');
		case IOin::Key::KeyC:		   return int('C');
		case IOin::Key::KeyD:		   return int('D');
		case IOin::Key::KeyE:		   return int('E');
		case IOin::Key::KeyF:		   return int('F');
		case IOin::Key::KeyG:		   return int('G');
		case IOin::Key::KeyH:		   return int('H');
		case IOin::Key::KeyI:		   return int('I');
		case IOin::Key::KeyJ:		   return int('J');
		case IOin::Key::KeyK:		   return int('K');
		case IOin::Key::KeyL:		   return int('L');
		case IOin::Key::KeyM:		   return int('M');
		case IOin::Key::KeyN:		   return int('N');
		case IOin::Key::KeyO:		   return int('O');
		case IOin::Key::KeyP:		   return int('P');
		case IOin::Key::KeyQ:		   return int('Q');
		case IOin::Key::KeyR:		   return int('R');
		case IOin::Key::KeyS:		   return int('S');
		case IOin::Key::KeyT:		   return int('T');
		case IOin::Key::KeyU:		   return int('U');
		case IOin::Key::KeyV:		   return int('V');
		case IOin::Key::KeyW:		   return int('W');
		case IOin::Key::KeyX:		   return int('X');
		case IOin::Key::KeyY:		   return int('Y');
		case IOin::Key::KeyZ:		   return int('Z');*/
		default:
			return -1;
			break;
		}
	}
}