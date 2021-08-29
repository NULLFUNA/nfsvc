#include <iostream>
#include <thread>

#include <windows.h>
#include <tchar.h>

bool	LoadConfig();
bool	TerminateRestrictedProcesses();

#define SERVICE_NAME "nfsvc"

SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE serviceStatusHandle;

void ServiceStop() {

	serviceStatus.dwCurrentState = SERVICE_STOPPED;
	serviceStatus.dwWin32ExitCode = -1;
	SetServiceStatus(serviceStatusHandle, &serviceStatus);

}

DWORD WINAPI HandlerEx(DWORD request, DWORD eventType, void *eventData, void *context) {

	switch( request ) {
		case SERVICE_CONTROL_STOP:
			serviceStatus.dwWin32ExitCode = 0;
			serviceStatus.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus (serviceStatusHandle, &serviceStatus);
		return NO_ERROR;

		case SERVICE_CONTROL_SHUTDOWN:
			serviceStatus.dwWin32ExitCode = 0;
			serviceStatus.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus (serviceStatusHandle, &serviceStatus);
		return NO_ERROR;

		default: break;
	}

	return NO_ERROR;
}

void ServiceMain( unsigned long argc, wchar_t** argv ) {

	int error;

	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwCurrentState = SERVICE_START_PENDING;
	serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	serviceStatus.dwWin32ExitCode = 0;
	serviceStatus.dwServiceSpecificExitCode = 0;
	serviceStatus.dwCheckPoint = 0;
	serviceStatus.dwWaitHint = 0;

	serviceStatusHandle = RegisterServiceCtrlHandlerEx( TEXT( SERVICE_NAME ), (LPHANDLER_FUNCTION_EX)HandlerEx, nullptr );

	if ( serviceStatusHandle == ( SERVICE_STATUS_HANDLE ) 0 )
		return;

	serviceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus (serviceStatusHandle, &serviceStatus);

	//	Load process name and max count from config
	if( !LoadConfig() ) {
		ServiceStop();
		return;
	}

	while ( serviceStatus.dwCurrentState == SERVICE_RUNNING ) {
		std::this_thread::sleep_for( std::chrono::seconds(1) );
		TerminateRestrictedProcesses();
	}

}

int main( int argc, char** argv ) {

	SERVICE_TABLE_ENTRY serviceTable[] = {
			{ _T(SERVICE_NAME), &ServiceMain },
			{ NULL, NULL }
	};

	if ( StartServiceCtrlDispatcher(serviceTable) )
			return 0;
	else if (GetLastError() == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT)
			MessageBoxA( nullptr, "Direct launching this application is unaviable! Please install it throw setup.", "Fatal error", 0 );
	else
			MessageBoxA( nullptr, "Something went wrong!", "Fatal error", 0 );

}
