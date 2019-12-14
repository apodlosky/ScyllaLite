#pragma once

#include "PluginLoader.h"
#include "ProcessLister.h"
#include "Logger.h"

class Scylla
{
public:
	static void initAsDll();

	static PluginLoader plugins;

	static ProcessLister processLister;

	static FileLog debugLog;

private:

	static const WCHAR DEBUG_LOG_FILENAME[];
};
