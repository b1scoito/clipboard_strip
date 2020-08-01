#pragma once

namespace startup
{
	inline bool is_file_registered()
	{
		const auto file_path = std::filesystem::current_path();
		const auto str_file = file_path.filename().generic_string() + ".exe";

		char ch_path_to_file[MAX_PATH] = {};
		auto dw_size = static_cast<DWORD>( sizeof ch_path_to_file );

		HKEY h_key = nullptr;
		auto l_status = RegOpenKeyExA( HKEY_CURRENT_USER, R"(Software\Microsoft\Windows\CurrentVersion\Run)", 0, KEY_READ, &h_key );

		auto b_success = l_status == 0;
		if ( b_success )
		{
			auto dw_reg_type = REG_SZ;

			l_status = RegGetValueA( h_key, nullptr, str_file.c_str(), RRF_RT_REG_SZ, &dw_reg_type, ch_path_to_file, &dw_size );
			b_success = l_status == 0;
		}

		if ( b_success )
			b_success = strlen( ch_path_to_file ) > 0 ? TRUE : FALSE;

		if ( h_key != nullptr )
		{
			RegCloseKey( h_key );
			h_key = nullptr;
		}
		return b_success;
	}

	inline bool register_file( const std::string& str_args = "" )
	{
		const auto file_path = std::filesystem::current_path();
		const auto str_file = file_path.filename().generic_string() + ".exe";
		auto str_path = file_path.generic_string() + ".exe";

		if ( str_args.empty() )
			str_path.append( str_args );

		HKEY h_key = nullptr;
		auto l_result = RegCreateKeyExA( HKEY_CURRENT_USER, R"(Software\Microsoft\Windows\CurrentVersion\Run)", 0, nullptr, 0, KEY_WRITE | KEY_READ, nullptr, &h_key, nullptr );

		auto b_success = l_result == 0;
		if ( b_success )
		{
			l_result = RegSetValueExA( h_key, str_file.c_str(), 0, REG_SZ, reinterpret_cast<BYTE*>( str_path.data() ), ( str_path.length() + 1 ) * 2 );
			b_success = l_result == 0;
		}

		if ( h_key != nullptr )
		{
			RegCloseKey( h_key );
			h_key = nullptr;
		}
		return b_success;
	}

}
