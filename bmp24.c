//
// Created by mariette on 23/05/2025.
// fichier qui regroupe les fonctions relatives au traitement d'image colorées format BMP 24 bits
// fonctions de la partie 2 du projet
// fichier source bmp24.c : définitions des fonctions

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
    t_bmp_header temp_header;
    t_bmp_info temp_info;
    int width, height, colorDepth;

    file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Erreur: Impossible d'ouvrir le fichier %s.\n", filename);
        return NULL;
    }

    // Lecture informations initiales pour l'allocation (offsets directs pour la lecture)
    file_rawRead(BITMAP_WIDTH, &width, sizeof(int32_t), 1, file);
    file_rawRead(BITMAP_HEIGHT, &height, sizeof(int32_t), 1, file);
    file_rawRead(BITMAP_DEPTH, &colorDepth, sizeof(uint16_t), 1, file);

    if (colorDepth != DEFAULT_DEPTH) {
        printf("Erreur: L'image n'est pas BMP 24 bits. %d\n", colorDepth);
        fclose(file);
        return NULL;
    }

    // Vérification des dimensions (on veut des multiples de 4)
    // Pour traîter le padding
    if (width % 4 != 0 || height % 4 != 0) {
        printf("Avertissement: L'image %s n'a pas des dimensions multiples de 4. Le padding peut ne pas être géré correctement.\n", filename);
    }

    img = bmp24_allocate(width, height, colorDepth);
    if (img == NULL) {
        fclose(file);
        return NULL;
    }

    // Lecture des en-têtes du fichier
    file_rawRead(BITMAP_MAGIC, &temp_header.type, sizeof(uint16_t), 1, file);
    file_rawRead(BITMAP_SIZE, &temp_header.size, sizeof(uint32_t), 1, file);
    file_rawRead(0x04, &temp_header.reserved1, sizeof(uint16_t), 1, file);
    file_rawRead(0x06, &temp_header.reserved2, sizeof(uint16_t), 1, file);
    file_rawRead(BITMAP_OFFSET, &temp_header.offset, sizeof(uint32_t), 1, file);

    if (temp_header.type != BMP_TYPE) {
        printf("Erreur: Le fichier %s n'est pas un fichier BMP valide.\n", filename, temp_header.type);
        bmp24_free(img);
        fclose(file);
        return NULL;
    }

    // Lecture des informations de l'image (header_info)
    file_rawRead(HEADER_SIZE, &temp_info.size, sizeof(uint32_t), 1, file);
    file_rawRead(BITMAP_WIDTH, &temp_info.width, sizeof(int32_t), 1, file);
    file_rawRead(BITMAP_HEIGHT, &temp_info.height, sizeof(int32_t), 1, file);
    file_rawRead(0x1A, &temp_info.planes, sizeof(uint16_t), 1, file);
    file_rawRead(BITMAP_DEPTH, &temp_info.bits, sizeof(uint16_t), 1, file);
    file_rawRead(0x1E, &temp_info.compression, sizeof(uint32_t), 1, file);
    file_rawRead(BITMAP_SIZE_RAW, &temp_info.imagesize, sizeof(uint32_t), 1, file);
    file_rawRead(0x26, &temp_info.xresolution, sizeof(int32_t), 1, file);
    file_rawRead(0x2A, &temp_info.yresolution, sizeof(int32_t), 1, file);
    file_rawRead(0x2E, &temp_info.ncolors, sizeof(uint32_t), 1, file);
    file_rawRead(0x32, &temp_info.importantcolors, sizeof(uint32_t), 1, file);

    img->header = temp_header;
    img->header_info = temp_info;

    // Lecture des données de l'image et initialiser la matrice de pixels
    bmp24_readPixelData(img, file);

    fclose(file);
    return img;
}

void bmp24_saveImage (t_bmp24 * img, const char * filename) {
    FILE *file = NULL;

    if (img == NULL) {
        printf("Erreur: L'image à sauvegarder est NULL.\n");
        return;
    }

    file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Erreur: Impossible de créer ou ouvrir le fichier %s pour écriture.\n", filename);
        return;
    }

    // Écriture en-tête du fichier (header)
    file_rawWrite(BITMAP_MAGIC, &img->header.type, sizeof(uint16_t), 1, file);
    file_rawWrite(BITMAP_SIZE, &img->header.size, sizeof(uint32_t), 1, file);
    file_rawWrite(0x04, &img->header.reserved1, sizeof(uint16_t), 1, file);
    file_rawWrite(0x06, &img->header.reserved2, sizeof(uint16_t), 1, file);
    file_rawWrite(BITMAP_OFFSET, &img->header.offset, sizeof(uint32_t), 1, file);

    // Écriture en-tête d'information (header_info)
    file_rawWrite(HEADER_SIZE, &img->header_info.size, sizeof(uint32_t), 1, file);
    file_rawWrite(BITMAP_WIDTH, &img->header_info.width, sizeof(int32_t), 1, file);
    file_rawWrite(BITMAP_HEIGHT, &img->header_info.height, sizeof(int32_t), 1, file);
    file_rawWrite(0x1A, &img->header_info.planes, sizeof(uint16_t), 1, file);
    file_rawWrite(BITMAP_DEPTH, &img->header_info.bits, sizeof(uint16_t), 1, file);
    file_rawWrite(0x1E, &img->header_info.compression, sizeof(uint32_t), 1, file);
    file_rawWrite(BITMAP_SIZE_RAW, &img->header_info.imagesize, sizeof(uint32_t), 1, file);
    file_rawWrite(0x26, &img->header_info.xresolution, sizeof(int32_t), 1, file);
    file_rawWrite(0x2A, &img->header_info.yresolution, sizeof(int32_t), 1, file);
    file_rawWrite(0x2E, &img->header_info.ncolors, sizeof(uint32_t), 1, file);
    file_rawWrite(0x32, &img->header_info.importantcolors, sizeof(uint32_t), 1, file);

    // Écriture données des pixels
    bmp24_writePixelData(img, file);

    fclose(file);
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

void bmp24_readPixelValue (t_bmp24 * image, int x, int y, FILE * file) {
    if (!image || !image->data || !file || x < 0 || x >= image->width || y < 0 || y >= image->height) return;
    long offset = (long)(image->height - 1 - y) * image->width * sizeof(t_pixel) + (long)x * sizeof(t_pixel);

    fseek(file, image->header.offset + offset, SEEK_SET);
    fread(&(image->data[y][x]), sizeof(t_pixel), 1, file);
}

void bmp24_readPixelData (t_bmp24 * image, FILE * file) {
    if (!image || !image->data || !file || image->width <= 0 || image->height <= 0) {
        return;
    }

    long octets_par_ligne_reel = (long)image->width * sizeof(t_pixel);

    fseek(file, image->header.offset, SEEK_SET);

    for (int bmp_y = 0; bmp_y < image->height; bmp_y++) {
        fread(image->data[image->height - 1 - bmp_y], sizeof(t_pixel), image->width, file);
    }
}

void bmp24_writePixelValue (t_bmp24 * image, int x, int y, FILE * file) {
    if (!image || !image->data || !file || x < 0 || x >= image->width || y < 0 || y >= image->height){
        return;
    }

    long offset = (long)(image->height - 1 - y) * image->width * sizeof(t_pixel) + (long)x * sizeof(t_pixel);

    fseek(file, image->header.offset + offset, SEEK_SET);
    fwrite(&(image->data[y][x]), sizeof(t_pixel), 1, file);
}

void bmp24_writePixelData (t_bmp24 * image, FILE * file) {
    if (!image || !image->data || !file || image->width <= 0 || image->height <= 0) {
        return;
    }
    long octets_par_ligne_reel = (long)image->width * sizeof(t_pixel);

    for (int image_y = 0; image_y < image->height; image_y++) {
        long offset_debut_ligne = (long)(image->height - 1 - image_y) * octets_par_ligne_reel;
        fseek(file, image->header.offset + offset_debut_ligne, SEEK_SET);
        fwrite(image->data[image_y], sizeof(t_pixel), image->width, file);
    }
}

// Fonction qui inverse les couleurs de l'image 24 bits
void bmp24_negative(t_bmp24 *img) {
    if (img == NULL || img->data == NULL) {
        printf("Erreur: La fonctionnalité négatif ne peut pas être appliquée.\n");
        return;
    }

    // Inversion des couleurs de chaque pixel
    for (int x = 0; x < img->height; x++) {
        for (int y = 0; y < img->width; y++) {
            img->data[x][y].red = 255 - img->data[x][y].red;
            img->data[x][y].green = 255 - img->data[x][y].green;
            img->data[x][y].blue = 255 - img->data[x][y].blue;
        }
    }
}

// Fonction qui ajuste la luminosité d'une image 24 bits
void bmp24_brightness(t_bmp24 *img, int value) {
    if (img == NULL || img->data == NULL) {
        printf("Erreur: La fonctionnalité luminosité ne peut pas être appliquée.\n");
        return;
    }

    // Modification ( ajout ou soustraction de valeur de chaque pixel dans chaque canal )
    for (int x = 0; x < img->height; x++) {
        for (int y = 0; y < img->width; y++) {
            // Canal Rouge
            int new_red = (int)img->data[x][y].red + value;
            if (new_red > 255) {
                img->data[x][y].red = 255;
            } else if (new_red < 0) {
                img->data[x][y].red = 0;
            } else {
                img->data[x][y].red = new_red;
            }

            // Canal Vert
            int new_green = (int)img->data[x][y].green + value;
            if (new_green > 255) {
                img->data[x][y].green = 255;
            } else if (new_green < 0) {
                img->data[x][y].green = 0;
            } else {
                img->data[x][y].green = new_green;
            }

            // Canal Bleu
            int new_blue = (int)img->data[x][y].blue + value;
            if (new_blue > 255) {
                img->data[x][y].blue = 255;
            } else if (new_blue < 0) {
                img->data[x][y].blue = 0;
            } else {
                img->data[x][y].blue = new_blue;
            }
        }
    }
}

// Fonction qui convertit une image 24 bits en niveau de gris
void bmp24_grayscale(t_bmp24 *img) {
    if (img == NULL || img->data == NULL) {
        printf("Erreur: La fonctionnalité conversion en niveaux de gris ne peut pas être appliquée.\n");
        return;
    }

    //  Calcule l'intensité de gris pour chaque pixel
    //  en utilisant une moyenne pondérée
    //  et applique cette valeur à tous les canaux(Rouge, Vert, Bleu) du pixel
    for (int x = 0; x < img->height; x++) {
        for (int y = 0; y < img->width; y++) {
            // Récupération des valeurs RVB du pixel actuel
            uint8_t r = img->data[x][y].red;
            uint8_t g = img->data[x][y].green;
            uint8_t b = img->data[x][y].blue;

            // Calculer l'intensité de gris -> formule de luminance
            // Utilisation de float pour le calcul pour plus de précision, puis arrondi
            double gray_value = (0.299 * r + 0.587 * g + 0.114 * b);


            // round() -> arrondir, fmax/fmin pour clamper( forcer à rester entre 2 bornes)
            // Conversion du double en uint8_t (unsigned char).
            uint8_t final_gray = (uint8_t)round(fmax(0.0, fmin(255.0, gray_value)));

            // même valeur de gris aux trois canaux du pixel
            img->data[x][y].red = final_gray;
            img->data[x][y].green = final_gray;
            img->data[x][y].blue = final_gray;
        }
    }
}


// Bonus
// Fonction qui convertie une image 24 bits en noir et blanc en se basant sur une moyenne de l'intensité de chaque pixel
void bmp24_threshold(t_bmp24 *img, int threshold) {
    if (img == NULL || img->data == NULL) {
        printf("Erreur: La fonctionnalité de seuillage ne peut pas être appliquée.\n");
        return;
    }

    // Vérification du seuil dans les limites valides
    if (threshold < 0) threshold = 0;
    if (threshold > 255) threshold = 255;

    for (int x = 0; x < img->height; x++) {
        for (int y = 0; y < img->width; y++) {
            // Calcul intensité moyenne du pixel
            int intensity = (img->data[x][y].red + img->data[x][y].green + img->data[x][y].blue) / 3;

            if (intensity >= threshold) {
                img->data[x][y].red = 255;
                img->data[x][y].green = 255;
                img->data[x][y].blue = 255; // Blanc
            } else {
                img->data[x][y].red = 0;
                img->data[x][y].green = 0;
                img->data[x][y].blue = 0;   // Noir
            }
        }
    }
}