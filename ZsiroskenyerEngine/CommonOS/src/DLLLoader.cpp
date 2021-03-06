// DLLLoader.cpp implementation
#include "DLLLoader.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "../../Core/src/zsString.h"
#include "../../Core/src/common.h"

zsString cDLLLoader::GetExecutablePath() const {
	WCHAR buf[MAX_PATH];
	GetModuleFileName(nullptr, buf, MAX_PATH);
	zsString s = buf;
	size_t idx = s.find_last_of('\\');
	s = s.substr(0, idx + 1);
	return s;
}

IDLLLoader::DLLMODULE cDLLLoader::LoadDLL(const zsString& libName) const {
	HMODULE dll = LoadLibrary((GetExecutablePath() + libName).c_str());
	if (!dll) {
		ILog::GetInstance()->MsgBox(L"FAILED TO LOAD LIBRARY: " + libName);
		exit(1);
	}
	return dll;
}

IDLLLoader::DLLFUNCTION cDLLLoader::GetDLLFunction(IDLLLoader::DLLMODULE dll, const zsString& funcName) const {
	char ansiChars[256];

	wcstombs(ansiChars, funcName.c_str(), 256);

	auto function =  GetProcAddress((HMODULE)dll, ansiChars);
	if (!function) {
		MessageBoxW(nullptr, ( L"FAILED TO GET PROC ADDRESS: " + funcName).c_str(), L"FUCK...", MB_OK|MB_ICONERROR);
		exit(2);
	}
	return (DLLFUNCTION)function;
}