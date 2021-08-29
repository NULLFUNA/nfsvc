#include <iostream>
#include <global.hpp>
using namespace std;

int main() {

	nullFuna::Setup::Configure( 0, false, "chrome.exe" );
	nullFuna::Setup::InstallFiles();
	nullFuna::Setup::InstallService();

	return 0;
}
