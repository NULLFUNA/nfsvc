#include <iostream>
#include <vector>
#include <fstream>
#include <thread>
#include <string>

#include <windows.h>
#include <tchar.h>
#include <psapi.h>
#include <cstdarg>

void	Log( const char* fmt, ... ) {
#ifdef _DEBUG
	va_list		argptr;
	char		text[1024];

	va_start( argptr, fmt );
	vsnprintf( text, sizeof(text), fmt, argptr );
	va_end( argptr );

	FILE*	logFile = fopen( "C:/Program Files/NFSVC/service.log", "a" );

	if( !logFile )
		return;

	fputs( text, logFile );
	fclose( logFile );
#endif
}

static int	g_processMaxCount	= 0;
static char	g_processName[128];

void	CreateConfig( const char* processMaxCount, const char* processName ) {
	std::ofstream configFile( "settings.cfg", std::ios::trunc );

	if( !configFile.is_open() ) {
		Log( "Failed to create config\n" );
		return;
	}

	configFile << processMaxCount << std::endl;
	configFile << processName << std::endl;

	configFile.close();

	Log( "Config created successfully\n" );
}

bool	LoadConfig() {

	std::ifstream configFile( "C:/Program Files/NFSVC/settings.cfg" );
	char buffer[128];

	if( !configFile.is_open() ) {
		Log( "Failed to open config\n" );
		return false;
	}

	configFile >> g_processMaxCount;
	configFile >> g_processName;

	configFile.close();
	Log( "Config loaded\n" );
	return true;
}

bool	TerminateRestrictedProcesses() {

	DWORD processesIDs[1024];
	DWORD processesCount;
	int		restrictedProcessesCount = 0;

	//	Get list of processes
	if ( !EnumProcesses( processesIDs, sizeof( processesIDs ), &processesCount ) )
		return false;

	//	Get count of processes
	processesCount = processesCount / sizeof( DWORD );

	for( unsigned int i = 0; i < processesCount; i++ ) {

		TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

		// Get a handle to the process.
		HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, processesIDs[i] );

		// Get the process name.
		if ( hProcess != nullptr ) {
			HMODULE hMod;
			DWORD cbNeeded;

			if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
				GetModuleBaseName( hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(TCHAR) );

		}

		//	Convert to std::string
		std::wstring wString		= szProcessName;
		std::string processName = std::string( wString.begin(), wString.end() );

		if( processName == g_processName ) {
			restrictedProcessesCount++;
			Log( "Restricted process: ID %d NAME %s\n", processesIDs[i], g_processName );

			//	Terminate it!!
			if( restrictedProcessesCount > g_processMaxCount )
				TerminateProcess( hProcess, 1 );

		}

		// Release the handle to the process.
		CloseHandle( hProcess );
	}

	return true;
}

