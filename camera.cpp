#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up,
               GLfloat yaw, GLfloat pitch)
               : front(glm::vec3(0.0f, 0.0f, -1.0f))
               , movementSpeed(DEFAULT_MOVEMENT_SPEED)
               , mouseSensitivity(DEFAULT_MOUSE_SENSITIVITY)
               , fov(DEFAULT_FOV)
{
    this->position = position;
    this->worldUp  = up;
    this->yaw      = yaw;
    this->pitch    = pitch;
    this->updateCameraVectors();
}

Camera::Camera(GLfloat posX, GLfloat posY, GLfloat posZ,
               GLfloat upX, GLfloat upY, GLfloat upZ,
               GLfloat yaw, GLfloat pitch)
               : front(glm::vec3(0.0f, 0.0f, -1.0f))
               , movementSpeed(DEFAULT_MOVEMENT_SPEED)
               , mouseSensitivity(DEFAULT_MOUSE_SENSITIVITY)
               , fov(DEFAULT_FOV)
{
    this->position = glm::vec3(posX, posY, posZ);
    this->worldUp  = glm::vec3( upX,  upY,  upZ);
    this->yaw      = yaw;
    this->pitch    = pitch;
    this->updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(this->position, this->position + this->front, this->up);
}

void Camera::processKeyboard(CameraMovement direction, GLfloat deltaTime)
{
    GLfloat velocity = this->movementSpeed * deltaTime;
    if (direction == FORWARD) {
        this->position += this->front * velocity;
    }
    else if (direction == BACKWARD) {
        this->position -= this->front * velocity;
    }
    else if (direction == LEFT) {
        this->position -= this->right * velocity;
    }
    else if (direction == RIGHT) {
        this->position += this->right * velocity;
    }
}

void Camera::processMouseMovement(GLfloat xOff, GLfloat yOff,
                                  GLboolean constrainPitch)
{
    xOff *= this->mouseSensitivity;
    yOff *= this->mouseSensitivity;

    this->yaw   += xOff;
    this->pitch += yOff;

    if (constrainPitch) {
        if (this->pitch > 89.0f) {
            this->pitch = 89.0f;
        }
        else if (this->pitch < -89.0f) {
            this->pitch = -89.0f;
        }
    }

    this->updateCameraVectors();
}

void Camera::processMouseScroll(GLfloat yOff)
{
    if (this->fov >=1.0f && this->fov <= 45.0f) {
        this->fov -= yOff;
    }
    if (this->fov <= 1.0f) {
        this->fov = 1.0f;
    }
    if (this->fov >= 45.0f) {
        this->fov = 45.0f;
    }
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(this->pitch)) * cos(glm::radians(this->yaw));
    front.y = sin(glm::radians(this->pitch));
    front.z = cos(glm::radians(this->pitch)) * sin(glm::radians(this->yaw));
    this->front = glm::normalize(front);
    this->right = glm::normalize(glm::cross(this->front, this->worldUp));
    this->up    = glm::normalize(glm::cross(this->right, this->front));
}
