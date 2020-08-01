#include "stdafx.hpp"
#include "clipboard.hpp"

#include "input.hpp"

bool clipboard::clear_clipboard_data()
{
	if ( !OpenClipboard( nullptr ) )
	{
		_log( LERROR, "[ clipboard::clear_clipboard_data ] could not open clipboard." );
		return false;
	}

	if ( !EmptyClipboard() )
	{
		_log( LWARN, "[ clipboard::clear_clipboard_data ] failed to clear clipboard." );
		return false;
	}

	if ( !CloseClipboard() )
		_log( LERROR, "[ clipboard::clear_clipboard_data ] could not close clipboard." );

	return true;
}

bool clipboard::get_clipboard_data( std::string& str_data )
{
	if ( !IsClipboardFormatAvailable( CF_TEXT ) )
	{
		_log( LDEBUG, "[ clipboard::get_clipboard_data ] current clipboard data is not formated as ANSI." );
		return false;
	}

	if ( !OpenClipboard( nullptr ) )
	{
		_log( LERROR, "[ clipboard::get_clipboard_data ] could not open clipboard." );
		return false;
	}

	auto* const h_data = GetClipboardData( CF_TEXT );
	if ( h_data )
	{
		auto* ch_data = static_cast<char*>( GlobalLock( h_data ) );
		if ( ch_data )
		{
			str_data = std::string( ch_data );
			GlobalUnlock( h_data );
		}
		else
			_log( LERROR, "[ clipboard::get_clipboard_data ] data == nullptr." );
	}
	else
		_log( LERROR, "[ clipboard::get_clipboard_data ] handle to clipboard data is invalid." );

	if ( !CloseClipboard() )
		_log( LERROR, "[ clipboard::get_clipboard_data ] could not close clipboard." );

	return !str_data.empty();
}

bool clipboard::set_clipboard_data( const std::string& str_data )
{
	if ( !OpenClipboard( nullptr ) )
	{
		_log( LERROR, "[ clipboard::set_clipboard_data ] could not open clipboard." );
		return false;
	}

	if ( !EmptyClipboard() )
	{
		_log( LWARN, "[ clipboard::set_clipboard_data ] failed to clear clipboard." );
		return false;
	}

	auto* const h_data = GlobalAlloc( GMEM_MOVEABLE, str_data.size() + 1 );
	if ( !h_data || h_data == INVALID_HANDLE_VALUE )
	{
		_log( LERROR, "[ clipboard::set_clipboard_data ] could not allocate data." );
		return false;
	}

	memcpy( GlobalLock( h_data ), str_data.c_str(), str_data.size() + 1 );

	GlobalUnlock( h_data );
	SetClipboardData( CF_TEXT, h_data );

	if ( !CloseClipboard() )
		_log( LERROR, "[ clipboard::get_clipboard_data ] could not close clipboard." );

	GlobalFree( h_data );
	return true;
}

bool clipboard::watch_clipboard()
{
	auto split_string = []( const std::string& str_text, const char ch_delim ) -> std::deque<std::string>
	{
		std::deque<std::string> vec_tokens{};

		std::stringstream ss;
		ss.str( str_text );

		std::string str_item;
		while ( std::getline( ss, str_item, ch_delim ) )
			if ( !str_item.empty() )
				vec_tokens.push_back( str_item );

		return vec_tokens;
	};

	if ( !g_input->is_key_down( VK_LCONTROL ) || !g_input->was_key_pressed( 0x43 ) )
		return false;

	std::string str_data;
	if ( !get_clipboard_data( str_data ) )
	{
		_log( LERROR, "[ clipboard::watch_clipboard ] failed to get clipboard data." );
		return false;
	}

	if ( str_data.length() > MAX_CLIPBOARD_LENGTH )
		return false;

	if ( str_data == str_old_data )
		return false;

	str_old_data.clear();
	str_old_data = str_data;

	vec_segmented_data.clear();
	vec_segmented_data = split_string( str_data, CLIPBOARD_SPLIT_CHAR );

	if ( vec_segmented_data.empty() )
		return false;

	if ( !set_clipboard_data( vec_segmented_data.at( 0 ) ) )
	{
		_log( LERROR, "[ clipboard::watch_clipboard ] failed to set clipboard data." );
		return false;
	}
	vec_segmented_data.pop_front();

	_log( LSUCCESS, "[ clipboard::watch_clipboard ] data ready." );
	return true;
}

bool clipboard::watch_paste()
{
	static auto b_last_paste = false;
	if ( !g_input->is_key_down( VK_LCONTROL ) || !g_input->was_key_pressed( 0x56 ) )
		return false;

	if ( b_last_paste )
	{
		str_old_data.clear();
		vec_segmented_data.clear();

		if ( !clear_clipboard_data() )
		{
			_log( LERROR, "[ clipboard::watch_paste ] failed to clear clipboard data." );
			return false;
		}

		b_last_paste = false;
	}

	if ( vec_segmented_data.empty() )
		return false;

	if ( !set_clipboard_data( vec_segmented_data.at( 0 ) ) )
	{
		_log( LERROR, "[ clipboard::watch_paste ] failed to set clipboard data." );
		return false;
	}
	vec_segmented_data.pop_front();

	if ( vec_segmented_data.empty() )
		b_last_paste = true;

	_log( LSUCCESS, "[ clipboard::watch_paste ] next data ready." );
	return true;
}