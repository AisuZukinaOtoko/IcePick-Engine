#include "RuntimeApplicationImpl.h"

int main(int argc, char** argv) {
	RuntimeApplication runtimeApplication;
	runtimeApplication.Init();
	runtimeApplication.Run();
	runtimeApplication.Terminate();
	return 0;
}