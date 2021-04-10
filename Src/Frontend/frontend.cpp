#include "frontend.h"
#include <functional>
void UserTab()
{
	static auto createUserTextSize = ImGui::CalcTextSize("Create User \n ");
	static auto listUsersTextSize = ImGui::CalcTextSize("list Users \n ");
	static auto removeUserTextSize = ImGui::CalcTextSize("Remove User \n ");

	static std::vector<Json::Value> data;
	static std::string previousUserResponse;
	static bool createUserPopup = false;
	static bool removeUserPopup = false;

	ImGui::Columns(2, 0, false);

	if (ImGui::Button("Create User", createUserTextSize))
	{
		createUserPopup = !createUserPopup;
	}
	
	if (createUserPopup)
	{
		ImGui::SetNextWindowSize({ 380,220 });

		ImGui::Begin("##CreateUserPopup", &createUserPopup, ImGuiWindowFlags_NoResize);
		{

			//ImGui::SetWindowFocus();
			static char tag[64];
			ImGui::InputText("Tag", tag, IM_ARRAYSIZE(tag));

			static char email[128];
			ImGui::InputText("Email", email, IM_ARRAYSIZE(email));

			static char id[128];
			ImGui::InputText("Id", id, IM_ARRAYSIZE(id));

			static char phoneNumber[128];
			ImGui::InputText("Phone Number", phoneNumber, IM_ARRAYSIZE(phoneNumber));

			static char passwordHash[UINT16_MAX];
			ImGui::InputText("Password Hash", passwordHash, IM_ARRAYSIZE(passwordHash));

			static char token[256];
			ImGui::InputText("Token", token, IM_ARRAYSIZE(token));
			if (ImGui::Button("Submit", { 48,30 }))
			{
				auto userResponse = gBackend->CreateUser(tag, email, id, phoneNumber, passwordHash, token);

				if (userResponse.first == CURLE_OK)
				{
					previousUserResponse = userResponse.second.toStyledString();
				}
				else
				{
					//display errror message containing what is required
				}
				createUserPopup = false;

				//set our strings back to null
				//////////////////////////////////////
				RtlZeroMemory(tag, sizeof(tag));
				RtlZeroMemory(email, sizeof(email));
				RtlZeroMemory(id, sizeof(id));
				RtlZeroMemory(phoneNumber, sizeof(phoneNumber));
				RtlZeroMemory(passwordHash, sizeof(passwordHash));
				RtlZeroMemory(token, sizeof(token));
				//////////////////////////////////////
			}
		}
		ImGui::End();
	}

	if (ImGui::Button("Remove User", removeUserTextSize))
	{
		removeUserPopup = !removeUserPopup;
	}

	if (removeUserPopup)
	{
		ImGui::SetNextWindowSize({ 380,220 });

		ImGui::Begin("##RemoveUserPopup", &removeUserPopup, ImGuiWindowFlags_NoResize);
		{
			static char userId[128];
			ImGui::InputText("ID", userId, IM_ARRAYSIZE(userId));
			if (ImGui::Button("Submit", { 48,30 }))
			{
				if (gBackend->DeleteMemberWithID(userId))
				{
					MessageBox(NULL, _T("Succsess"), _T("Removal was succsessfull"), MB_OK);
				}
				else
				{
					MessageBox(NULL, _T("Error"), _T("Removal Failed"), MB_OK);
				}
				removeUserPopup = !removeUserPopup;
			}

		}
		ImGui::End();
	}



	if (!previousUserResponse.empty())
	{
		ImGui::Text("Response :\n");
		ImGui::TextWrapped(previousUserResponse.c_str());
	}

	ImGui::NextColumn();

	if (ImGui::Button("List Users", listUsersTextSize))
	{
		/*
			clears so we dont have a long list of duplicates
		*/
		data.clear();

		auto users = gBackend->GetMembers();
		if (users.first == CURLE_OK)
		{
			auto usersObj = users.second;
			/*
				UINT32 since we dont know how many users will exist, and we dont want the index going over the max range
			*/
			for (UINT32 i = 0; i < usersObj.size(); ++i)
			{
				data.push_back(usersObj[i]);
				/*if (usersObj[i].isMember("id"))
					std::cout << usersObj[i]["id"].asString() << std::endl;*/
			}
		}
	}
	static int currentIndx = 0;
	//ImGui::SetCursorPos({(winPos.x + winSize.x) / 2, listUsersTextSize.y+20 });
	if (data.size() > 0)
	{
		/*
			Utilizes lambdas to dynamically loop though and display all existing users via tags 
		*/
		if (ImGui::ListBox("##UserList", &currentIndx, [](void* data, int idx, const char** out_text)
			{
				auto& vector = *static_cast<std::vector<Json::Value>*>(data);
				*out_text = vector.at(idx)["tag"].asCString();
				return true;
			}, &data, data.size(), 10));
		if (currentIndx != -1 && data.size() >= currentIndx+1)
		{
			/*
				Stored as string instead of cstring because it dies otherwise, i'll have more info later
			*/
			std::string selectedUsrObject = data[currentIndx].toStyledString();

			auto textSize = ImGui::CalcTextSize(selectedUsrObject.c_str());
			ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetCursorScreenPos(),
				{ ImGui::GetCursorScreenPos().x + textSize.x,ImGui::GetCursorScreenPos().y + textSize.y + 10 },
				ImGui::ColorConvertFloat4ToU32({ 46, 49, 49, 0.2 }), 5.f);

			ImGui::TextDisabled(selectedUsrObject.c_str());

		}
	}

	/*for (auto text : data)
		ImGui::TextWrapped(text.c_str());*/

}



const char* options[] =
{
	"User Data",
	"Roles",
	"Guilds"
	/*To be added*/
};
int optionsPos = -1;
void Frontend::DrawInterface()
{
	ImGui::SetNextWindowSize({ 845,560 });
	ImGui::Begin("##Main Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	{				
		auto p = ImGui::GetWindowPos();
		//ImGui::GetOverlayDrawList()->AddText({ p.x + ImGui::GetWindowSize().x, p.y}, ImColor(9, 169, 232, int(230 * alphamodulate)), "Franxx Rust External");

		//auto cursorScreenPos = ImGui::GetWindowPos();
		//ImGui::SetCursorPosX(ImGui::GetWindowPos().x + (ImGui::GetWindowSize().x - ImGui::CalcTextSize("Options").x - 50));
		//ImGui::SetCursorPosY(ImGui::GetWindowPos().y);

		ImGui::GetOverlayDrawList()->AddLine({ p.x + ImGui::GetWindowSize().x - 30, p.y	+ 10 }, { p.x + ImGui::GetWindowSize().x - 10, p.y + 10 }, ImGui::ColorConvertFloat4ToU32({ 255,255,255,255 }));
		ImGui::GetOverlayDrawList()->AddLine({ p.x + ImGui::GetWindowSize().x - 30, p.y + 20 }, { p.x + ImGui::GetWindowSize().x - 10, p.y + 20 }, ImGui::ColorConvertFloat4ToU32({ 255,255,255,255 }));
		ImGui::GetOverlayDrawList()->AddLine({ p.x + ImGui::GetWindowSize().x - 30, p.y + 30 }, { p.x + ImGui::GetWindowSize().x - 10, p.y + 30 }, ImGui::ColorConvertFloat4ToU32({ 255,255,255,255 }));
		//ImGui::SetCursorPosX(p.x + ImGui::GetWindowSize().x - 30);
		static bool burgerMenuPressed;
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 40);
		
		if (ImGui::InvisibleButton("##BurgerMenu", ImVec2(50, 20)))
			burgerMenuPressed = !burgerMenuPressed;
		
		if (burgerMenuPressed)
		{
			
			//ImGui::PushStyleColor(ImGuiCol_ChildBg, {0,0,0.5,0.2});
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 70);
			ImGui::SetCursorPosY(40);

			if (ImGui::BeginChild("##BurgerChild", ImVec2(108, 80)))
			{
				//ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 98);
				ImGui::ListBox("##BurgerOptions", &optionsPos, options, IM_ARRAYSIZE(options));

			}ImGui::EndChild();
		}
		switch (optionsPos)
		{
		case 0:
			UserTab();
			break;
		case 1:
			break;
		case 2:
			break;
		default:
			break;
		}

	}
	ImGui::End();
}