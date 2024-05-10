#ifndef Camera_hpp
#define Camera_hpp

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include <string>

namespace gps {

    enum MOVE_DIRECTION { MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT, MOVE_UP, MOVE_DOWN };

    class Camera
    {
    public:
        // Camera constructor
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);
        // Return the view matrix, using the glm::lookAt() function
        glm::mat4 getViewMatrix();
        // Update the camera internal parameters following a camera move event
        void move(MOVE_DIRECTION direction, float speed);
        // Update the camera internal parameters following a camera rotate event
        // yaw - camera rotation around the y axis
        // pitch - camera rotation around the x axis
        void rotate(float pitch, float yaw);
        // Get the distance from the camera to a point
        float getDistance(glm::vec3 point);
        // Set the camera position, target, and up vector
        void set(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);
        // Print camera information (for debugging)
        void print();

        // Public members for direct access
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraFrontDirection;
        glm::vec3 cameraRightDirection;
        glm::vec3 cameraUpDirection;
    };

}

#endif /* Camera_hpp */
