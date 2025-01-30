#include "../low-engine/LowEngine.h"

int main() {
    LowEngine engine;
    engine.OpenWindow("Lords of war", 800, 600);
    while (engine.WindowIsOpen()) {

    }
    return 0;
}
