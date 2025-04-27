
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

void  bmp8_saveImage( char filename, *t_bmp8 image);
void bmp8_printInfo( t_bmp8* image );
void bmp_brightness(t_bmp8* image, int value);
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize);