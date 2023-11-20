#include <SDKDDKVer.h>
#include <windows.h>

#include <thread>
#include <mutex>

#include "../configuration.h"

#ifdef RUN_UNIT_TESTS
 #include "../../tests/framework.h"
#endif


// Code base library
#include "../../dependencies/CodeBaseLibrary/poolallocator.h"
#include "../../dependencies/CodeBaseLibrary/strings.h"
#include "../../dependencies/CodeBaseLibrary/types.h"
#include "../../dependencies/CodeBaseLibrary/random.h"
#include "../../dependencies/CodeBaseLibrary/logging.h"
#include "../../dependencies/CodeBaseLibrary/fileloader.h"
#include "../../dependencies/CodeBaseLibrary/timer.h"

#include "../Input/InputSystem.h"
#include "../Physics/PhysicsSystem.h"
#include "../Renderer/RenderSystem.h"
#include "../Resources/ResourceManager.h"
#include "../Scripting/ScriptSystem.h"
#include "../Serialization/Serialization.h"
#include "../AI/ActorSystem.h"

#include "../application/applicationlayer.h"

#include "../engine/engine.h"



#include <string>
