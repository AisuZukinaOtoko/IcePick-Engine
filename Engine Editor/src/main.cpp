#include "EditorApplicationImpl.h"

int main(int argc, char** argv) {
	EditorApplication application;
	application.Init();
	application.Run();
	application.Terminate();
	return 0;
}