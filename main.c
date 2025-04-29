#include "bmp8.h"
#include <stdio.h>
#include <stdlib.h>

int main() {

    // fonction 2.1 bmp8_LoadImage
    t_bmp8 *img = bmp8_loadImage("barbara_gray.bmp");

    // fonction 2.3 bmp8_free
    bmp8_free(img);

    // fonction 3.1 bmp8_negative
    bmp8_negative(img);

    // fonction 3.3 bmp8_threshold
    bmp8_threshold(img);

    // fonction 4.1
    // bmp8_applyFilter()
    return 0;
}
