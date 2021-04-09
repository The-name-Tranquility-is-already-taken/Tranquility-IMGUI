#pragma once
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <json/json.h>
#define CURL_STATICLIB
#include <curl/curl.h>



#include "../IMGUI-Dependencies/imgui.h"
#include "../IMGUI-Dependencies/imgui_impl_glfw.h"
#include "../IMGUI-Dependencies/imgui_impl_opengl3.h"
#include <stdio.h>

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>          // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h>            // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif


class Backend
{
private:
	HWND ourHWND;
	// Data
	//friend class Frontend;
public:
	bool RenderLoop();

	/// <summary>
	/// Creates a user
	/// </summary>
	/// <param name="tag">Username#userid#serverid</param>
	/// <param name="email">Basic Email Address</param>
	/// <param name="id">Globalally Unique User ID</param>
	/// <param name="phoneNumber">Phone Number</param>
	/// <param name="passwordHash">Hash of Password</param>
	/// <param name="token">Placeholder</param>
	/// <returns>A CURLcode and then the jsonObj returned</returns>
	std::pair<CURLcode,Json::Value> Backend::CreateUser(
		std::string tag,
		std::string email,
		std::string id,
		std::string phoneNumber,
		std::string passwordHash,
		std::string token
	);
	/// <summary>
	/// Lists all existing Members
	/// </summary>
	/// <returns>A CURLcode and then the jsonObj returned</returns>
	std::pair<CURLcode, Json::Value> Backend::GetMembers();
};



extern Backend* gBackend;