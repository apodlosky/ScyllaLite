#pragma once

#include <windows.h>
#include <unordered_map>

class ImportThunk;
class ImportModuleThunk;

class ImportsHandling
{
public:
	std::unordered_map<DWORD_PTR, ImportModuleThunk> moduleList;
	std::unordered_map<DWORD_PTR, ImportModuleThunk> moduleListNew;

	ImportsHandling();
	~ImportsHandling();

	unsigned int thunkCount() const { return m_thunkCount; }
	unsigned int invalidThunkCount() const { return m_invalidThunkCount; }
	unsigned int suspectThunkCount() const { return m_suspectThunkCount; }

	void clearAllImports();

	//bool addImport(const WCHAR * moduleName, const CHAR * name, DWORD_PTR va, DWORD_PTR rva, WORD ordinal = 0, bool valid = true, bool suspect = false);
	//bool addModule(const WCHAR * moduleName, DWORD_PTR firstThunk);

	void scanAndFixModuleList();

private:
	DWORD numberOfFunctions;

	unsigned int m_thunkCount;
	unsigned int m_invalidThunkCount;
	unsigned int m_suspectThunkCount;

	void updateCounts();

	bool addModuleToModuleList(const WCHAR * moduleName, DWORD_PTR firstThunk);
	void addUnknownModuleToModuleList(DWORD_PTR firstThunk);
	bool addNotFoundApiToModuleList(const ImportThunk * apiNotFound);
	bool addFunctionToModuleList(const ImportThunk * apiFound);
	bool isNewModule(const WCHAR * moduleName);
};
