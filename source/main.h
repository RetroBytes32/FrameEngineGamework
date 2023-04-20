//#define _WIN32_WINNT  0x500
#include <SDKDDKVer.h>
#include <windows.h>

// Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <vector>
#include <thread>
#include <map>


#include "configuration.h"
#include "plugins.h"

//
// My standard includes
#include "std/poolallocator.h"
#include "std/strings.h"
#include "std/types.h"
#include "std/random.h"
#include "std/logging.h"
#include "std/fileloader.h"


//
// Singleton access points

#include "std/timer.h"
Timer      PhysicsTime;
Timer      RenderTime;

Timer      Time;

Logger     Log;
RandomGen  Random;

#include "Input/InputSystem.h"
InputSystem   Input;

#include "Physics/PhysicsSystem.h"
PhysicsSystem Physics;

#include "Renderer/RenderSystem.h"
RenderSystem  Renderer;
ColorPreset   Colors;

#include "resources/ResourceManager.h"
ResourceManager Resources;


bool isPaused = false;
bool isActive = true;
HWND wHnd = NULL;


#include "application/application.h"

#include "winproc.h"

