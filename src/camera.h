
#include <vk_types.h>
#include <SDL_events.h>

class Camera {
public:
    glm::vec3 velocity {0.f};
    glm::vec3 position {0, 0, 5};

    // vertical rotation
    float pitch {0.0f};
    // horizontal rotation
    float yaw {0.0f};

    glm::mat4 get_view_matrix();
    glm::mat4 get_rotation_matrix();

    void processSDLEvent(SDL_Event& e);

    void update();
};
