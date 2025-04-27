#include <stdio.h>
#include <stdlib.h>


typedef struct {
    unsigned char header[54]; /* En tête */
    unsigned char colorTable[1024]; /* table de couleur */
    unsigned char * data; /*données de l'image sous forme de tableau*/
    unsigned int width; /* largeur de l'image en pixels (offset 18) */
    unsigned int height; /* hauteur de l'image en pixels (offset 22)*/
    unsigned int colorDepth ; /* profondeur de couleur de l'image (8)*/
    unsigned int dataSize; /* taille des données de l'image en octets (offset 28 )*/

} t_bmp8;

t_bmp8 * bmp8_loadImage( const char * filename) ;

void bmp8_free(t_bmp8 * img);

void bmp8_negative(t_bmp8 * img);

void bmp8_threshold(t_bmp8 * img);

void bmp8_applyFilter(t_bmp8 * img, float ** kernel, int kernelSize);