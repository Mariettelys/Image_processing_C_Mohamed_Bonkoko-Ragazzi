//
// Created by marie on 23/05/2025.
//

#include "bmp24.h"


// Partie 2

//Allocation matrice de t_pixel
t_pixel ** bmp24_allocateDataPixels (int width, int height) {
  t_pixel **pixels = NULL; // Pointeur vers tableau de pointeurs de pixels
  int i;
  // Allocation pointeur de pointeurs de lignes pour le tableau
  pixels = (t_pixel **)malloc(height * sizeof(t_pixel *));
  if (pixels == NULL) {
    fprintf(stderr, "Erreur: allocation mémoire impossible pour les pointeurs de lignes de pixels.\n");
    return NULL;
  }

  // Allouer chaque ligne (width pixels)
  for (i = 0; i < height; i++) {
    pixels[i] = (t_pixel *)malloc(width * sizeof(t_pixel));
    if (pixels[i] == NULL) {
      fprintf(stderr, "Erreur : allocation mémoire impossible pour la ligne de pixels %d.\n", i);
      // allocation échouée, -> libérer les lignes déjà allouées
      int j;
      for (j = 0; j < i; j++) {
        free(pixels[j]);
      }
      free(pixels); // Libération tableau pointeurs
      return NULL;
    }
  }

  return pixels;
}

void bmp24_freeDataPixels (t_pixel ** pixels, int height) {
  if (pixels == NULL) {
    return; // On ne retourne rien
  }

  // Libération ligne
  int i;
  for (i = 0; i < height; i++) {
    if (pixels[i] != NULL) {
      free(pixels[i]);
      pixels[i] = NULL;
    }
  }

  // Libération tableau de pointeurs de lignes
  free(pixels);
  pixels = NULL;
}

t_bmp24 * bmp24_allocate (int width, int height, int colorDepth) {
  t_bmp24 *img = NULL;

  // Allocation de la structure t_bmp24
  img = (t_bmp24 *)malloc(sizeof(t_bmp24));
  if (img == NULL) {
    fprintf(stderr, "Erreur : allocation mémoire impossible pour la structure t_bmp24.\n");
    return NULL;
  }

  // Initialisation
  img->width = width;
  img->height = height;
  img->colorDepth = colorDepth;
  img->data = NULL;

  // Allouer la matrice de pixels en appelant bmp24_allocateDataPixels
  img->data = bmp24_allocateDataPixels(width, height);
  if (img->data == NULL) {
    free(img);
    fprintf(stderr, "Erreur : allocation impossible des données de pixels pour t_bmp24.\n");
    return NULL;
  }
  return img;
}

void bmp24_free(t_bmp24 * img) {
  if (img == NULL) {
    return;
  }

  // Libération la matrice de pixels
  if (img->data != NULL) {
    bmp24_freeDataPixels(img->data, img->height);
    img->data = NULL;
  }

  // Libération la structure t_bmp24
  free(img);
  img = NULL;
}

t_bmp24 * bmp24_loadImage (const char * filename) {
    FILE *file = NULL;
    t_bmp24 *img = NULL;

    file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Erreur : ouverture du fichier impossible %s\n", filename);
        return NULL;
    }

    // Lecture du header pour informations relatives à l'image
    t_bmp_header temp_header;
    t_bmp_info temp_info;

    file_rawRead(BITMAP_MAGIC, &temp_header, sizeof(t_bmp_header), 1, file); // Lit l'en-tête pour l'offset
    file_rawRead(BITMAP_MAGIC + HEADER_SIZE, &temp_info, sizeof(t_bmp_info), 1, file); // Lit l'info header

    img = bmp24_allocate(temp_info.width, temp_info.height, temp_info.bits);
    if (img == NULL) {
        printf("Erreur : Échec de l'allocation mémoire pour la structure t_bmp24.\n");
        fclose(file);
        return NULL;
    }

    //Copier en-têtes lues temporairement dans structure allouée
    img->header = temp_header;
    img->header_info = temp_info;

    // Vérification profondeur de couleur
    if (img->colorDepth != DEFAULT_DEPTH) {
        printf("Erreur : L'image %s n'est pas une image 24 bits (profondeur : %u).\n", filename, img->colorDepth);
        bmp24_free(img);
        fclose(file);
        return NULL;
    }

    // Vérification largeur et hauteur de l'image ( = multiples de 4)
    if ((img->width * sizeof(t_pixel)) % 4 != 0) {
        printf("Attention: La largeur de l'image (%d) n'est pas un multiple de 4 octets par ligne (width * 3 = %d).\n", img->width, img->width * 3);
    }

    // Lecture données image et initialisation matrice de pixels avec bmp24_readPixelData
    bmp24_readPixelData(img, file);



    // Lecture en-tête (14 octets)
    if (fread(&img->header, 1, HEADER_SIZE, file) != HEADER_SIZE) {
        printf("Erreur : Lecture de l'en-tête du fichier incomplète pour %s.\n", filename);
        bmp24_free(img);
        fclose(file);
        return NULL;
    }

    // Lecture informations de l'image (40 octets)
    if (fread(&img->header_info, 1, INFO_SIZE, file) != INFO_SIZE) {
        printf("Erreur : Lecture des infos de l'image incomplète pour %s.\n", filename);
        bmp24_free(img);
        fclose(file);
        return NULL;
    }

    // Récupération des dimensions et la profondeur de couleur
    img->width = img->header_info.width;
    img->height = img->header_info.height;
    img->colorDepth = img->header_info.bits;

    if (img->colorDepth != DEFAULT_DEPTH) {
        printf("Erreur : L'image %s n'est pas une image 24 bits (profondeur : %u).\n", filename, img->colorDepth);
        bmp24_free(img);
        fclose(file);
        return NULL;
    }

    // Allocation matrice de pixels
    img->data = bmp24_allocateDataPixels(img->width, img->height);
    if (img->data == NULL) {
        bmp24_free(img);
        fclose(file);
        return NULL;
    }

    // Position au début des pixels ( vérification si éventuelles autre informations avant )
    fseek(file, img->header.offset, SEEK_SET);

    // Calcul du padding par ligne
    int row_byte_size = img->width * sizeof(t_pixel); // Largeur en octets sans padding
    padding = (4 - (row_byte_size % 4)) % 4; // Nombre d'octets de padding

    // Lire les pixels ligne par ligne, de bas en haut (format BMP)
    for (i = img->height - 1; i >= 0; i--) {
        for (j = 0; j < img->width; j++) {
            if (fread(&img->data[i][j], sizeof(t_pixel), 1, file) != 1) {
                printf("Erreur : Lecture des données de pixels incomplète pour %s.\n", filename);
                bmp24_free(img);
                fclose(file);
                return NULL;
            }
        }
        //octets de padding
        fseek(file, padding, SEEK_CUR);
    }

    fclose(file);
    return img;
}

void bmp24_saveImage (t_bmp24 * img, const char * filename) {

    if (img == NULL) {
        printf("Erreur : L'image à sauvegarder n'existe pas.\n");
        return;
    }

    FILE *file = NULL; // Pointeur vers le fichier à créer
    int i, j;
    int padding;        // Nombre d'octets de remplissage par ligne
    unsigned char pad_byte = 0x00; //Octet nul pour le remplissage

    // 2. Ouvrir le fichier en mode écriture binaire
    file = fopen(filename, "wb"); // wb -> écriture en mode binaire
    if (file == NULL) {
        printf("Erreur : Ouverture du fichier impossible %s pour l'écriture.\n", filename);
        return;
    }

    // Calculer les tailles et le padding
    int row_byte_size = img->width * sizeof(t_pixel); // Taille d'une ligne de pixels en octets (sans padding)
    padding = (4 - (row_byte_size % 4)) % 4;         // Calcul du nombre d'octets de padding

    // Calcul taille des données de pixels  (avec padding pour chaque ligne)
    img->header_info.imagesize = (uint32_t)(img->height * (row_byte_size + padding));

    // Calcul taille totale du fichier (en-têtes + données de pixels)
    img->header.size = (uint32_t)(HEADER_SIZE + INFO_SIZE + img->header_info.imagesize);

    // L'offset des données est souvent juste après les en-têtes
    img->header.offset = HEADER_SIZE + INFO_SIZE; // 14 (header) + 40 (info) = 54

    // Ecriture en-tête du fichier (t_bmp_header - 14 octets)
    fwrite(&img->header, 1, HEADER_SIZE, file);

    // Ecriture informations de l'image (t_bmp_info - 40 octets)
    fwrite(&img->header_info, 1, INFO_SIZE, file);

    // Ecriture données des pixels
    for (i = img->height - 1; i >= 0; i--) {
        for (j = 0; j < img->width; j++) {
            // Écrire les 3 octets (Bleu, Vert, Rouge) pour chaque pixel
            fwrite(&img->data[i][j], sizeof(t_pixel), 1, file);
        }
        // Ecriture octets de padding
        for (j = 0; j < padding; j++) {
            fwrite(&pad_byte, 1, 1, file); // Ecriture octet null
        }
    }

    fclose(file);
    printf("Image sauvegardée sous : %s\n", filename);
}

/*
* @brief Positionne le curseur de fichier à la position position dans le fichier file,
* puis lit n éléments de taille size dans buffer.
* @param position La position à partir de laquelle il faut lire dans file.
* @param buffer Le buffer pour stocker les données lues.
* @param size La taille de chaque élément à lire.
* @param n Le nombre d'éléments à lire.
* @param file Le descripteur de fichier dans lequel il faut lire.
* @return void
*/
void file_rawRead (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file) {
  fseek(file, position, SEEK_SET);
  fread(buffer, size, n, file);
}

/*
 * @brief Positionne le curseur de fichier à la position position dans le fichier file,
 * puis écrit n éléments de taille size depuis le buffer.
 * @param position La position à partir de laquelle il faut écrire dans file.
 * @param buffer Le buffer contenant les éléments à écrire.
 * @param size La taille de chaque élément à écrire.
 * @param n Le nombre d'éléments à écrire.
 * @param file Le descripteur de fichier dans lequel il faut écrire.
 * @return void
 */
void file_rawWrite (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file) {
  fseek(file, position, SEEK_SET);
  fwrite(buffer, size, n, file);
}