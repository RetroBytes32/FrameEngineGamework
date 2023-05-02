#include "gameobject.h"

GameObject::GameObject():
name(""),
isActive(true),
entity(nullptr),
script(nullptr),
camera(nullptr),
rigidBody(nullptr) {
    return;
}




void GameObject::AttachEntity(Entity* entityPtr) {
    entity = entityPtr;
    return;
}

void GameObject::DetachEntity(void) {
    entity = nullptr;
    return;
}

Entity* GameObject::GetAttachedEntity(void) {
    return entity;
}


void GameObject::AttachScript(Script* scriptPtr) {
    script = scriptPtr;
    scriptPtr->isActive = true;
    return;
}

void GameObject::DetachScript(void) {
    script = nullptr;
    script->isActive = false;
    return;
}

Script* GameObject::GetAttachedScript(void) {
    return script;
}


void GameObject::AttachRidigBody(rp3d::RigidBody* rigidBodyPtr){
    rigidBody = rigidBodyPtr;
    return;
}

void GameObject::DetachRidigBody(void) {
    rigidBody = nullptr;
    return;
}

rp3d::RigidBody* GameObject::GetAttachedRidigBody(void) {
    return rigidBody;
}



void GameObject::AttachCamera(Camera* cameraPtr) {
    camera = cameraPtr;
    return;
}

void GameObject::DetachCamera(void) {
    camera = nullptr;
    return;
}

Camera* GameObject::GetAttachedCamera(void) {
    return camera;
}








//
// Physics functions
//

void GameObject::AddForce(float x, float y, float z) {
    assert(rigidBody != nullptr);
    rp3d::Vector3 nudge(x, y, z);
    rigidBody->applyLocalForceAtCenterOfMass(nudge);
    return;
}

void GameObject::AddTorque(float x, float y, float z) {
    assert(rigidBody != nullptr);
    rp3d::Vector3 twist(x, y, z);
    rigidBody->applyLocalTorque(twist);
    return;
}

void GameObject::SetMass(float mass) {
    assert(rigidBody != nullptr);
    rigidBody->setMass(mass);
}

void GameObject::SetLinearDamping(float damping) {
    assert(rigidBody != nullptr);
    rigidBody->setLinearDamping(damping);
}

void GameObject::SetAngularDamping(float damping) {
    assert(rigidBody != nullptr);
    rigidBody->setAngularDamping(damping);
}

void GameObject::EnableGravity(bool enabled) {
    assert(rigidBody != nullptr);
    rigidBody->enableGravity(enabled);
}


void GameObject::CalculatePhysics(void) {
    assert(rigidBody != nullptr);
    rigidBody->updateMassFromColliders();
    rigidBody->updateLocalCenterOfMassFromColliders();
    rigidBody->updateLocalInertiaTensorFromColliders();
    return;
}


void GameObject::SetLinearAxisLockFactor(float x, float y, float z) {
    assert(rigidBody != nullptr);
    rp3d::Vector3 lockFactor(x, y, z);
    rigidBody->setLinearLockAxisFactor(lockFactor);
    return;
}

void GameObject::SetAngularAxisLockFactor(float x, float y, float z) {
    assert(rigidBody != nullptr);
    rp3d::Vector3 lockFactor(x, y, z);
    rigidBody->setAngularLockAxisFactor(lockFactor);
    return;
}


void GameObject::AddColliderBox(rp3d::BoxShape* boxShape, float x, float y, float z) {
    assert(rigidBody != nullptr);
    assert(boxShape != nullptr);
    
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    
    rigidBody->addCollider(boxShape, offsetTransform);
    
    return;
}

void GameObject::AddCollider(ColliderTag* colliderTag, float x, float y, float z) {
    assert(rigidBody != nullptr);
    assert(colliderTag != nullptr);
    if (colliderTag->isStatic) {
        rigidBody->setType(rp3d::BodyType::STATIC);
    } else {
        rigidBody->setType(rp3d::BodyType::DYNAMIC);
    }
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    
    rigidBody->addCollider(colliderTag->colliderShape, offsetTransform);
    
    return;
}


void GameObject::SetRigidBodyStatic(void) {
    assert(rigidBody != nullptr);
    rigidBody->setType(rp3d::BodyType::STATIC);
    return;
}


void GameObject::SetRigidBodyDynamic(void) {
    assert(rigidBody != nullptr);
    rigidBody->setType(rp3d::BodyType::DYNAMIC);
    return;
}


