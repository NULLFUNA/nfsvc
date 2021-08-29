#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#define NFSVC_VERSION 1

namespace nullFuna::Setup {

	void Configure( int processMaxCount, bool foregroundEnabled, const char* processName );

	void InstallFiles();
	void InstallService();

	void UninstallService();

}

#endif // GLOBAL_HPP
