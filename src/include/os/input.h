#include"PCH.h"
#include<Windows.h>
struct Key
{
	enum Enum
	{
		None = 0,
		Esc,
		Return,
		Tab,
		Space,
		Backspace,
		Up,
		Down,
		Left,
		Right,
		Insert,
		Delete,
		Home,
		End,
		PageUp,
		PageDown,
		Print,
		Plus,
		Minus,
		LeftBracket,
		RightBracket,
		Semicolon,
		Quote,
		Comma,
		Period,
		Slash,
		Backslash,
		Tilde,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		NumPad0,
		NumPad1,
		NumPad2,
		NumPad3,
		NumPad4,
		NumPad5,
		NumPad6,
		NumPad7,
		NumPad8,
		NumPad9,
		Key0,
		Key1,
		Key2,
		Key3,
		Key4,
		Key5,
		Key6,
		Key7,
		Key8,
		Key9,
		KeyA,
		KeyB,
		KeyC,
		KeyD,
		KeyE,
		KeyF,
		KeyG,
		KeyH,
		KeyI,
		KeyJ,
		KeyK,
		KeyL,
		KeyM,
		KeyN,
		KeyO,
		KeyP,
		KeyQ,
		KeyR,
		KeyS,
		KeyT,
		KeyU,
		KeyV,
		KeyW,
		KeyX,
		KeyY,
		KeyZ,

		GamepadA,
		GamepadB,
		GamepadX,
		GamepadY,
		GamepadThumbL,
		GamepadThumbR,
		GamepadShoulderL,
		GamepadShoulderR,
		GamepadUp,
		GamepadDown,
		GamepadLeft,
		GamepadRight,
		GamepadBack,
		GamepadStart,
		GamepadGuide,

		Count
	};
};
//struct Event{
//	Key k;
//};
//std::queue<Event> eQueue;
static uint8_t s_translateKey[256];

static Key::Enum translateKey(WPARAM _wparam)
{
	return (Key::Enum)s_translateKey[_wparam & 0xff];
}