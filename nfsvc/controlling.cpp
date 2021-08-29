#include <iostream>
#include <vector>
#include <fstream>
#include <thread>
#include <string>

#include <windows.h>
#include <tchar.h>
#include <psapi.h>

struct ProcessInfo {
	std::string		m_Name;
	DWORD					m_PID;
};

static int	g_processMaxCount	= 0;
static bool	g_processForegroundEnabled = false;
static char	g_processName[128];

bool	LoadConfig() {

	std::ifstream configFile( "C:/Program Files/NFSVC/settings.cfg" );
	char buffer[128];

	if( !configFile.is_open() )
		return false;

	configFile >> g_processMaxCount;
	configFile >> g_processForegroundEnabled;
	configFile >> g_processName;

	configFile.close();
	return true;
}

HWND GetWindowHandleByPID( DWORD processID ) {
		HWND hSearch = nullptr;
		do {
				hSearch = FindWindowEx( nullptr, hSearch, nullptr, nullptr );
				DWORD checkProcessID = 0;
				GetWindowThreadProcessId( hSearch, &checkProcessID );

				if ( checkProcessID == processID )
					return hSearch;

		}
		while ( hSearch != nullptr );
}

bool	TerminateRestrictedProcesses() {

	DWORD processesIDs[1024];
	DWORD processesCount;
	int		restrictedProcessesCount = 0;

	DWORD foregroundPID = 0;
	bool	foregroundRequired = false;

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
		std::wstring wString = szProcessName;
		std::string processName = std::string( wString.begin(), wString.end() );

		if( processName == g_processName ) {
			restrictedProcessesCount++;

			if( !foregroundPID )
				foregroundPID = processesIDs[i];

			//	Terminate it!!
			if( restrictedProcessesCount > g_processMaxCount ) {
				std::cout << "Terminating process: " << processesIDs[i] << " " << processName.c_str() << std::endl;
				TerminateProcess( hProcess, 1 );
				foregroundRequired = true;
			}

		}

		// Release the handle to the process.
		CloseHandle( hProcess );
	}

	if( foregroundRequired && g_processForegroundEnabled )
		SetForegroundWindow( GetWindowHandleByPID( foregroundPID ) );


	return true;
}

//int main() {

//	//	Load process name and max count from config
//	if( !LoadConfig() ) {
//		std::cout << "Error: LoadConfig returned false" << std::endl;
//		return 1;
//	}

//	//	Service loop
//	while( true ) {

//		std::this_thread::sleep_for( std::chrono::seconds(1) );

//		if( !TerminateRestrictedProcesses() ) {
//			std::cout << "Error: TerminateRestrictedProcesses returned false" << std::endl;
//			continue;
//		}

//	}

//	//	never will be shutdown
//	return 1;
//}

