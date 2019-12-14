#include "ImportsHandling.h"

#include "Thunks.h"
#include "Architecture.h"

//#define DEBUG_COMMENTS

ImportsHandling::ImportsHandling()
{
	m_thunkCount = m_invalidThunkCount = m_suspectThunkCount = 0;
}

ImportsHandling::~ImportsHandling()
{
}

void ImportsHandling::updateCounts()
{
	m_thunkCount = m_invalidThunkCount = m_suspectThunkCount = 0;

	auto it_module = moduleList.begin();
	while (it_module != moduleList.end())
	{
		ImportModuleThunk &moduleThunk = it_module->second;

		auto it_import = moduleThunk.thunkList.begin();
		while (it_import != moduleThunk.thunkList.end())
		{
			ImportThunk &importThunk = it_import->second;

			m_thunkCount++;
			if(!importThunk.valid)
				m_invalidThunkCount++;
			else if(importThunk.suspect)
				m_suspectThunkCount++;

			it_import++;
		}

		it_module++;
	}
}

void ImportsHandling::clearAllImports()
{
	moduleList.clear();
	updateCounts();
}

/*bool ImportsHandling::addImport(const WCHAR * moduleName, const CHAR * name, DWORD_PTR va, DWORD_PTR rva, WORD ordinal, bool valid, bool suspect)
{
	ImportThunk import;
	ImportModuleThunk  * module = 0;
	std::map<DWORD_PTR, ImportModuleThunk>::iterator iterator1;

	if (moduleList.size() > 1)
	{
		iterator1 = moduleList.begin();
		while (iterator1 != moduleList.end())
		{
			if (rva >= iterator1->second.firstThunk)
			{
				iterator1++;
				if (iterator1 == moduleList.end())
				{
					iterator1--;
					module = &(iterator1->second);
					break;
				}
				else if (rva < iterator1->second.firstThunk)
				{
					iterator1--;
					module = &(iterator1->second);
					break;
				}
			}
		}
	}
	else
	{
		iterator1 = moduleList.begin();
		module = &(iterator1->second);
	}

	if (!module)
	{
		Scylla::debugLog.log(L"ImportsHandling::addFunction module not found rva " PRINTF_DWORD_PTR_FULL, rva);
		return false;
	}

	//TODO
	import.suspect = true;
	import.valid = false;
	import.va = va;
	import.rva = rva;
	import.ordinal = ordinal;

	wcscpy_s(import.moduleName, MAX_PATH, moduleName);
	strcpy_s(import.name, MAX_PATH, name);

	module->thunkList.insert(std::pair<DWORD_PTR,ImportThunk>(import.rva, import));

	return true;
}
*/

/*
bool ImportsHandling::addModule(const WCHAR * moduleName, DWORD_PTR firstThunk)
{
	ImportModuleThunk module;

	module.firstThunk = firstThunk;
	wcscpy_s(module.moduleName, MAX_PATH, moduleName);

	moduleList.insert(std::pair<DWORD_PTR,ImportModuleThunk>(firstThunk,module));

	return true;
}
*/

void ImportsHandling::scanAndFixModuleList()
{
	WCHAR prevModuleName[MAX_PATH] = {0};

	auto it_module = moduleList.begin();
	while (it_module != moduleList.end())
	{
		ImportModuleThunk &moduleThunk = it_module->second;

		auto it_import = moduleThunk.thunkList.begin();

		ImportThunk * importThunkPrev;
		importThunkPrev = &it_import->second;

		while (it_import != moduleThunk.thunkList.end())
		{
			ImportThunk &importThunk = it_import->second;

			if (importThunk.moduleName[0] == 0 || importThunk.moduleName[0] == L'?')
			{
				addNotFoundApiToModuleList(&importThunk);
			}
			else 
			{
				
				if (_wcsicmp(importThunk.moduleName, prevModuleName))
				{
					addModuleToModuleList(importThunk.moduleName, importThunk.rva);
				}
				
				addFunctionToModuleList(&importThunk);
			}

			wcscpy_s(prevModuleName, importThunk.moduleName);
			it_import++;
		}

		moduleThunk.thunkList.clear();

		it_module++;
	}

	moduleList = moduleListNew;
	moduleListNew.clear();
}

bool ImportsHandling::addModuleToModuleList(const WCHAR * moduleName, DWORD_PTR firstThunk)
{
	ImportModuleThunk module;

	module.firstThunk = firstThunk;
	wcscpy_s(module.moduleName, moduleName);

	module.key = module.firstThunk;
	moduleListNew[module.key] = module;
	return true;
}

bool ImportsHandling::isNewModule(const WCHAR * moduleName)
{
	auto it_module = moduleListNew.begin();
	while (it_module != moduleListNew.end())
	{
		if (!_wcsicmp(it_module->second.moduleName, moduleName))
		{
			return false;
		}

		it_module++;
	}

	return true;
}

void ImportsHandling::addUnknownModuleToModuleList(DWORD_PTR firstThunk)
{
	ImportModuleThunk module;

	module.firstThunk = firstThunk;
	wcscpy_s(module.moduleName, L"?");

	module.key = module.firstThunk;
	moduleListNew[module.key] = module;
}

bool ImportsHandling::addNotFoundApiToModuleList(const ImportThunk * apiNotFound)
{
	ImportThunk import;
	ImportModuleThunk  * module = 0;
	DWORD_PTR rva = apiNotFound->rva;

	if (moduleListNew.size() > 0)
	{
		auto it_module = moduleListNew.begin();
		while (it_module != moduleListNew.end())
		{
			if (rva >= it_module->second.firstThunk)
			{
				it_module++;
				if (it_module == moduleListNew.end())
				{
					it_module--;
					//new unknown module
					if (it_module->second.moduleName[0] == L'?')
					{
						module = &(it_module->second);
					}
					else
					{
						addUnknownModuleToModuleList(apiNotFound->rva);
						module = &(moduleListNew.find(rva)->second);
					}

					break;
				}
				else if (rva < it_module->second.firstThunk)
				{
					it_module--;
					module = &(it_module->second);
					break;
				}
			}
			else
			{
#ifdef DEBUG_COMMENTS
				Scylla::debugLog.log(L"Error iterator1 != (*moduleThunkList).end()");
#endif
				break;
			}
		}
	}
	else
	{
		//new unknown module
		addUnknownModuleToModuleList(apiNotFound->rva);
		module = &(moduleListNew.find(rva)->second);
	}

	if (!module)
	{
#ifdef DEBUG_COMMENTS
		Scylla::debugLog.log(L"ImportsHandling::addFunction module not found rva " PRINTF_DWORD_PTR_FULL, rva);
#endif
		return false;
	}


	import.suspect = true;
	import.valid = false;
	import.va = apiNotFound->va;
	import.rva = apiNotFound->rva;
	import.apiAddressVA = apiNotFound->apiAddressVA;
	import.ordinal = 0;

	wcscpy_s(import.moduleName, L"?");
	strcpy_s(import.name, "?");

	import.key = import.rva;
	module->thunkList[import.key] = import;
	return true;
}

bool ImportsHandling::addFunctionToModuleList(const ImportThunk * apiFound)
{
	ImportThunk import;
	ImportModuleThunk  * module = 0;
	std::unordered_map<DWORD_PTR, ImportModuleThunk>::iterator it_module;

	if (moduleListNew.size() > 1)
	{
		auto it_module = moduleListNew.begin();
		while (it_module != moduleListNew.end())
		{
			if (apiFound->rva >= it_module->second.firstThunk)
			{
				it_module++;
				if (it_module == moduleListNew.end())
				{
					it_module--;
					module = &(it_module->second);
					break;
				}
				else if (apiFound->rva < it_module->second.firstThunk)
				{
					it_module--;
					module = &(it_module->second);
					break;
				}
			}
			else
			{
#ifdef DEBUG_COMMENTS
				Scylla::debugLog.log(L"Error iterator1 != moduleListNew.end()");
#endif
				break;
			}
		}
	}
	else
	{
		it_module = moduleListNew.begin();
		module = &(it_module->second);
	}

	if (!module)
	{
#ifdef DEBUG_COMMENTS
		Scylla::debugLog.log(L"ImportsHandling::addFunction module not found rva " PRINTF_DWORD_PTR_FULL, apiFound->rva);
#endif
		return false;
	}


	import.suspect = apiFound->suspect;
	import.valid = apiFound->valid;
	import.va = apiFound->va;
	import.rva = apiFound->rva;
	import.apiAddressVA = apiFound->apiAddressVA;
	import.ordinal = apiFound->ordinal;
	import.hint = apiFound->hint;

	wcscpy_s(import.moduleName, apiFound->moduleName);
	strcpy_s(import.name, apiFound->name);

	import.key = import.rva;
	module->thunkList[import.key] = import;
	return true;
}
