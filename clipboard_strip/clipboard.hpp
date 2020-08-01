#pragma once

constexpr auto MAX_CLIPBOARD_LENGTH = 256;
constexpr auto CLIPBOARD_SPLIT_CHAR = '|';

class clipboard
{
private:
	std::deque<std::string> vec_segmented_data{};
	std::string str_old_data{};

private:
	static bool clear_clipboard_data();
	static bool get_clipboard_data( std::string& str_data );
	static bool set_clipboard_data(const std::string& str_data);

public:
	clipboard() = default;
	~clipboard() = default;

	bool watch_clipboard();
	bool watch_paste();
};
inline auto g_clipboard = std::make_unique<clipboard>();