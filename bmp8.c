
#include "bmp8.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void bmp8_saveImage(const char *filename, t_bmp8 *image) {
    FILE *imagedep = fopen(filename, "wb"); // Ouverture de l'image de départ en mode écriture
    if (imagedep == NULL) { // Vérifie si l'ouverture a fonctionné
        fprintf(stderr, "Erreur d'ouverture du fichier pour écriture : %s\n", filename);
        return;
    }

    size_t temp; // Utiliser size_t pour le retour de fwrite

    // Écriture de l'en-tête (54 octets)
    temp = fwrite(image->header, sizeof(unsigned char), 54, imagedep);
    if (temp != 54) {
        fprintf(stderr, "Erreur d'écriture de l'en-tête.\n");
        fclose(imagedep);
        return;
    }

    // Écriture de la table de couleurs (1024 octets)
    temp = fwrite(image->colorTable, sizeof(unsigned char), 1024, imagedep);
    if (temp != 1024) {
        fprintf(stderr, "Erreur d'écriture de la table des couleurs.\n");
        // Correction : Fermer le fichier avant de quitter
        fclose(imagedep);
        return;
    }

    // Écriture des données des pixels
    temp = fwrite(image->data, sizeof(unsigned char), image->dataSize, imagedep);
    if (temp != image->dataSize) {
        fprintf(stderr, "Erreur lors de l'écriture des données de l'image.\n");
        fclose(imagedep);
        return;
    }

    fclose(imagedep);
    // Pas besoin de return ici si c'est la fin de la fonction
}

void bmp8_printInfo(t_bmp8 *image) {
    printf("Image Info:\n"); // Ajout d'un saut de ligne
    printf("\tWidth: %u pixels\n", image->width); // %u pour unsigned int
    printf("\tHeight: %u pixels\n", image->height); // %u pour unsigned int
    printf("\tColor Depth: %u bits\n", image->colorDepth); // %u pour unsigned int
    printf("\tData Size: %u bytes\n", image->dataSize); // %u pour unsigned int
}

void bmp_brightness(t_bmp8 *image, int value) {
    // Correction : La boucle doit commencer à 0 et aller jusqu'à dataSize - 1
    for (unsigned int i = 0; i < image->dataSize; i++) {
        int nvpixel = image->data[i] + value;

        if (nvpixel > 255) {
            nvpixel = 255;
        }
        if (nvpixel < 0) {
            nvpixel = 0;
        }

        image->data[i] = (unsigned char)nvpixel;
        // Correction : Supprimer le 'return;' ici
    }
}

void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
    // Calcul de n (taille moitié du noyau) à partir de kernelSize
    // kernelSize doit être impair pour un noyau centré.
    int n = kernelSize / 2;

    unsigned int width = img->width;
    unsigned int height = img->height;

    // Allocation d'une nouvelle mémoire pour les données de l'image filtrée
    unsigned char *nvdata = (unsigned char *)malloc(img->dataSize);
    if (nvdata == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour nvdata dans bmp8_applyFilter.\n");
        return;
    }

    // Copie initiale des données pour gérer les bords non traités par le filtre
    // Les pixels des bords non traités par le filtre conserveront leur valeur originale.
    for (unsigned int i = 0; i < img->dataSize; i++) {
        nvdata[i] = img->data[i];
    }

    // Boucles pour parcourir les pixels de l'image, en ignorant les bords.
    // Le filtre ne peut pas être appliqué aux 'n' premiers et 'n' derniers pixels de chaque dimension.
    for (unsigned int y = n; y < height - n; y++) {
        for (unsigned int x = n; x < width - n; x++) {
            float somme = 0.0f; // Utiliser float pour la somme intermédiaire

            // Application du noyau (convolution)
            for (int yy = -n; yy <= n; yy++) {
                for (int xx = -n; xx <= n; xx++) {
                    int voisinX = x + xx;
                    int voisinY = y + yy;

                    // Accès au pixel voisin
                    // Assurez-vous que voisinX et voisinY restent dans les limites de l'image
                    // (Les boucles externes (x,y) garantissent cela pour les pixels non-bords)
                    unsigned char pixelVoisin = img->data[voisinY * width + voisinX];
                    float coeff = kernel[yy + n][xx + n];

                    somme += pixelVoisin * coeff;
                }
            }

            // Clamping des valeurs à 0-255
            // Utilisation de fmax et fmin de <math.h> pour la clarté, ou des ifs
            somme = fmax(0.0f, fmin(255.0f, somme));

            // Stockage de la nouvelle valeur du pixel dans le tableau temporaire
            nvdata[y * width + x] = (unsigned char)somme;
        }
    }

    // Copie des données filtrées du tableau temporaire vers les données originales de l'image
    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = nvdata[i];
    }

    // Libération de la mémoire allouée pour le tableau temporaire
    free(nvdata);
    nvdata = NULL; // Bonne pratique
}


// Partie 2


t_pixel** bmp24_allocateDataPixels(int width, int height) {
    t_pixel** data_pixels = (t_pixel**)calloc(height, sizeof(t_pixel*));

    if (data_pixels == NULL) {
        printf("Erreur lors de l'allocation de mémoire pour les  lignes");
        return NULL;
    }
    for (int i = 0; i < height; i++) {
        data_pixels[i] = (t_pixel*)calloc(width, sizeof(t_pixel));

        if (data_pixels[i] == NULL) {
            printf("Erreur lors de l'allocation de mémoire pour une colonne de pixels");
            for (int j = 0; j < i; j++) {
                free(data_pixels[j]);
            }
            free(data_pixels);
            return NULL;
        }
    }

    return data_pixels;
}
void bmp24_freeDataPixels(t_pixel** pixels, int height) {
    if (pixels == NULL) {
        return;
    }

    for (int i = 0; i < height; i++) {
        if (pixels[i] != NULL) {
            free(pixels[i]);
            pixels[i] = NULL;
        }
    }

    free(pixels);
}



// --- Fonctions de Lecture/Écriture Directe des Pixels ---

#include "my_bmp.h"

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

//Partie 3

// Images en gris
unsigned int * bmp8_computeHistogram(t_bmp8 * img) {
    /*calcule l'histogramme de l'image */
    unsigned int *histogram = (unsigned int *)calloc(256, sizeof(unsigned int)); /*création du tableau pour les 256 niveaux de gris possibles  et initialisation à 0*/
    if (histogram == NULL) {// vérifier si l'allocation a réussi
        printf( "Erreur d'allocation mémoire pour l'histogramme.");
        return NULL;
    }
    // Calcul des valeurs de gris chaque i correspond à 1 pixels
    for (unsigned int i = 0; i < img->dataSize; i++) { // Parcours  chaque pixel
        histogram[img->data[i]]+=1; // Incrémente le compteur si on trouve un autre pixel avec cette valeur de gris
    }
    return histogram;
}
unsigned int * bmp8_computeCDF(unsigned int * hist) {
    /*creer un tableau qui contient, à chaque indice,  la somme de l'occurrence de tous les pixels
     *qui ont un certain niveau de gris ou un niveau de gris inférieur
     *à celui ci*/

    unsigned int *cdf = (unsigned int *)calloc(256, sizeof(unsigned int)); /*création du tableau pour les 256 niveaux de gris possibles et pour la cdf et initialisation à 0*/
    unsigned int *hist_eq = (unsigned int *)calloc(256, sizeof(unsigned int));

    if (cdf == NULL || hist_eq == NULL) { // vérifie si les allocations ont reussi 
        printf("Echec d'allocation mémoire pour la CDF ou hist_eq.\n");
        free(cdf); // libere la memoire conenue dans cdf 
        free(hist_eq);
        return NULL;
    }

    // Calcul de la CDF
    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + hist[i];
    }

    unsigned int N = cdf[255]; // Nombre  de pixel de l'image

    // Trouver cdf_min
    unsigned int cdf_min = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] > 0) {
            cdf_min = cdf[i];
            break;
        }
    }

    double denominator = (double)N - cdf_min;
    if (denominator == 0) {
        denominator = 1;
    }

    // Etapes de normalisation de la CDF selon la formule donnée dans le projet 
    for (int i = 0; i < 256; i++) {
        double numerator = (double)cdf[i] - cdf_min;
        hist_eq[i] = (unsigned int)round((numerator / denominator) * 255.0);

        // S'assurer que la valeur est bien entre 0 et 255
        if (hist_eq[i] > 255) hist_eq[i] = 255;
    }
    free(cdf); // Libére la mémoire allouée pour cdf
    return hist_eq; // Retourne le tableau de la CDF

}

void bmp8_equalize(t_bmp8 * img, unsigned int * hist_eq){ /*chaque pixel pour une valeur de gris i est remplacé par une valeur associée déterminée à partir de la cdf et de la normalisation */
    unsigned int * hist= bmp8_computeHistogram(img);
    hist_eq =bmp8_computeCDF(hist);
   
    for (int i = 0; i < img->dataSize ; i++) {

        img->data[i] = hist_eq[ img->data[i] ];
    }

    return;

    }

//Images en couleur


