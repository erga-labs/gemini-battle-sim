
#include <raylib/raylib.h>


class Battalion {

public:
    Battalion(Vector2 initialPosition);
    ~Battalion();
    void draw();

private:
    Vector2 m_position;

};
