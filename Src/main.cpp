#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include "Backend/backend.h"

/*
We are using a console based application for now by default 
*/
int main(int, char**)
{
	/* In windows, this will init the winsock stuff */
	curl_global_init(CURL_GLOBAL_ALL);
	gBackend->RenderLoop();

	/* cleanup on exit */
	curl_global_cleanup();

	return EXIT_SUCCESS;
}

