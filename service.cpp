#include <iostream>
#include <thread>

#include <windows.h>
#include <tchar.h>

void	CreateConfig( const char* processMaxCount, const char* processName );
bool	LoadConfig();
bool	TerminateRestrictedProcesses();

#define SERVICE_NAME "nfsvc"

SERVICE_STATUS				g_serviceStatus;
SERVICE_STATUS_HANDLE g_serviceStatusHandle;

void ServiceStop() {

	g_serviceStatus.dwCurrentState	= SERVICE_STOPPED;
	g_serviceStatus.dwWin32ExitCode = -1;
	SetServiceStatus( g_serviceStatusHandle, &g_serviceStatus );

}

DWORD WINAPI ServiceHandlerEx( DWORD Request, DWORD eventType, void *eventData, void *Context ) {

	switch( Request ) {
		case SERVICE_CONTROL_STOP:
			g_serviceStatus.dwWin32ExitCode = 0;
			g_serviceStatus.dwCurrentState	= SERVICE_STOPPED;
			SetServiceStatus( g_serviceStatusHandle, &g_serviceStatus );
		return NO_ERROR;

		case SERVICE_CONTROL_SHUTDOWN:
			g_serviceStatus.dwWin32ExitCode = 0;
			g_serviceStatus.dwCurrentState	= SERVICE_STOPPED;
			SetServiceStatus( g_serviceStatusHandle, &g_serviceStatus );
		return NO_ERROR;

		default: break;
	}

	return NO_ERROR;
}

void ServiceMain( unsigned long argc, wchar_t** argv ) {

	g_serviceStatus.dwServiceType							= SERVICE_WIN32_OWN_PROCESS;
	g_serviceStatus.dwCurrentState						= SERVICE_START_PENDING;
	g_serviceStatus.dwControlsAccepted				= SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	g_serviceStatus.dwWin32ExitCode						= 0;
	g_serviceStatus.dwServiceSpecificExitCode = 0;
	g_serviceStatus.dwCheckPoint							= 0;
	g_serviceStatus.dwWaitHint								= 0;

	g_serviceStatusHandle = RegisterServiceCtrlHandlerEx( TEXT( SERVICE_NAME ), (LPHANDLER_FUNCTION_EX)ServiceHandlerEx, nullptr );

	if ( g_serviceStatusHandle == ( SERVICE_STATUS_HANDLE ) 0 )
		return;

	g_serviceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus( g_serviceStatusHandle, &g_serviceStatus );

	//	Load process name and max count from config
	if( !LoadConfig() ) {
		ServiceStop();
		return;
	}

	while ( g_serviceStatus.dwCurrentState == SERVICE_RUNNING ) {
		std::this_thread::sleep_for( std::chrono::seconds(1) );
		TerminateRestrictedProcesses();
	}

}


int main( int argc, char** argv ) {

	if( argc == 4 )
		if( !stricmp( argv[1], "-config" ) ) {
			CreateConfig( argv[2], argv[3] );
			MessageBoxA( nullptr, "Success.", "NFSVC Registration",
									 MB_SETFOREGROUND );
			return 0;
		}


	SERVICE_TABLE_ENTRY serviceTable[] = {
			{ _T(SERVICE_NAME), &ServiceMain },
			{ NULL, NULL }
	};

	if ( StartServiceCtrlDispatcher(serviceTable) )
			return 0;
	else if (GetLastError() == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT)
			MessageBoxA( nullptr, "Direct launching this application is unaviable! Please install it throw command line.", "NFSVC",
									 MB_ICONERROR | MB_SETFOREGROUND );
	else
			MessageBoxA( nullptr, "Something went wrong!", "NFSVC",
									 MB_ICONERROR | MB_SETFOREGROUND );

}
