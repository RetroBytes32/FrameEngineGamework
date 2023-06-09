#include "camera.h"

Camera::Camera() : 
    
    up(glm::vec3(0, 1, 0)),
    forward(glm::vec3(0, 0, 0)),
    right(glm::vec3(0, 0, 0)),
    
    useMouseLook(false),
    
    fov(60),
    aspect(1.33),
    clipNear(0.1),
    clipFar(1000000),
    
    MouseLimitPitchMax(180),
    MouseLimitPitchMin(180),
    
    MouseSensitivityYaw(0.004),
    MouseSensitivityPitch(0.0038)
{
}

void Camera::SetMouseCenter(int mouseResetX, int mouseResetY) {CursorSetPosition(mouseResetX, mouseResetY);}
void Camera::EnableMouseLook(void)  {ShowCursor(false); useMouseLook = true;}
void Camera::DisableMouseLook(void) {ShowCursor(true); useMouseLook = false;}

void Camera::MouseLook(float deltaTime, int mouseResetX, int mouseResetY) {
    
    glm::vec2 MousePos = CursorGetPosition();
    CursorSetPosition(mouseResetX, mouseResetY);
    
    float MouseDiffX = (MousePos.x - mouseResetX) * MouseSensitivityYaw * deltaTime;
    float MouseDiffY = (MousePos.y - mouseResetY) * MouseSensitivityPitch * deltaTime;
    
    transform.orientation.x += (float)MouseDiffX;
    transform.orientation.y -= (float)MouseDiffY;
    
    // Yaw limit
    if (transform.orientation.x >= 0.109655)  {transform.orientation.x -= 0.109655;}
    if (transform.orientation.x <= 0.109655)  {transform.orientation.x += 0.109655;}
    
    // Pitch limit
    if (transform.orientation.y >  0.0274f) transform.orientation.y =  0.0274f;
    if (transform.orientation.y < -0.0274f) transform.orientation.y = -0.0274f;
    
    return;
}

float Camera::GetPitch(void) {
    return glm::degrees(glm::asin(transform.orientation.y));
}

float Camera::GetYaw(float pitch) {
    return glm::degrees(glm::acos(transform.orientation.x / cos(glm::radians(pitch))));
}
