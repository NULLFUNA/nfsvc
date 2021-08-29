#include <global.hpp>
#include <QFile>
#include <QDir>

static int	g_processMaxCount	= 0;
static bool	g_processForegroundEnabled = false;
static char	g_processName[128];

void nullFuna::Setup::Configure( int processMaxCount, bool foregroundEnabled, const char* processName ) {
	g_processMaxCount = processMaxCount;
	g_processForegroundEnabled = foregroundEnabled;
	strcpy_s( g_processName, 128, processName );
}

void nullFuna::Setup::InstallFiles() {

	QDir installDirectory;
	installDirectory.mkdir( "C:/Program Files/NFSVC" );

	QFile::copy(":/files/data/nfsvc.exe", "C:/Program Files/NFSVC/nfsvc.exe");
	QFile::copy(":/files/data/libgcc_s_seh-1.dll", "C:/Program Files/NFSVC/libgcc_s_seh-1.dll");
	QFile::copy(":/files/data/libstdc++-6.dll", "C:/Program Files/NFSVC/libstdc++-6.dll");
	QFile::copy(":/files/data/libwinpthread-1.dll", "C:/Program Files/NFSVC/libwinpthread-1.dll");

	QFile file( "C:/Program Files/NFSVC/settings.cfg" );
	if ( file.open( QIODevice::ReadWrite ) ) {
			QTextStream stream( &file );

			//	Write service config
			stream << g_processMaxCount << Qt::endl;
			stream << g_processForegroundEnabled << Qt::endl;
			stream << g_processName << Qt::endl;

			//	Write service version
			stream << NFSVC_VERSION << Qt::endl;

			file.close();
	}

}


void nullFuna::Setup::InstallService() {
	system( "sc create \"Controlling Running Processes (NFSVC)\" binPath= \"C:\\Program Files\\NFSVC\\nfsvc.exe\"" );
}

void nullFuna::Setup::UninstallService() {
	system( "sc delete \"Controlling Running Processes (NFSVC)\"" );
}
