#include "stdafx.hpp"
#include "input.hpp"

// the following code is not defined in the class or the header for countless reasons...

HHOOK o_keybd = nullptr;
LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam )
{
	if ( nCode == HC_ACTION )
		g_input->process_keybd_message( wParam, reinterpret_cast<PKBDLLHOOKSTRUCT>( lParam ) );

	return CallNextHookEx( o_keybd, nCode, wParam, lParam );
}

input::input()
{
	o_keybd = SetWindowsHookExA( WH_KEYBOARD_LL, LowLevelKeyboardProc, nullptr, 0 );
	if ( !o_keybd )
		throw std::runtime_error( "[ input ] o_keybd is null." );
}

input::~input()
{
	UnhookWindowsHookEx( o_keybd );
}

bool input::process_keybd_message( WPARAM wParam, PKBDLLHOOKSTRUCT kbData )
{
	key_state_t state;
	switch ( wParam )
	{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			state = key_state_t::KDOWN;
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			state = key_state_t::KUP;
			break;
		default:
			return false;
	}

	const auto key = kbData->vkCode;
	if ( state == key_state_t::KUP && arr_key_map[key] == key_state_t::KDOWN )
		arr_key_map[key] = key_state_t::KPRESSED;
	else
		arr_key_map[key] = state;

	return true;
}

key_state_t input::get_key_state( const std::uint32_t vk )
{
	return arr_key_map[vk];
}

bool input::is_key_down( const std::uint32_t vk )
{
	return arr_key_map[vk] == key_state_t::KDOWN;
}

bool input::was_key_pressed( const std::uint32_t vk )
{
	if ( arr_key_map[vk] == key_state_t::KPRESSED )
	{
		arr_key_map[vk] = key_state_t::KUP;
		return true;
	}

	return false;
}

void input::reset_pressed_state()
{
	const auto duration = std::chrono::system_clock::now().time_since_epoch();
	const auto epoch = std::chrono::duration_cast<std::chrono::milliseconds>( duration ).count();
	static auto last_epoch = epoch;

	if ( epoch >= last_epoch )
	{
		for ( auto& i : arr_key_map )
		{
			if ( i == key_state_t::KPRESSED )
				i = key_state_t::KUP;
		}
		last_epoch = epoch + 250;
	}
}

std::string input::get_key_name_by_id( const std::uint32_t vk ) const
{
	static std::unordered_map<int, std::string> key_names =
	{
		{ 0,				"none" },
		{ VK_LBUTTON,		"mouse1" },
		{ VK_RBUTTON,		"mouse2" },
		{ VK_MBUTTON,		"mouse3" },
		{ VK_XBUTTON1,		"mouse4" },
		{ VK_XBUTTON2,		"mouse5" },
		{ VK_BACK,			"backspace" },
		{ VK_TAB,			"tab" },
		{ VK_CLEAR,			"clear" },
		{ VK_RETURN,		"enter" },
		{ VK_SHIFT,			"shift" },
		{ VK_CONTROL,		"ctrl" },
		{ VK_MENU,			"alt" },
		{ VK_PAUSE,			"pause" },
		{ VK_CAPITAL,		"caps lock" },
		{ VK_ESCAPE,		"escape" },
		{ VK_SPACE,			"space" },
		{ VK_PRIOR,			"page up" },
		{ VK_NEXT,			"page down" },
		{ VK_END,			"end" },
		{ VK_HOME,			"home" },
		{ VK_LEFT,			"left" },
		{ VK_UP,			"up" },
		{ VK_RIGHT,			"right" },
		{ VK_DOWN,			"down" },
		{ VK_SELECT,		"select" },
		{ VK_PRINT,			"print screen" },
		{ VK_INSERT,		"insert" },
		{ VK_DELETE,		"delete" },
		{ VK_HELP,			"help" },
		{ VK_SLEEP,			"sleep" },
		{ VK_MULTIPLY,		"numpad *" },
		{ VK_ADD,			"numpad +" },
		{ VK_SUBTRACT,		"Numpad -" },
		{ VK_DECIMAL,		"numpad ." },
		{ VK_DIVIDE,		"numpad /" },
		{ VK_NUMLOCK,		"num lock" },
		{ VK_SCROLL,		"scroll lock" },
		{ VK_LSHIFT,		"left shift" },
		{ VK_RSHIFT,		"right shift" },
		{ VK_LCONTROL,		"left ctrl" },
		{ VK_RCONTROL,		"right ctrl" },
		{ VK_LMENU,			"left alt" },
		{ VK_RMENU,			"right alt" },
		{ VK_OEM_4,			"[" },
		{ VK_OEM_6,			"]" },
		{ VK_OEM_5,			"\\" },
		{ VK_OEM_MINUS,		"-" },
		{ VK_OEM_PLUS,		"=" },
		{ VK_OEM_1,			";" },
		{ VK_OEM_7,			"\'" },
		{ VK_OEM_COMMA,		"," },
		{ VK_OEM_PERIOD,	"." },
		{ VK_OEM_2,			"/" },
		{ VK_OEM_3,			"`" }
	};

	// numbers
	if ( vk >= '0' && vk <= '9' )
		return std::string( 1, static_cast<char>( vk ) );

	// letters
	if ( vk >= 'A' && vk <= 'Z' )
		return std::string( 1, static_cast<char>( vk + 0x20 ) );

	// numpad 0 - 9
	if ( vk >= 0x60 && vk <= 0x69 )
		return "numpad " + std::to_string( vk - 0x60 );

	// function keys f1 - f12
	if ( vk >= 0x70 && vk <= 0x87 )
		return std::string( 'f' + std::to_string( vk - 0x70 + 1 ) );

	// hardcoded
	return key_names[vk];
}
