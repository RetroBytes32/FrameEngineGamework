#include <GameEngineFramework/Engine/Engine.h>

extern EngineComponents     Components;
extern ColorPreset          Colors;
extern NumberGeneration     Random;
extern Logger               Log;
extern Timer                PhysicsTime;
extern Timer                Time;

extern EngineSystemManager  Engine;
extern PlatformLayer        Platform;
extern ResourceManager      Resources;
extern Serialization        Serializer;
extern ScriptSystem         Scripting;
extern RenderSystem         Renderer;
extern PhysicsSystem        Physics;
extern InputSystem          Input;
extern MathCore             Math;
extern ActorSystem          AI;



class ENGINE_API WeatherSystem {
    
public:
    
    /// Sun
    GameObject* sunObject;
    Light* sunLight;
    
    /// Sky
    GameObject* skyObject;
    Material* skyMaterial;
    
    /// Day/night cycle
    
    
    
    void Initiate(void) {
        
        //
        // Sun
        //
        
        sunObject = Engine.Create<GameObject>();
        sunObject->AddComponent( Engine.CreateComponent<Light>() );
        
        Transform* transform = sunObject->GetComponent<Transform>();
        transform->RotateEuler(0.0f, -1.0f, 2.0f);
        
        sunLight = sunObject->GetComponent<Light>();
        
        sunLight->type       = LIGHT_TYPE_DIRECTIONAL;
        sunLight->intensity  = 0.7f;
        sunLight->color      = Colors.white;
        
        Engine.sceneMain->AddLightToSceneRoot( sunLight );
        
        //
        // Sky
        //
        
        // Amount of fade bias from the color "skyHigh" to "skyLow".
        float colorBias = 1.0f;
        
        // Sky mesh resource name.
        // Note: this mesh is loaded by the resource manager.
        std::string skyResourceName = "sky";
        
        // Generate the sky which will be returned as a game object.
        // This game object will contain a mesh renderer to draw the sky.
        skyObject = Engine.CreateSky(skyResourceName, Colors.blue, Colors.blue, colorBias);
        
        // Add the sky's mesh renderer to the main scene.
        Engine.sceneMain->AddMeshRendererToSceneRoot( skyObject->GetComponent<MeshRenderer>(), RENDER_QUEUE_SKY );
        
        // Sky rendering colors
        MeshRenderer* skyRenderer = skyObject->GetComponent<MeshRenderer>();
        skyMaterial = skyRenderer->material;
        skyMaterial->diffuse = Colors.dkgray;
        skyMaterial->ambient = Colors.white;
        
        return;
    }
    
    
    void Update(void) {
        
        return;
        
        GameObject* weatherObject = Engine.Create<GameObject>();
        Transform* transform = weatherObject->GetComponent<Transform>();
        
        glm::vec3 playerPosition = AI.GetPlayerWorldPosition();
        
        float posx = playerPosition.x;
        float posy = playerPosition.y;
        float posz = playerPosition.z;
        
        posx += Random.Range(0, 10) - Random.Range(0, 10);
        posy += Random.Range(0, 10) - Random.Range(0, 10);
        posz += Random.Range(0, 10) - Random.Range(0, 10);
        
        transform->position = glm::vec3(posx, posy, posz);
        
        Material* weatherMaterial = Engine.Create<Material>();
        
        weatherMaterial->shader = Engine.shaders.color;
        
        weatherObject->AddComponent( Engine.CreateComponentMeshRenderer( Engine.meshes.cube, weatherMaterial ) );
        MeshRenderer* renderer = weatherObject->GetComponent<MeshRenderer>();
        
        Engine.sceneMain->AddMeshRendererToSceneRoot( renderer );
        
        return;
    }
    
    
    
    
    
    
    
};
