#pragma once
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <json/json.h>
#define CURL_STATICLIB
#include <curl/curl.h>
#include <d3d11.h>

#include "../IMGUI-Dependencies/imgui.h"
#include "../IMGUI-Dependencies/imgui_impl_win32.h"
#include "../IMGUI-Dependencies/imgui_impl_dx11.h"

#include <stdio.h>


class Backend
{
private:
	ID3D11Device* pd3dDevice = NULL;
	ID3D11DeviceContext* pd3dDeviceContext = NULL;
	IDXGISwapChain* pSwapChain = NULL;
	ID3D11RenderTargetView* mainRenderTargetView = NULL;
public:
	bool RenderLoop();
	// Forward declarations of helper functions
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
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
	std::pair<CURLcode,Json::Value> CreateUser(
		std::string tag,
		std::string email,
		std::string phoneNumber,
		std::string password
	);
	/// <summary>
	/// Lists all existing Members
	/// </summary>
	/// <returns>A CURLcode and then the jsonObj returned</returns>
	std::pair<CURLcode, Json::Value> GetMembers();
	bool DeleteMemberWithID(std::string id);
};



extern Backend* gBackend;