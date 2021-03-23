#include "log.h"
#include "player/player.h"

int main(int argc, char* argv[]) {
    custom::player player;

    try {
        player = custom::player(argc, argv);
    } catch(const std::exception& ex) {
        log::error() << ex.what() << '\n';
        return 1;
    }

    return 0;
}