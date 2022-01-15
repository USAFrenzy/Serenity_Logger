#include "ColorConsoleTarget.h"

#include <iostream>

namespace serenity::expiremental::targets
{
	ColorConsole::ColorConsole( ) : TargetBase( "Console Logger" ), consoleMode( console_interface::std_out ), coloredOutput( false )
	{
		WriteToBaseBuffer( false );
		InitTerminalOutputs( );
		SetConsoleInterface( consoleMode );
		SetOriginalColors( );
		if( IsValidHandle( ) && IsTerminalType( ) ) {
			coloredOutput = true;
		}
	}

	ColorConsole::ColorConsole( std::string_view name )
	  : TargetBase( name ), consoleMode( console_interface::std_out ), coloredOutput( false )
	{
		WriteToBaseBuffer( false );
		InitTerminalOutputs( );
		SetConsoleInterface( consoleMode );
		SetOriginalColors( );
		if( IsValidHandle( ) && IsTerminalType( ) ) {
			coloredOutput = true;
		}
	}

	ColorConsole::ColorConsole( std::string_view name, std::string_view msgPattern )
	  : TargetBase( name, msgPattern ), consoleMode( console_interface::std_out ), coloredOutput( false )
	{
		WriteToBaseBuffer( false );
		InitTerminalOutputs( );
		SetConsoleInterface( consoleMode );
		SetOriginalColors( );
		if( IsValidHandle( ) && IsTerminalType( ) ) {
			coloredOutput = true;
		}
	}

	ColorConsole::~ColorConsole( )
	{
#ifdef WINDOWS_PLATFORM
		DWORD opMode { 0 };
		if( !GetConsoleMode( outputHandle, &opMode ) ) {
			exit( GetLastError( ) );
		}
		if( !SetConsoleMode( outputHandle, opMode ) ) {
			exit( GetLastError( ) );
		}
#endif  // WINDOWS_PLATFORM
	}

	bool ColorConsole::IsValidHandle( )
	{
#ifdef WINDOWS_PLATFORM
		return outputHandle != INVALID_HANDLE_VALUE;
#else
		return outputHandle != nullptr;
#endif  // WINDOWS_PLATFORM
	}

	void ColorConsole::SetMsgColor( LoggerLevel level, std::string_view color )
	{
		msgLevelColors.at( level ) = color;
	}

	std::string_view ColorConsole::GetMsgColor( LoggerLevel level )
	{
		return msgLevelColors.at( level );
	}

	void ColorConsole::ColorizeOutput( bool colorize )
	{
		coloredOutput = colorize;
	}

	// Other than some wierd color trailing on the right-hand side for some of the differently colored lines,
	// This now ACTUALLY works almost as intended
	void ColorConsole::SetConsoleInterface( console_interface mode )
	{
		consoleMode = mode;

#ifdef WINDOWS_PLATFORM
		if( mode == console_interface::std_out ) {
			outputHandle = GetStdHandle( STD_OUTPUT_HANDLE );
		}
		else {
			outputHandle = GetStdHandle( STD_ERROR_HANDLE );
		}
		DWORD opMode { 0 };
		if( !GetConsoleMode( outputHandle, &opMode ) ) {
			exit( GetLastError( ) );
		}
		opMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		if( !SetConsoleMode( outputHandle, opMode ) ) {
			exit( GetLastError( ) );
		}
#else
		if( mode == console_interface::std_out ) {
			outputHandle = stdout;
		}
		else {
			outputHandle = stderr;
		}
#endif  // WINDOWS_PLATFORM
	}

	console_interface ColorConsole::ConsoleInterface( )
	{
		return consoleMode;
	}

	// TODO: Work on this and see if this works as intended
	void ColorConsole::InitTerminalOutputs( )
	{
		terminalBuff = {
		{ console_interface::std_out, std::cout.rdbuf( ) },
		{ console_interface::std_err, std::cerr.rdbuf( ) },
		{ console_interface::std_log, std::clog.rdbuf( ) },
		};
	}

	// TODO: Work on this and see if this works as intended
	template <class BufferType> void SetOutputBuffer( console_interface mode, BufferType *buffer )
	{
		switch( mode ) {
			case console_interface::std_out: std::cout.rdbuf( buffer ); break;
			case console_interface::std_err: std::cerr.rdbuf( buffer ); break;
			case console_interface::std_log: std::clog.rdbuf( buffer ); break;
			default: break;
		}
	}

	// TODO: Work on this and see if this works as intended
	// dest probably shouldn't be a sv and if I'm going this route. This seems like the wrong way to go about this though...
	void ColorConsole::RedirectOutput( console_interface mode, std::string_view dest )
	{
		// store old buffer
		redirectionBackup.push_back( std::make_pair( mode, terminalBuff.at( mode ) ) );
		if( mode == console_interface::std_out ) {
			if( stdoutHandle.is_open( ) ) stdoutHandle.close( );
			ResetOutputBuffer( mode );
			stdoutHandle.open( dest );
			SetOutputBuffer( mode, stdoutHandle.rdbuf( ) );
		}
		else {
			if( stderrHandle.is_open( ) ) stderrHandle.close( );
			ResetOutputBuffer( mode );
			stderrHandle.open( dest );
			SetOutputBuffer( mode, stderrHandle.rdbuf( ) );
		}
	}

	// TODO: Work on this and see if this works as intended
	void ColorConsole::ResetOutputBuffer( console_interface mode )
	{
		for( auto &pair : redirectionBackup ) {
			if( pair.first == mode ) {
				auto originalBuffer { pair.second };
				switch( mode ) {
					case console_interface::std_out: std::cout.rdbuf( originalBuffer ); break;
					case console_interface::std_err: std::cerr.rdbuf( originalBuffer ); break;
					case console_interface::std_log: std::clog.rdbuf( originalBuffer ); break;
					default: break;
				}
			}
			break;  // end early if found
		}
	}

	bool ColorConsole::IsTerminalType( )
	{
		auto consoleType { ( consoleMode == console_interface::std_out ) ? stdout : stderr };
		return ( ISATTY( FILENO( consoleType ) ) ) ? true : false;
	}

	// TODO: Clean this up to look more succint
	void ColorConsole::PrintMessage( std::string_view formatted )
	{
		std::string_view msgColor { "" }, reset { "" };
		if( IsValidHandle( ) ) {
			if( IsTerminalType( ) && coloredOutput ) {
				msgColor = msgLevelColors.at( MsgInfo( )->MsgLevel( ) );
				reset    = se_colors::formats::reset;
			}
			std::string message;
			message.reserve( formatted.size( ) + msgColor.size( ) + reset.size( ) );
			message.append( msgColor.data( ), msgColor.size( ) );
			message.append( formatted.data( ), formatted.size( ) );
			message.append( reset.data( ), reset.size( ) );

#ifdef WINDOWS_PLATFORM
			/*
				From MSDN (https://docs.microsoft.com/en-us/windows/console/writeconsole):
				If the handle is not a console handle, the output is redirected and you should call WriteFile to perform the I/O.
			*/
			if( IsTerminalType( ) ) {
				WriteConsole( outputHandle, message.data( ), message.size( ), NULL, NULL );
			}
			else {
				WriteFile( outputHandle, message.data( ), message.size( ), NULL, NULL );
			}
#else
			fwrite( message.data( ), sizeof( char ), message.size( ), outputHandle );
#endif     // WINDOWS_PLATFORM
			terminalBuff.at( consoleMode )->pubsync( ); // flush in case redirected to file
		}  // IsValidHandle() Check
	}      // PrintMessage()

	void ColorConsole::SetOriginalColors( )
	{
		msgLevelColors = {
		{ LoggerLevel::trace, se_colors::bright_colors::combos::white::on_black },
		{ LoggerLevel::info, se_colors::bright_colors::foreground::green },
		{ LoggerLevel::debug, se_colors::bright_colors::foreground::cyan },
		{ LoggerLevel::warning, se_colors::bright_colors::foreground::yellow },
		{ LoggerLevel::error, se_colors::basic_colors::foreground::red },
		{ LoggerLevel::fatal, se_colors::bright_colors::combos::yellow::on_red },
		{ LoggerLevel::off, se_colors::formats::reset },
		};
	}

}  // namespace serenity::expiremental::targets
