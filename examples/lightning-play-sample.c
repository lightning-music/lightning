#include "lightning.h"

#include <stdio.h>
#include <unistd.h>

int
main(int argc, char **argv)
{
    Lightning l = Lightning_init();
    Lightning_connect_to(l, "system:playback_1", "system:playback_2");
    printf("playing %s\n", argv[1]);
    Lightning_play_sample(l, argv[1], 1.0, 1.0);
    sleep(4);
    Lightning_free(&l);
    return 0;
}
