#ifndef _PHYSICS_WRAP_
#define _PHYSICS_WRAP_

#include <GameEngineFramework/MemoryAllocation/PoolAllocator.h>

#include <GameEngineFramework/Physics/components/meshcollider.h>
#include <GameEngineFramework/Renderer/components/mesh.h>

#include "../../../vendor/ReactPhysics3d/ReactPhysics3d.h"

#include <cstdlib>

typedef rp3d::RigidBody    RigidBody;
typedef rp3d::BoxShape     BoxShape;
typedef rp3d::SphereShape  SphereShape;
typedef rp3d::CapsuleShape CapsuleShape;

// Collision type layer mask
enum class LayerMask {
    Default = 0x0001,
    Ground  = 0x0002,
    Object  = 0x0003,
    Actor   = 0x0004,
    Player  = 0x0005
};


// Solid collision mask
enum class CollisionMask {
    Default = 0x0001,
    Ground  = 0x0002,
    Entity  = 0x0003,
};


class RaybackCastCaller : public rp3d::RaycastCallback {
    
public:
    
    bool isHit;
    
    glm::vec3 point;
    
    glm::vec3 normal;
    
    rp3d::CollisionBody* body;
    
    rp3d::Collider* collider;
    
    void* userData;
    
    RaybackCastCaller();
    
    virtual rp3d::decimal notifyRaycastHit( const rp3d::RaycastInfo& info ) {
        
        isHit = true;
        
        point.x = info.worldPoint.x;
        point.y = info.worldPoint.y;
        point.z = info.worldPoint.z;
        
        normal.x = info.worldNormal.x;
        normal.y = info.worldNormal.y;
        normal.z = info.worldNormal.z;
        
        body = info.body;
        collider = info.collider;
        
        userData = collider->getUserData();
        
        // Return a fraction of 1.0 to gather all hits
        return rp3d::decimal (1.0) ;
    }
    
};



class ENGINE_API Hit {
    
public:
    
    glm::vec3 point;
    
    glm::vec3 normal;
    
    void* gameObject;
    
};




class ENGINE_API PhysicsSystem {
    
public:
    
    /// Physical world simulation.
    rp3d::PhysicsWorld* world;
    
    /// Physics library common access singleton.
    rp3d::PhysicsCommon common;
    
    PhysicsSystem();
    
    /// Prepare the physics system.
    void Initiate(void);
    
    /// Prepare the physics system.
    void Shutdown(void);
    
    /// Create a rigid body object and return its pointer.
    rp3d::RigidBody* CreateRigidBody(float x=0.0, float y=0.0, float z=0.0);
    /// Destroy a rigid body object.
    bool DestroyRigidBody(rp3d::RigidBody* rigidBodyPtr);
    
    /// Set the gravity for the world simulation.
    void SetWorldGravity(float x, float y, float z);
    
    /// Create a box collider scaled by x, y, z.
    rp3d::BoxShape* CreateColliderBox(float xscale, float yscale, float zscale);
    /// Create a sphere collider scaled by radius.
    rp3d::SphereShape* CreateColliderSphere(float radius);
    /// Create a capsule collider scaled by radius and height.
    rp3d::CapsuleShape* CreateColliderCapsule(float radius, float height);
    
    /// Create a height field collider from a height field map and return its pointer.
    MeshCollider* CreateHeightFieldMap(float* heightField, unsigned int width, unsigned int height, float scaleX=1, float scaleY=1, float scaleZ=1);
    
    /// Destroy a height field map collider.
    bool DestroyHeightFieldMap(MeshCollider* collider);
    
    /// Cast a ray and return the hit data of any objects that intersected the ray.
    bool Raycast(glm::vec3 from, glm::vec3 direction, float distance, Hit& hit, LayerMask layer=LayerMask::Default);
    
    
private:
    
    // Add a rigid body to the free list
    void AddRigidBodyToFreeList(rp3d::RigidBody* rigidBodyPtr);
    // Remove a rigid body from the free list
    rp3d::RigidBody* RemoveRigidBodyFromFreeList(void);
    
    // Free list of rigid bodies
    std::vector<rp3d::RigidBody*> mRigidBodyFreeList;
    
    // Callback function for physics ray casting
    RaybackCastCaller mRaybackCastCaller;
    
    // Allocator of mesh colliders
    PoolAllocator<MeshCollider> mMeshColliders;
    
};


#endif
