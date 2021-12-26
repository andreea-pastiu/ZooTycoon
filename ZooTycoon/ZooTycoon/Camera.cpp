#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
        this->cameraFrontDirection = cameraTarget -cameraPosition; //xtarget-xposition si y
        this->cameraRightDirection = glm::cross(cameraFrontDirection, cameraUpDirection);
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        if (direction == MOVE_FORWARD)
        {
            cameraPosition += cameraFrontDirection * speed;
            //cameraTarget += cameraFrontDirection * speed;
        }
        else
        {
            if (direction == MOVE_BACKWARD)
            {
                cameraPosition -= cameraFrontDirection * speed;
                //cameraTarget -= cameraFrontDirection * speed;
            }
            else
            {
                if (direction == MOVE_RIGHT)
                {
                    //cameraPosition += cameraRightDirection * speed;
                    //cameraTarget += cameraRightDirection * speed;
                    cameraPosition += glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection)) * speed;
                }
                else
                {
                    //cameraPosition -= cameraRightDirection * speed;
                    //cameraTarget -= cameraRightDirection * speed;
                    cameraPosition -= glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection)) * speed;
                }
            }
        }
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        glm::vec3 direction;
        direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        direction.y = sin(glm::radians(pitch));
        direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        cameraFrontDirection = glm::normalize(direction);
    }
}