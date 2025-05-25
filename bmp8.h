//
// auteur : mariette
// fichier qui regroupe les fonctions relatives au traitement d'image en niveaux de gris format BMP 8 bits
// fonctions de la partie 1 du projet
// fichier header bmp8.h : prototypes des fonctions
// + définition du type t_bmp8
//
#ifndef BMP8H
#define BMP8_H


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>


typedef struct {
    unsigned char header[54]; /* En tête */
    unsigned char colorTable[1024]; /* table de couleur */
    unsigned char * data; /*données de l'image sous forme de tableau*/
    unsigned int width; /* largeur de l'image en pixels (offset 18) */
    unsigned int height; /* hauteur de l'image en pixels (offset 22)*/
    unsigned int colorDepth ; /* profondeur de couleur de l'image (8)*/
    unsigned int dataSize; /* taille des données de l'image en octets (offset 28 )*/

} t_bmp8;

//Fonction qui permet de lire une image BMP en niveaux de gris
t_bmp8 * bmp8_loadImage( const char * filename) ;

// Fonction qui permet de libérer la mémoire allouée pour stocker une image de type t_bmp8
void bmp8_free(t_bmp8 * img);

// Fonction qui permet d'inverser les couleurs d'une image en niveaux de gris
void bmp8_negative(t_bmp8 * img);

// Fonction qui permet de transformer une image en niveaux de gris en une image binaire
void bmp8_threshold(t_bmp8 * img);

// Fonction qui applique un filtre sur une image en niveaux de gris
void bmp8_applyFilter(t_bmp8 * img, float ** kernel, int kernelSize);



#endif