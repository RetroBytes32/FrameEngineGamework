#include <GameEngineFramework/Renderer/components/light.h>


Light::Light() : 
    
    isActive(true),
    doCastShadow(true),
    doHaloEffect(true),
    
    type(0),
    
    renderDistance(300),
    
    position(glm::vec3(0)),
    offset(glm::vec3(0)),
    direction(glm::vec3(0)),
    
    intensity(100),
    range(300),
    attenuation(0.008)
{
    color = Color(1, 1, 1);
}

