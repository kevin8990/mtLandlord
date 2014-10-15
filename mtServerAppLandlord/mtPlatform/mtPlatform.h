#ifndef		__MT_PLATFORM_H
#define		__MT_PLATFORM_H

/// Windows API
#include <windows.h>
#include <process.h>
#include <sqltypes.h>
#include <sqlext.h>
#include <direct.h>


/// POSIX Standard API
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

/// C++ API

#include <iostream>
#include <typeinfo>
using namespace std;

/// C++ STL 
#include <queue>
#include <deque>

/// USER 
#include "mtPragma.h"
#include "mtDefine.h"
#include "mtEnum.h"
#include "mtTypedef.h"
#include "mtFunction.h"
#include "mtGlobal.h"
#include <time.h>
#endif	/// __MT_PLATFORM_H
