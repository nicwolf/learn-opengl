#ifndef CAMERA_H
#define CAMERA_H

// Standard Includes
#include <vector>

// OpenGL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Abstract Camera Movements
enum CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default Camera Values
const GLfloat DEFAULT_YAW               = -90.0f;
const GLfloat DEFAULT_PITCH             =   0.0f;
const GLfloat DEFAULT_MOVEMENT_SPEED    =   3.0f;
const GLfloat DEFAULT_MOUSE_SENSITIVITY =  0.25f;
const GLfloat DEFAULT_FOV               =  45.0f;

// Camera Class
class Camera
{
public:
    // Camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    GLfloat yaw, pitch;

    // Camera Options
    GLfloat movementSpeed, mouseSensitivity, fov;

    // Constructors
    Camera(glm::vec3 position=glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up=glm::vec3(0.0f, 1.0f, 0.0f),
           GLfloat yaw=DEFAULT_YAW, GLfloat pitch=DEFAULT_PITCH);

    Camera(GLfloat posX, GLfloat posY, GLfloat posZ,
           GLfloat upX, GLfloat upY, GLfloat upZ,
           GLfloat yaw, GLfloat pitch);

    // Methods
    glm::mat4 getViewMatrix();
    void processKeyboard(CameraMovement direction, GLfloat deltaTime);
    void processMouseMovement(GLfloat xOff, GLfloat yOff,
                              GLboolean constrainPitch=true);
    void processMouseScroll(GLfloat yOff);

private:
    void updateCameraVectors();
};

#endif // CAMERA_H
