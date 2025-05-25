#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>


// Structures

typedef struct {
unsigned char header[54]; /* En tête */
unsigned char colorTable[1024]; /* table de couleur */
unsigned char * data; /*données de l'image sous forme de tableau*/
unsigned int width; /* largeur de l'image en pixels (offset 18) */
unsigned int height; /* hauteur de l'image en pixels (offset 22)*/
unsigned int colorDepth ; /* profondeur de couleur de l'image (8)*/
unsigned int dataSize; /* taille des données de l'image en octets (offset 28 )*/

} t_bmp8;


// prototypes-------------------------------------------------------------
//menu
void menu_partie1_et_3_gris();
void menu_partie2_et_3_couleur();

// Partie 1 Image BMP 8 bits
//Fonction qui permet de lire une image BMP en niveaux de gris
t_bmp8 * bmp8_loadImage( const char * filename) ;

// Fonction qui permet de libérer la mémoire allouée pour stocker une image de type t_bmp8
void bmp8_free(t_bmp8 * img);

// Fonction qui permet d'inverser les couleurs d'une image en niveaux de gris
void bmp8_negative(t_bmp8 * img);

// Fonction qui permet de transformer une image en niveaux de gris en une image binaire
void bmp8_threshold(t_bmp8 * img);

int bmp8_saveImage( const char* filename, t_bmp8 * image);
void bmp8_printInfo(t_bmp8 *image);
void bmp8_brightness(t_bmp8* image, int value);
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize);

//Partie 3 Histogramme et égalisation-----------------------------------------------
unsigned int * bmp8_computeHistogram(t_bmp8 * img);
unsigned int * bmp8_computeCDF(unsigned int * hist);
void bmp8_equalize(t_bmp8 * img, unsigned int * hist_eq);
void bmp24_equalize(t_bmp24 * img);
