#pragma once

#include "Logger.h"

class Scylla
{
public:
	static void initAsDll();

	static FileLog debugLog;

private:

	static const WCHAR DEBUG_LOG_FILENAME[];
};
