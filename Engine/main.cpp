//extern "C" {
//	_declspec(dllexport) unsigned NvOptimusEnablement = 0x00000001;
//}

//#include <Math/Vectors.hpp>

#include "Application.h"

int main() {
	Application app;

	app.Initialize(1600, 900, "Allure 2D", false);
	app.Run();
	app.Exit();

	return 0;
}