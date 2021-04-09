#include "backend.h"
#include "../Frontend/frontend.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

bool Backend::RenderLoop()
{
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(1280, 800, "Tranquility Dev Pannel", NULL, NULL);
	this->ourHWND = glfwGetWin32Window(window);
	
	if (window == NULL)
		return 1;

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
	// Initialize OpenGL loader
	#pragma region errors
	#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
		bool err = gl3wInit() != 0;
	#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
		bool err = glewInit() != GLEW_OK;
	#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
		bool err = gladLoadGL() == 0;
	#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
		bool err = gladLoadGL(glfwGetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
	#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
		bool err = false;
		glbinding::Binding::initialize();
	#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
		bool err = false;
		glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
	#else
		bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
	#endif
	#pragma endregion

	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImVec4 clearColour = ImVec4(0.45f, 0.55f, 0.60f, 255);

	while (!glfwWindowShouldClose(window))
	{

		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		Frontend::DrawInterface();

		ImGui::Render();

		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clearColour.x, clearColour.y, clearColour.z, clearColour.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}
	// Cleanup

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	return false;
}


std::size_t Callback(
		const char* in,
		std::size_t size,
		std::size_t num,
		std::string* out
)
{
	const std::size_t totalBytes(size * num);
	out->append(in, totalBytes);
	return totalBytes;
}


std::pair<CURLcode,Json::Value> Backend::CreateUser(
	std::string tag,
	std::string email,
	std::string id,
	std::string phoneNumber,
	std::string passwordHash,
	std::string token
)
{
	CURL* curl;
	CURLcode res;

	/* get a curl handle */
	curl = curl_easy_init();
	if (curl) 
	{
		/*Create fields */
		std::string fields;
		fields += "tag=" + tag + "&";
		fields += "email=" + email + "&";
		fields += "id=" + id + "&";
		fields += "phoneNumber=" + phoneNumber + "&";
		fields += "hash=" + passwordHash + "&";
		fields += "token=" + token;
		std::cout << fields <<std::endl;
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be a https:// URL if that is what should receive the
		   data. 
		*/
		curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:322/api/member");
		/* Now specify the POST data */
		curl_easy_setopt(curl,
			CURLOPT_POSTFIELDS, 
			fields.c_str()
		);

		std::unique_ptr<std::string> httpData(new std::string());

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());


		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));

		/* always cleanup */
		curl_easy_cleanup(curl);
		Json::Value jsonData;
		Json::Reader jsonReader;

		if (jsonReader.parse(*httpData, jsonData))
		{
			std::cout << "Successfully parsed JSON data" << std::endl;
			return std::make_pair(res, jsonData);

		}

	}
	return {};

}


std::pair<CURLcode, Json::Value> Backend::GetMembers()
{
	CURL* curl;
	CURLcode res;

	/* get a curl handle */
	curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:322/api/member");
		std::unique_ptr<std::string> httpData(new std::string());

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());


		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

		/* always cleanup */
		curl_easy_cleanup(curl);
		Json::Value jsonData;
		Json::Reader jsonReader;

		if (jsonReader.parse(*httpData, jsonData))
		{
			return std::make_pair(res, jsonData);
		}
	}
	return{};
}





extern Backend* gBackend = new Backend();