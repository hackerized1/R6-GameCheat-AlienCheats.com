#pragma once
#include "IOCTL_Define.h"
#include <TlHelp32.h>
#include <vector>

class MemoryManagement
{
public:
	HANDLE hDevice;
	uint64_t ModuleBase;
	bool Init(string ProcessName);
	template<typename T> T RPM(uint64_t lpBaseAddress);
	BOOL Read_Buf(uint64_t lpBaseAddress, uint64_t buf, size_t bufersize);
	template<typename T> void WPM(uint64_t lpBaseAddress, T Val);
	template<typename T> void WPM(uint64_t lpBaseAddress, T Val, bool isIgnoreProtect);
	uint32_t GetPIDByProcessName(string ProcessName);
	uint64_t GetModuleBase();
};

bool MemoryManagement::Init(string ProcessName)
{
	DWORD Bytes;
	this->hDevice = CreateFileW(DRIVER_DEVICE_PATH, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	if (this->hDevice == INVALID_HANDLE_VALUE)
		return 0;;
	_DRIVER_INIT tmp;
	tmp.ProcessId = this->GetPIDByProcessName(ProcessName);
	if (!tmp.ProcessId)
		return false;
	DeviceIoControl(this->hDevice, IOCTL_DRIVER_INIT, &tmp, sizeof(tmp), &tmp, sizeof(tmp), &Bytes, 0);
	this->ModuleBase = this->GetModuleBase();
	if (this->ModuleBase)
		return true;
	else
		return false;
}

BOOL MemoryManagement::Read_Buf(uint64_t lpBaseAddress, uint64_t buf, size_t bufersize)
{
	DWORD Bytes;
	DRIVER_MANAGE_MEMORY copy = {};
	copy.Src = (ULONGLONG)buf;
	copy.Dst = lpBaseAddress;
	copy.Size = bufersize;
	copy.isWrite = false;
	copy.isIgnoreProtect = false;

	if (DeviceIoControl(this->hDevice, IOCTL_DRIVER_MANAGE_MEMORY, &copy, sizeof(copy), &copy, sizeof(copy), &Bytes, NULL))
		return TRUE;
	else
		return FALSE;
}

template<typename T> T MemoryManagement::RPM(uint64_t lpBaseAddress)
{
	DWORD Bytes;
	T Ret{};
	DRIVER_MANAGE_MEMORY copy = {};
	copy.Src = (ULONGLONG)&Ret;
	copy.Dst = lpBaseAddress;
	copy.Size = sizeof(Ret);
	copy.isWrite = false;
	copy.isIgnoreProtect = false;
	DeviceIoControl(this->hDevice, IOCTL_DRIVER_MANAGE_MEMORY, &copy, sizeof(copy), &copy, sizeof(copy), &Bytes, 0);
	return Ret;
}

template<typename T> void MemoryManagement::WPM(uint64_t lpBaseAddress, T Val)
{
	DWORD Bytes;
	T Ret;
	Ret = Val;
	DRIVER_MANAGE_MEMORY copy = {};
	copy.Src = (ULONGLONG)&Ret;
	copy.Dst = lpBaseAddress;
	copy.Size = sizeof(Ret);
	copy.isWrite = true;
	copy.isIgnoreProtect = false;
	DeviceIoControl(this->hDevice, IOCTL_DRIVER_MANAGE_MEMORY, &copy, sizeof(copy), &copy, sizeof(copy), &Bytes, 0);
	return;
}

template<typename T> void MemoryManagement::WPM(uint64_t lpBaseAddress, T Val, bool fuckprotect)
{
	DWORD Bytes;
	T Ret{};
	Ret = Val;
	DRIVER_MANAGE_MEMORY copy = {};
	copy.Src = (ULONGLONG)&Ret;
	copy.Dst = lpBaseAddress;
	copy.Size = sizeof(Ret);
	copy.isWrite = true;
	copy.isIgnoreProtect = fuckprotect;
	DeviceIoControl(this->hDevice, IOCTL_DRIVER_MANAGE_MEMORY, &copy, sizeof(copy), &copy, sizeof(copy), &Bytes, 0);
	return;
}


uint32_t MemoryManagement::GetPIDByProcessName(string ProcessName)
{
	DWORD Result = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 ProcEntry;
	ProcEntry.dwSize = sizeof(ProcEntry);
	do
		if (!strcmp(ProcEntry.szExeFile, ProcessName.c_str()))
		{
			Result = ProcEntry.th32ProcessID;
		}
	while (Process32Next(hSnap, &ProcEntry));
	if (hSnap)
		CloseHandle(hSnap);
	return Result;
}

uint64_t MemoryManagement::GetModuleBase()
{
	DWORD Bytes;
	_GET_BASE_ADDRESS ModuleBase;
	uint64_t ModulePtr = 0;
	ModuleBase.Result = &ModulePtr;
	DeviceIoControl(this->hDevice, IOCTL_DRIVER_GET_BASE_ADDRESS, &ModuleBase, sizeof(ModuleBase), &ModuleBase, sizeof(ModuleBase), &Bytes, 0);
	return ModulePtr;
}

inline MemoryManagement* mem = new MemoryManagement();