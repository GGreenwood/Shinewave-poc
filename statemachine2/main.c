#include <stdio.h>
#include <stdint.h>

#include "shinewave-poc-statemachine.c"

uint8_t state_data[] = {
    // One animation, state, and exit
    1, 1, 1,
    // Animation 0, white to black
    0, 0, 0, 
    255, 255, 255,
    INTER_LERP,
    5,
    LOOP_BOUNCE,
    // Exit 0 is impossible
    0,
    SELECTION_ANY,
    0, 0,
    0,
    DIRECTION_NEUTRAL,
    DIRECTION_NEUTRAL,
    0, 0,
    0, 0,
    0,
    // State 0 just loops forever
    0,
    1,
    // Exit arrays
    0   // State 0 links to exit 0
};

Controller_t controller = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int main(int argc, char *argv[]) {
    Machine state_machine = Machine_deserialize(state_data);

    for(uint8_t i = 0; i < 50; i++) {
        Color cur_color = Machine_color(&state_machine);
        printf("R:%d\tG:%d\tB:%d\n", cur_color.r, cur_color.g, cur_color.b);
        Machine_advance(&state_machine, &controller);
    }
}
