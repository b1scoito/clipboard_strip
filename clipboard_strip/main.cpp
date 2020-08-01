#include "stdafx.hpp"

#include "clipboard.hpp"
#include "input.hpp"

int __stdcall WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
#ifndef _DEBUG
	if ( !startup::is_file_registered() )
	{
		_log( LWARN, "[ WinMain ] program is not registed..." );
		
		if ( !startup::register_file() )
			_log( LERROR, "[ WinMain ] failed to register program startup..." );
	}
#endif
	
	std::thread( []()
	{
		while ( true )
		{
			g_input->reset_pressed_state();

			g_clipboard->watch_clipboard();
			g_clipboard->watch_paste();

			if ( g_input->is_key_down( VK_LCONTROL ) && g_input->is_key_down( VK_LSHIFT ) && g_input->is_key_down( VK_SCROLL ) )
				exit( EXIT_SUCCESS );

			std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
		}
	} ).detach();

	MSG msg;
	while ( !GetMessageA( &msg, nullptr, NULL, NULL ) )
	{
		TranslateMessage( &msg );
		DispatchMessageA( &msg );
	}

	return EXIT_SUCCESS;
}