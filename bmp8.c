//
// auteur : mariette
// fichier qui regroupe les fonctions relatives au traitement d'image en niveaux de gris format BMP 8 bits
// fonctions de la partie 1 du projet
// fichier source bmp8.c : prototypes des fonctions
// + définition du type t_bmp8
//
#include "bmp8.h"

t_bmp8 *bmp8_loadImage(const char *filename) {
    FILE *file = NULL;
    t_bmp8 *img = NULL;

    // Ouverture du fichier en mode lecture binaire
    file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }

    // Allocation de la mémoire pour la structure t_bmp8
    img = (t_bmp8 *)malloc(sizeof(t_bmp8));
    if (img == NULL) {
        printf("Erreur : Allocation de la mémoire impossible , espace insuffisant\n");
        fclose(file);
        return NULL;
    }

    // Lecture de l'en-tête du fichier (54 octets)
    if (fread(img->header, 1, 54, file) != 54) {
        printf("Erreur : Lecture de l'en-tête incomplète pour %s\n", filename);
        bmp8_free(img);
        fclose(file);
        return NULL;
    }

    // Extraction des informations de l'en-tête et initialiser les champs de img
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
    // Ici short (16 bits), donc seulement 2 octets
    img->colorDepth = (unsigned int)(img->header[28] |
                                     (img->header[29] << 8));

    // Taille des données de l'image (dataSize) - Offset 34 (4 octets)
    img->dataSize = (unsigned int)(img->header[34] |
                                   (img->header[35] << 8) |
                                   (img->header[36] << 16) |
                                   (img->header[37] << 24));

    // Vérification de la profondeur de couleur (doit être 8 bits)
    if (img->colorDepth != 8) {
        printf("Erreur : L'image %s n'est pas une image 8 bits en niveaux de gris .\n", filename);
        bmp8_free(img);
        fclose(file);
        return NULL;
    }

    // Lecture de la table de couleurs (1024 octets pour 8 bits)
    if (fread(img->colorTable, 1, 1024, file) != 1024) {
        printf("Erreur : Lecture de la table de couleurs incomplète pour %s\n", filename);
        bmp8_free(img);
        fclose(file);
        return NULL;
    }

    // Allocation de la mémoire pour les données des pixels (data)
    img->data = (unsigned char *)malloc(img->dataSize);
    if (img->data == NULL) {
        printf("Erreur : Allocation de la mémoire impossible pour les données des pixels de %s, espace insuffisant\n", filename);
        bmp8_free(img);
        fclose(file);
        return NULL;
    }

    // Lecture des données des pixels
    if (fread(img->data, 1, img->dataSize, file) != img->dataSize) {
        printf("Erreur : Lecture des données de l'image incomplète pour %s\n", filename);
        bmp8_free(img);
        fclose(file);
        return NULL;
    }

    fclose(file);
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
  for (int x = offset; x < height - offset; x++) {
    for (int y = offset; y < width - offset; y++) {
      float sum = 0.0;

      // Appliquer le noyau
      for (int i = -offset; i <= offset; i++) {
        for (int j = -offset; j <= offset; j++) {
          int pixel = img->data[(x + i) * width + (y + j)];
          float coeff = kernel[i + offset][j + offset];
          sum += pixel * coeff;
        }
      }

      // S'assurer d'être entre 0 et 255
      int value = (int)roundf(sum);
      if (value < 0) value = 0;
      if (value > 255) value = 255;

      newData[x * width + y] = (unsigned char)value;
    }
  }

  // Remplacer les anciennes données par les nouvelles
  for (int i = 0; i < img->dataSize; i++) {
    img->data[i] = newData[i];
  }

  free(newData);
}

