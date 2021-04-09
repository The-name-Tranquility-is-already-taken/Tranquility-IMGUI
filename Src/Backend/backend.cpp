#include "backend.h"
#include "../Frontend/frontend.h"

bool Backend::RenderLoop()
{
	WNDCLASSEX wc =
	{
		sizeof(WNDCLASSEX),		//cbSize
		CS_CLASSDC,				//style
		WndProc,				//lpfnWndProc
		0L,						//cbClsExtra
		0L,						//cbWndExtra
		GetModuleHandle(NULL),	//hInstance
		NULL,					//hIcon
		NULL,					//hCursor
		NULL,					//hbrBackground
		NULL,					//lpszMenuName
		_T("ImGui Example"),	//lpszClassName
		NULL					//hIconSm
	};
	/*
	Register our WNDClass to our program so that Windows knows what class it is so that they can maintain information about it for further use.
	*/
	::RegisterClassEx(&wc);
	/*
		Create our window with our WndClass fields.
		_T is used so if we decide to change the string type from unicode to multibyte & vice-versa, we dont need to change a single line of code
		we pass our class instance so wndproc can be used even when its a static function
	*/
	HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX9 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, this);

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return EXIT_FAILURE;

	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(pd3dDevice, pd3dDeviceContext);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	//sets our background colour
	ImVec4 clearColour = ImVec4(0.45f, 0.55f, 0.60f, 255);

	while (msg.message != WM_QUIT)
	{

		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			continue;
		}
		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		Frontend::DrawInterface();
		ImGui::Render();
		pd3dDeviceContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
		pd3dDeviceContext->ClearRenderTargetView(mainRenderTargetView, (float*)&clearColour);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		pSwapChain->Present(1, 0); // Present with vsync
		//g_pSwapChain->Present(0, 0); // Present without vsync
	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);
	return false;
}
bool Backend::CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &pSwapChain, &pd3dDevice, &featureLevel, &pd3dDeviceContext) != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}


void Backend::CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (pSwapChain) { pSwapChain->Release(); pSwapChain = NULL; }
	if (pd3dDeviceContext) { pd3dDeviceContext->Release(); pd3dDeviceContext = NULL; }
	if (pd3dDevice) { pd3dDevice->Release(); pd3dDevice = NULL; }
}

void Backend::CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
	pBackBuffer->Release();
}

void Backend::CleanupRenderTarget()
{
	if (mainRenderTargetView) { mainRenderTargetView->Release(); mainRenderTargetView = NULL; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI Backend::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static Backend* pThis = nullptr;

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_NCCREATE:
		pThis = static_cast<Backend*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		break;
	case WM_SIZE:
		if (!pThis)
			return 0;
		if (pThis->pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			pThis->CleanupRenderTarget();
			pThis->pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			pThis->CreateRenderTarget();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
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
			return std::make_pair(res, jsonData);

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

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Callback);
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
			return std::make_pair(res, jsonData);
		
	}
	return{};
}

bool Backend::DeleteMemberWithID(std::string id)
{
	CURL* curl;
	CURLcode res;

	/* get a curl handle */
	curl = curl_easy_init();
	if (curl)
	{
		std::string idrfk = std::string("http://localhost:322/api/member/") + id;
		curl_easy_setopt(curl, CURLOPT_URL, idrfk.c_str());

		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			return false;
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
		return true;
	}
	return false;

}



extern Backend* gBackend = new Backend();