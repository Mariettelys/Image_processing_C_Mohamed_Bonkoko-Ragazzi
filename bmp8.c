#include "bmp8.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

t_bmp8 * bmp8_loadImage(const char * filename){
  // Ouvrir le fichier en mode binaire lecture ("rb")
  FILE *file = fopen(filename, "rb" );

  // Vérifies si l'ouverture du fichier a réussi
  if (file == NULL){
    printf("Erreur : ouverture du fichier %s impossible \n", filename);
    return NULL;
  }

  // Alocation de la mémoire pour une image de type t_bmp8
  t_bmp8 *img = (t_bmp8 *)malloc(sizeof(t_bmp8));
  // Vérification allocation de la mémoire réussie
  if (img==NULL){
    printf("Erreur : mémoire insuffisante pour la création de l'image \n");
    fclose(file);
    return NULL;
  }

  // Lecture des 54 premiers octets de l'en-tête BMP
  fread(img->header, sizeof(unsigned char), 54, file);

  // Lecture des valeurs importantes de l'en-tête :
  // La largeur de l'image (stockée à l'offset 18)
  img->width = *(unsigned int*)&img->header[18];
  // La hauteur de l'image (stockée à l'offset 22)
  img->height = *(unsigned int*)&img->header[22];
  // La profondeur de couleur (stockée à l'offset 28)
  img->colorDepth = *(unsigned short*)&img->header[28];
  // La taille des données de l'image (stockée à l'offset 34)
  img->dataSize = *(unsigned int*)&img->header[34];

  // Vérifie que l'image est bien en niveaux de gris (8 bits)
  if ( img->colorDepth != 8) {
    printf("Erreur : le fichier n'est pas une image en 8 bits. \n");
    free(img);
    fclose(file);
    return NULL;
  }

  // Lecture de la Table de couleurs (1024 octets pour 8 bits)
  fread(img->colorTable, sizeof(unsigned char),1024,file);

  //On alloue la mémoire pour les données de l'image (pixels)
  img->data = (unsigned char*)malloc(img->dataSize * sizeof(unsigned char));

  //Vérifie que l'allocation de mémoire pour les données a réussi
  if (img->data == NULL){
    printf("Erreur : mémoire insuffisante pour les données de l'image \n");
    // Libre la mémoire de la structure
    free(img);
    // Ferme le fichier
    fclose(file);
    return NULL;
  }

  // Lecture des données de l'image (pixels en niveaux de gris)
  fread(img->data, sizeof(unsigned char), img->dataSize, file);

  // Fermeture du fichier
  fclose(file);

  // Retour du pointeur versl'image chargée
  return img;
}

void bmp8_free(t_bmp8 * img) {
  free(img);
}

void bmp8_negative(t_bmp8 * img) {
  // Récupération nb pixels
  int nb_pixels = img->height * img->width;
  // Inversion nb pixels
  for (int i =0 ; i < nb_pixels ; i++) {
    img->data[i]= 255 - img->data[i] ;
  }
}

void bmp8_threshold(t_bmp8 * img) {
  // Récupération nb pixels
  int nb_pixels = img->height * img->width ;
  // Définition seuil
  int seuil = 128;
  // Tri en fonction du seuil
  for (int i=0 ; i<nb_pixels ; i++) {
    if (img->data[i] <= seuil) {
      img->data[i] = 0 ;
    }
    else {
      img->data[i] = 255 ;
    }
  }
}

/***void bmp8_applyFilter(t_bmp8 * img, float ** kernel, int kernelSize) {
  // Création autre tableau temporaire
  int * tab = NULL;
  int nb_pixels = img->height * img->width;
  tab = (int *)malloc(nb_pixels*sizeof(int));

  for ( int i = img->width +1 ; i< nb_pixels-img->width -1; i++) {
    tab[i]=
  }

  int tab_mat[kernelSize*kernelSize];
  for (int i = 0; i<kernelSize ; i++) {
    for (int j = 0; j < kernelSize; j++) {
      tab_mat[i+j*]= kernel[i][j];
    }

  }
}***/

void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
  int width = img->width;
  int height = img->height;
  int offset = kernelSize / 2;

  // Allocation d'une nouvelle image temporaire pour stocker les nouvelles valeurs
  unsigned char *newData = malloc(img->dataSize);
  if (newData == NULL) {
    printf("Erreur d'allocation mémoire pour le filtre.\n");
    return;
  }

  // Parcours de l'image (hors bordure)
  for (int y = offset; y < height - offset; y++) {
    for (int x = offset; x < width - offset; x++) {
      float sum = 0.0;

      // Appliquer le noyau
      for (int i = -offset; i <= offset; i++) {
        for (int j = -offset; j <= offset; j++) {
          int pixel = img->data[(y + i) * width + (x + j)];
          float coeff = kernel[i + offset][j + offset];
          sum += pixel * coeff;
        }
      }

      // S'assurer d'être entre 0 et 255
      int value = (int)roundf(sum);
      if (value < 0) value = 0;
      if (value > 255) value = 255;

      newData[y * width + x] = (unsigned char)value;
    }
  }

  // Remplacer les anciennes données par les nouvelles
  for (int i = 0; i < img->dataSize; i++) {
    img->data[i] = newData[i];
  }

  free(newData);
}
