
#include <raylib/raylib.h>


class Battalion {

public:
    Battalion(Vector2 initialPosition);
    ~Battalion();
    void draw();
    void setColor(Color color);

private:
    Vector2 m_position;
    Color m_color;

};
