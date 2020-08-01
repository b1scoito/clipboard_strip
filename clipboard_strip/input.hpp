#pragma once
#include <unordered_map>

enum key_state_t
{
	KNONE = 1,
	KDOWN,
	KUP,
	KPRESSED
};

class input
{
private:
	key_state_t arr_key_map[255]{};

public:
	input();
	~input();

	key_state_t get_key_state( std::uint32_t vk );
	std::string get_key_name_by_id( std::uint32_t vk ) const;

	bool is_key_down( std::uint32_t vk );
	bool was_key_pressed( std::uint32_t vk );
	void reset_pressed_state();

	bool process_keybd_message( WPARAM wParam, PKBDLLHOOKSTRUCT kbData );
};
inline auto g_input = std::make_unique<input>();