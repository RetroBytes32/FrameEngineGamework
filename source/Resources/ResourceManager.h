#ifndef RESOURCE_ALLOCATION_SYSTEM
#define RESOURCE_ALLOCATION_SYSTEM

#include "../Renderer/RenderSystem.h"
#include "../Physics/PhysicsSystem.h"

#include "assets/meshTag.h"
#include "assets/textureTag.h"
#include "assets/shaderTag.h"
#include "assets/colliderTag.h"

#include "../../dependencies/CodeBaseLibrary/fileloader.h"
#include "../../dependencies/CodeBaseLibrary/logging.h"

#include "../../dependencies/CodeBaseLibrary/directorysearch.h"
#include "../../dependencies/CodeBaseLibrary/poolallocator.h"
#include "../../dependencies/CodeBaseLibrary/strings.h"


class __declspec(dllexport) ResourceManager {
    
public:
    
    /// Main rendering scene.
    Scene* sceneMain;
    
    /// Prepare the loading system.
    void Initiate(void);
    
    /// Load a wavefront model file and assign it a resource tag name.
    bool LoadWaveFront(std::string path, std::string resourceName);
    /// Load a texture image file and assign it a resource tag name.
    bool LoadTexture(std::string path, std::string resourceName);
    /// Load a GLSL shader file and assign it a resource tag name.
    bool LoadShaderGLSL(std::string path, std::string resourceName);
    
    /// Find a mesh resource by its resource tag name.
    MeshTag*     FindMeshTag(std::string resourceName);
    /// Find a texture resource by its resource tag name.
    TextureTag*  FindTextureTag(std::string resourceName);
    /// Find a shader resource by its resource tag name.
    ShaderTag*   FindShaderTag(std::string resourceName);
    /// Find a resource by its resource tag name.
    ColliderTag* FindColliderTag(std::string resourceName);
    
    /// Destroy and deallocate a mesh model resource by its name.
    bool UnloadMeshTag(std::string resourceName);
    /// Destroy and deallocate a texture image resource by its name.
    bool UnloadTextureTag(std::string resourceName);
    /// Destroy and deallocate a shader program resource by its name.
    bool UnloadShaderTag(std::string resourceName);
    
    /// Create a render mesh object from a mesh resource tag.
    Mesh*     CreateMeshFromTag(std::string resourceName);
    /// Create a material object from a texture image resource tag.
    Material* CreateMaterialFromTag(std::string resourceName);
    /// Create a shader object from a GLSL shader resource tag.
    Shader*   CreateShaderFromTag(std::string resourceName);
    /// Create a physics collision shape from a collider resource tag.
    rp3d::BoxShape* CreateColliderFromTag(std::string resourceName);
    
    /// Purge all assets loaded in the resource system.
    void DestroyAssets(void);
    
    ResourceManager();
    
private:
    
    // Thread loading queue
    std::vector<std::string> streamQueue;
    
    // Asset lists
    std::vector<MeshTag>      mMeshTags;
    std::vector<TextureTag>   mTextureTags;
    std::vector<ShaderTag>    mShaderTags;
    std::vector<ColliderTag>  mColliderTags;
    
    // Raw image data loader
    unsigned char* LoadImageRaw(char const* path, int* width, int* height, int* channels, int req_channels);
    
};



#endif
