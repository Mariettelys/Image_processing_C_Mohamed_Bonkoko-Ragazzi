#include "bmp8.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

t_bmp8 *bmp8_loadImage(const char *filename) {
    FILE *file = NULL;
    t_bmp8 *img = NULL;

    // 1. Ouvrir le fichier en mode lecture binaire
    file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }

    // 2. Allouer de la mémoire pour la structure t_bmp8
    img = (t_bmp8 *)malloc(sizeof(t_bmp8));
    if (img == NULL) {
        fprintf(stderr, "Erreur : Échec de l'allocation mémoire pour la structure t_bmp8\n");
        fclose(file);
        return NULL;
    }

    // 3. Lire l'en-tête du fichier (54 octets)
    if (fread(img->header, 1, 54, file) != 54) {
        fprintf(stderr, "Erreur : Lecture de l'en-tête incomplète pour %s\n", filename);
        bmp8_free(img);
        fclose(file);
        return NULL;
    }

    // 4. Extraire les informations de l'en-tête et initialiser les champs de img
    // Largeur (width) - Offset 18 (4 octets)
    img->width = (unsigned int)(img->header[18] |
                                (img->header[19] << 8) |
                                (img->header[20] << 16) |
                                (img->header[21] << 24));

    // Hauteur (height) - Offset 22 (4 octets)
    img->height = (unsigned int)(img->header[22] |
                                 (img->header[23] << 8) |
                                 (img->header[24] << 16) |
                                 (img->header[25] << 24));

    // Profondeur de couleur (colorDepth) - Offset 28 (2 octets)
    // C'est un short (16 bits), donc seulement 2 octets
    img->colorDepth = (unsigned int)(img->header[28] |
                                     (img->header[29] << 8));

    // Taille des données de l'image (dataSize) - Offset 34 (4 octets)
    img->dataSize = (unsigned int)(img->header[34] |
                                   (img->header[35] << 8) |
                                   (img->header[36] << 16) |
                                   (img->header[37] << 24));

    // 5. Vérifier la profondeur de couleur (doit être 8 bits)
    if (img->colorDepth != 8) {
        fprintf(stderr, "Erreur : L'image %s n'est pas une image 8 bits en niveaux de gris (profondeur : %u).\n", filename, img->colorDepth);
        bmp8_free(img);
        fclose(file);
        return NULL;
    }

    // 6. Lire la table de couleurs (1024 octets pour 8 bits)
    if (fread(img->colorTable, 1, 1024, file) != 1024) {
        fprintf(stderr, "Erreur : Lecture de la table de couleurs incomplète pour %s\n", filename);
        bmp8_free(img);
        fclose(file);
        return NULL;
    }

    // 7. Allouer de la mémoire pour les données des pixels (data)
    img->data = (unsigned char *)malloc(img->dataSize);
    if (img->data == NULL) {
        fprintf(stderr, "Erreur : Échec de l'allocation mémoire pour les données des pixels de %s\n", filename);
        bmp8_free(img);
        fclose(file);
        return NULL;
    }

    // 8. Lire les données des pixels
    if (fread(img->data, 1, img->dataSize, file) != img->dataSize) {
        fprintf(stderr, "Erreur : Lecture des données de l'image incomplète pour %s\n", filename);
        bmp8_free(img);
        fclose(file);
        return NULL;
    }

    // 9. Fermer le fichier
    fclose(file);

    // 10. Retourner le pointeur vers l'image chargée
    return img;
}



/*t_bmp8 * bmp8_loadImage(const char * filename){
  printf("Tentative d'ouverture du fichier : %s\n", filename);
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
}*/

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