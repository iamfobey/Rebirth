/*
*  Thank you for staying with us.
*  (c) Oneiro Games 2019 - 2021
*/

#include <SandBoxApp.h>

int main()
{
	SandBoxApp* app = new SandBoxApp;
	try
	{
		app->Run();
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << '\n';
		spdlog::get("log")->error(ex.what());

		delete app;

		return EXIT_FAILURE;
	}

	delete app;

	return EXIT_SUCCESS;
}