#ifndef		__MT_PLATFORM_H
#define		__MT_PLATFORM_H

/// win
#include <Winsock2.h>
#include <mswsock.h>
#pragma comment (lib, "WS2_32")
#pragma comment (lib, "Mswsock")
#pragma warning ( disable:4200 )

/// 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>

/// standard C++ 
#include <iostream>
using namespace std;


/// user 
#include "mtDefine.h"
#include "mtException.h"
#include "mtGlobal.h"
#endif	/// __MT_PLATFORM_H