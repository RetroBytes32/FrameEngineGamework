#include <GameEngineFramework/Resources/assets/shaderTag.h>
#include <GameEngineFramework/Resources/FileLoader.h>
#include <GameEngineFramework/Logging/Logging.h>

extern Logger Log;

ShaderTag::ShaderTag() : 
    
    path(""),
    name(""),
    isLoaded(false),
    
    vertexScript(""),
    fragmentScript("")
{
}

bool ShaderTag::Load(void) {
    
    FileLoader loader(path);
    if (!loader.CheckIsFileLoaded()) {
        std::string logstr = "! " + path;
        Log.Write(logstr);
        return false;
    }
    
    if (isLoaded) 
        Unload();
    
    vertexScript   = loader.GetDataBlockByName("vertex");
    fragmentScript = loader.GetDataBlockByName("fragment");
    
    isLoaded = true;
    return true;
}

bool ShaderTag::Unload(void) {
    
    vertexScript = "";
    fragmentScript = "";
    
    return true;
}
