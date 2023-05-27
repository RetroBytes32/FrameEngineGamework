#include "engine.h"

ColorPreset       Colors;
RandomGen         Random;
Logger            Log;
Timer             PhysicsTime;
Timer             Time;

ResourceManager   Resources;
PhysicsSystem     Physics;
RenderSystem      Renderer;
AudioSystem       Audio;
ScriptSystem      Scripting;
InputSystem       Input;

ApplicationLayer      Application;
EngineSystemManager   Engine;



EngineSystemManager::EngineSystemManager(void) : 
    
    mSceneMain(nullptr) 
{
    // Preallocate some buffer space
    mGameObjectActive.reserve(64);
    
    return;
}

GameObject* EngineSystemManager::CreateGameObject(void) {
    GameObject* newGameObject = mGameObject.Create();
    AddGameObjectToActiveList(newGameObject);
    return newGameObject;
}

void EngineSystemManager::DestroyGameObject(GameObject* gameObjectPtr) {
    assert(gameObjectPtr != nullptr);
    RemoveGameObjectFromActiveList(gameObjectPtr);
    
    // Remove all components
    for (unsigned int i=0; i < gameObjectPtr->GetComponentCount(); i++) {
        
        Component* componentPtr = gameObjectPtr->GetComponent(i);
        DestroyComponent(componentPtr);
        continue;
    }
    
    mGameObject.Destroy(gameObjectPtr);
    return;
}

GameObject* EngineSystemManager::CreateCameraController(float x, float y, float z) {
    
    GameObject* cameraController = CreateGameObject();
    cameraController->name = "camera";
    
    // Add a camera component
    Component* cameraComponent = CreateComponent(ComponentType::Camera);
    Camera* cameraMain = (Camera*)cameraComponent->GetComponent();
    Renderer.cameraMain = cameraMain;
    cameraMain->EnableMouseLook();
    cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
    // Add a rigid body component
    Component* rigidBodyComponent = CreateComponent(ComponentType::RigidBody);
    rp3d::RigidBody* rigidBody = (rp3d::RigidBody*)rigidBodyComponent->GetComponent();
    
    rp3d::Vector3 position(x, y, z);
    rp3d::Quaternion quat = rp3d::Quaternion::identity();
    
    rp3d::Transform bodyTransform(position, quat);
    rigidBody->setTransform(bodyTransform);
    
    // Add a scripting component
    Component* scriptComponent = CreateComponent(ComponentType::Script);
    Script* script = (Script*)scriptComponent->GetComponent();
    script->gameObject = cameraController;
    script->isActive = true;
    
    cameraController->AddComponent(cameraComponent);
    cameraController->AddComponent(rigidBodyComponent);
    cameraController->AddComponent(scriptComponent);
    
    cameraController->EnableGravity(false);
    cameraController->SetAngularAxisLockFactor(0, 0, 0);
    
    return cameraController;
}

Component* EngineSystemManager::CreateComponentEntityRenderer(Mesh* meshPtr, Material* materialPtr) {
    Component* newComponent = CreateComponent(ComponentType::Renderer);
    Entity* entityRenderer = (Entity*)newComponent->GetComponent();
    entityRenderer->AttachMesh(meshPtr);
    entityRenderer->AttachMaterial(materialPtr);
    
    
    return newComponent;
}

Entity* EngineSystemManager::CreateEntityRenderer(Mesh* meshPtr, Material* materialPtr) {
    Entity* entityPtr = Renderer.CreateEntity();
    entityPtr->AttachMesh( meshPtr );
    entityPtr->AttachMaterial( materialPtr );
    mSceneMain->AddEntityToSceneRoot(entityPtr);
    return entityPtr;
}

void EngineSystemManager::DestroyEntityRenderer(Entity* entityPtr) {
    assert(entityPtr != nullptr);
    mSceneMain->RemoveEntityFromSceneRoot(entityPtr);
    Renderer.DestroyEntity(entityPtr);
    return;
}

void EngineSystemManager::AddGameObjectToActiveList(GameObject* gameObjectPtr) {
    assert(gameObjectPtr != nullptr);
    mGameObjectActive.push_back( gameObjectPtr );
    return;
}

bool EngineSystemManager::RemoveGameObjectFromActiveList(GameObject* gameObjectPtr) {
    assert(gameObjectPtr != nullptr);
    for (std::vector<GameObject*>::iterator it = mGameObjectActive.begin(); it != mGameObjectActive.end(); ++it) {
        GameObject* thisGameObjectPtr = *it;
        if (gameObjectPtr == thisGameObjectPtr) {
            mGameObjectActive.erase(it);
            return true;
        }
    }
    return false;
}

GameObject* EngineSystemManager::GetGameObject(unsigned int index) {
    if (index <= mGameObjectActive.size()) 
        return mGameObjectActive[index];
    return nullptr;
}

unsigned int EngineSystemManager::GetGameObjectCount(void) {
    return mGameObjectActive.size();
}

unsigned int EngineSystemManager::GetComponentCount(void) {
    return mComponents.Size();
}

void EngineSystemManager::Initiate() {
    mSceneMain = Renderer.CreateScene();
    Renderer.AddSceneToRenderQueue(mSceneMain);
    return;
}

Component* EngineSystemManager::CreateComponent(ComponentType type) {
    void* component_object = nullptr;
    
    switch (type) {
        
        case ComponentType::Renderer: {
            component_object = (void*)this->CreateEntityRenderer(nullptr, nullptr);
            break;
        }
        case ComponentType::RigidBody: {
            component_object = (void*)Physics.CreateRigidBody();
            break;
        }
        case ComponentType::Script: {
            component_object = (void*)Scripting.CreateScript();
            break;
        }
        case ComponentType::Camera: {
            component_object = (void*)Renderer.CreateCamera();
            break;
        }
        case ComponentType::Light: {
            Light* lightPtr = Renderer.CreateLight();
            mSceneMain->AddLightToSceneRoot(lightPtr);
            component_object = (void*)lightPtr;
            break;
        }
        
        default:
            break;
    }
    
    Component* newComponent = mComponents.Create();
    newComponent->SetComponent(type, component_object);
    return newComponent;
}

void EngineSystemManager::DestroyComponent(Component* componentPtr) {
    assert(componentPtr != nullptr);
    
    ComponentType componentType = componentPtr->GetType();
    
    switch (componentType) {
        
        case ComponentType::Renderer: {
            Entity* componentEntityRenderer = (Entity*)componentPtr->GetComponent();
            DestroyEntityRenderer(componentEntityRenderer);
            break;
        }
        case ComponentType::RigidBody: {
            rp3d::RigidBody* componentRigidBody = (rp3d::RigidBody*)componentPtr->GetComponent();
            Physics.DestroyRigidBody(componentRigidBody);
            break;
        }
        case ComponentType::Script: {
            Script* componentScript = (Script*)componentPtr->GetComponent();
            Scripting.DestroyScript(componentScript);
            break;
        }
        case ComponentType::Camera: {
            Camera* componentCamera = (Camera*)componentPtr->GetComponent();
            Renderer.DestroyCamera(componentCamera);
            break;
        }
        case ComponentType::Light: {
            Light* componentLight = (Light*)componentPtr->GetComponent();
            mSceneMain->RemoveLightFromSceneRoot(componentLight);
            Renderer.DestroyLight(componentLight);
            break;
        }
        
        default:
            break;
    }
    mComponents.Destroy(componentPtr);
    return;
}

void EngineSystemManager::Update(void) {
    
    // Run through the game objects
    for (int i=0; i < mGameObject.Size(); i++ ) {
        
        GameObject* objectPtr = mGameObject[i];
        
        if (!objectPtr->isActive) 
            continue;
        
        
        // Check to get the rigid body as the source transform
        rp3d::RigidBody* componentRigidBody = objectPtr->GetCachedRigidBody();
        if (componentRigidBody != nullptr) {
            
            // Get the rigid body as the source transformation
            rp3d::Vector3 bodyPosition;
            rp3d::Transform bodyTransform = componentRigidBody->getTransform();
            bodyPosition = bodyTransform.getPosition();
            rp3d::Quaternion quaterion = bodyTransform.getOrientation();
            
            // Source position, rotation and scale
            glm::vec3 position;
            position.x = bodyPosition.x;
            position.y = bodyPosition.y;
            position.z = bodyPosition.z;
            
            glm::vec4 rotation;
            rotation.x = quaterion.x;
            rotation.y = quaterion.y;
            rotation.z = quaterion.z;
            rotation.w = quaterion.w;
            
            glm::vec3 scale = objectPtr->transform.scale;
            
            // Source matrix
            glm::mat4 sourceTransform;
            bodyTransform.getOpenGLMatrix(&sourceTransform[0][0]);
            
            // Update the game object transform
            objectPtr->transform.matrix = sourceTransform;
            objectPtr->transform.position = position;
            objectPtr->transform.rotation = rotation;
            
            
            //
            // Sync with the rigid body
            //
            
            Entity* componentEntityRenderer = objectPtr->GetCachedEntity();
            if (componentEntityRenderer != nullptr) {
                
                componentEntityRenderer->transform.matrix = sourceTransform;
                componentEntityRenderer->transform.position = position;
                componentEntityRenderer->transform.rotation = rotation;
                componentEntityRenderer->transform.scale    = scale;
            }
            
            Light* componentLight = objectPtr->GetCachedLight();
            if (componentLight != nullptr) {
                componentLight->transform.matrix = sourceTransform;
                componentLight->transform.position = position;
                componentLight->transform.rotation = rotation;
            }
            
            Camera* componentCamera = objectPtr->GetCachedCamera();
            if (componentCamera != nullptr) {
                componentCamera->transform.matrix = sourceTransform;
                componentCamera->transform.position = position;
                if (!componentCamera->useMouseLook) 
                    componentCamera->transform.rotation = rotation;
            }
            
        } else {
            
            // Source position, rotation and scale
            glm::vec3 position = objectPtr->transform.position;
            glm::vec4 rotation = objectPtr->transform.rotation;
            glm::vec3 scale    = objectPtr->transform.scale;
            
            //
            // No rigid body, sync with the game object
            //
            
            Entity* componentEntityRenderer = objectPtr->GetCachedEntity();
            if (componentEntityRenderer != nullptr) {
                componentEntityRenderer->transform.position = position;
                componentEntityRenderer->transform.rotation = rotation;
                componentEntityRenderer->transform.scale    = scale;
                componentEntityRenderer->transform.matrix   = Renderer.CalculateModelMatrix(objectPtr->transform);
            }
            
            Light* componentLight = objectPtr->GetCachedLight();
            if (componentLight != nullptr) {
                componentLight->transform.position = position;
                componentLight->transform.rotation = rotation;
            }
            
            Camera* componentCamera = objectPtr->GetCachedCamera();
            if (componentCamera != nullptr) {
                componentCamera->transform.matrix = objectPtr->transform.matrix;
                componentCamera->transform.position = position;
                if (!componentCamera->useMouseLook) 
                    componentCamera->transform.rotation = rotation;
            }
            
        }
        
        continue;
    }
    
    return;
}

void EngineSystemManager::Shutdown(void) {
    
    while (GetGameObjectCount() > 0) {
        DestroyGameObject( GetGameObject(0) );
    }
    
    assert(GetGameObjectCount() == 0);
    assert(mComponents.Size() == 0);
    
    return;
}
