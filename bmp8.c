
#include "bmp8.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>



// Partie 1-----------------------------------------------------------------

int bmp8_saveImage(const char *filename, t_bmp8 *image) {
    FILE *imagedep = fopen(filename, "wb"); // Ouverture de l'image de départ en mode écriture
    if (imagedep == NULL) { // Vérifie si l'ouverture a fonctionné
        printf( "Erreur d'ouverture de fichier ");
        return 0;
    }

    size_t temp;

    // Écriture de l'en-tête
    temp = fwrite(image->header, sizeof(unsigned char), 54, imagedep);
    if (temp != 54) {
        printf( "Erreur d'écriture de l'en-tête.");
        fclose(imagedep);
        return 0;
    }

    // Écriture de la table de couleurs
    temp = fwrite(image->colorTable, sizeof(unsigned char), 1024, imagedep);
    if (temp != 1024) {
        printf("Erreur d'écriture de la table des couleurs.");

        fclose(imagedep);
        return 0;
    }

    // Écriture des données des pixels
    temp = fwrite(image->data, sizeof(unsigned char), image->dataSize, imagedep);
    if (temp != image->dataSize) {
        printf("Erreur d'écriture des données de l'image");
        fclose(imagedep);
        return 0;
    }

    fclose(imagedep);
    return 1;
}
void bmp8_printInfo(t_bmp8 *image) {
    printf("Image Info:\n");
    printf("\tWidth: %u pixels\n", image->width);
    printf("\tHeight: %u pixels\n", image->height);
    printf("\tColor Depth: %u bits\n", image->colorDepth);
    printf("\tData Size: %u bytes\n", image->dataSize);
}
void bmp8_brightness(t_bmp8 *image, int value) {
    for (unsigned int i = 0; i < image->dataSize; i++) {
        int nvpixel = image->data[i] + value;

        if (nvpixel > 255) {
            nvpixel = 255;
        }
        if (nvpixel < 0) {
            nvpixel = 0;
        }

        image->data[i] = (unsigned char)nvpixel;
    }
}
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
    if (!img || !img->data || !kernel || kernelSize <= 0 || kernelSize % 2 == 0) {
        printf( "Erreur: Paramètres invalides pour bmp8_applyFilter.");
        return;
    }

    int n = kernelSize / 2;
    unsigned int width = img->width;
    unsigned int height = img->height;

    unsigned char *nvdata = (unsigned char *)malloc(img->dataSize);
    if (nvdata == NULL) {
        printf("Erreur d'allocation mémoire pour nvdata dans bmp8_applyFilter.");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        nvdata[i] = img->data[i];
    }

    for (unsigned int y = n; y < height - n; y++) {
        for (unsigned int x = n; x < width - n; x++) {
            float somme_ponderee = 0.0f;
            for (int yy = -n; yy <= n; yy++) {
                for (int xx = -n; xx <= n; xx++) {
                    int voisinX = x + xx;
                    int voisinY = y + yy;
                    unsigned char pixelVoisin = img->data[voisinY * width + voisinX];
                    float coeff = kernel[yy + n][xx + n];
                    somme_ponderee += pixelVoisin * coeff;
                }
            }
            somme_ponderee = fmax(0.0f, fmin(255.0f, somme_ponderee));
            nvdata[y * width + x] = (unsigned char)round(somme_ponderee);
        }
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = nvdata[i];
    }

    free(nvdata);
    nvdata = NULL;
}


// Partie 2--------------------------------------------------------------


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
void bmp24_readPixelValue (t_bmp24 * image, int x, int y, FILE * file) {
    // Vérification des paramètres d'entrée
    if (image == NULL) {
        printf("Erreur : L'objet image est NULL dans bmp24_readPixelValue.\n");
        return;
    }
    if (image->data == NULL) {
        printf("Erreur : Les données de pixels de l'image sont NULL dans bmp24_readPixelValue.\n");
        return;
    }
    if (file == NULL) {
        printf("Erreur : Le pointeur de fichier est NULL dans bmp24_readPixelValue.\n");
        return;
    }
    if (x < 0 || x >= image->width || y < 0 || y >= image->height) {
        printf("Erreur : Coordonnées (%d, %d) hors limites pour l'image (largeur %d, hauteur %d) dans bmp24_readPixelValue.\n", x, y, image->width, image->height);
        return;
    }

    long octetsParLigne = (long)image->width * sizeof(t_pixel);
    long offsetLigne = (long)(image->height - 1 - y) * octetsParLigne;
    long offsetPixelDansLigne = (long)x * sizeof(t_pixel);
    long positionAbsolue = image->header.offset + offsetLigne + offsetPixelDansLigne;

    fseek(file, positionAbsolue, SEEK_SET);
    size_t elementsLus = fread(&(image->data[y][x]), sizeof(t_pixel), 1, file);

    if (elementsLus != 1) {
        printf("Avertissement : Échec de la lecture d'un pixel à (%d, %d) dans bmp24_readPixelValue.\n", x, y);
    }
}
void bmp24_readPixelData (t_bmp24 * image, FILE * file) {
    // Vérification des paramètres d'entrée
    if (image == NULL) {
        printf("Erreur : L'objet image est NULL dans bmp24_readPixelData.\n");
        return;
    }
    if (image->data == NULL) {
        printf("Erreur : Les données de pixels de l'image sont NULL dans bmp24_readPixelData.\n");
        return;
    }
    if (file == NULL) {
        printf("Erreur : Le pointeur de fichier est NULL dans bmp24_readPixelData.\n");
        return;
    }
    if (image->width <= 0 || image->height <= 0) {
        printf("Erreur : Largeur (%d) ou hauteur (%d) de l'image invalide dans bmp24_readPixelData.\n", image->width, image->height);
        return;
    }

    long octetsParLigne = (long)image->width * sizeof(t_pixel);

    fseek(file, image->header.offset, SEEK_SET);

    for (int yFichier = 0; yFichier < image->height; yFichier++) {
        size_t elementsLus = fread(image->data[image->height - 1 - yFichier], sizeof(t_pixel), image->width, file);
        if (elementsLus != image->width) {
            printf("Avertissement : Échec de la lecture de la ligne %d du fichier BMP (attendu %d pixels, lus %zu) dans bmp24_readPixelData.\n", yFichier, image->width, elementsLus);
            // On continue le traitement, mais c'est une indication qu'il y a un problème.
            // Pour un code plus robuste, on pourrait choisir de retourner ici.
        }
    }
}
void bmp24_writePixelValue (t_bmp24 * image, int x, int y, FILE * file) {
    // Vérification des paramètres d'entrée
    if (image == NULL) {
        printf("Erreur : L'objet image est NULL dans bmp24_writePixelValue.\n");
        return;
    }
    if (image->data == NULL) {
        printf("Erreur : Les données de pixels de l'image sont NULL dans bmp24_writePixelValue.\n");
        return;
    }
    if (file == NULL) {
        printf("Erreur : Le pointeur de fichier est NULL dans bmp24_writePixelValue.\n");
        return;
    }
    if (x < 0 || x >= image->width || y < 0 || y >= image->height) {
        printf("Erreur : Coordonnées (%d, %d) hors limites pour l'image (largeur %d, hauteur %d) dans bmp24_writePixelValue.\n", x, y, image->width, image->height);
        return;
    }

    long octetsParLigne = (long)image->width * sizeof(t_pixel);
    long offsetLigne = (long)(image->height - 1 - y) * octetsParLigne;
    long offsetPixelDansLigne = (long)x * sizeof(t_pixel);
    long positionAbsolue = image->header.offset + offsetLigne + offsetPixelDansLigne;

    fseek(file, positionAbsolue, SEEK_SET);
    size_t elementsEcrits = fwrite(&(image->data[y][x]), sizeof(t_pixel), 1, file);

    if (elementsEcrits != 1) {
        printf("Avertissement : Échec de l'écriture d'un pixel à (%d, %d) dans bmp24_writePixelValue.\n", x, y);
    }
}
void bmp24_writePixelData (t_bmp24 * image, FILE * file) {
    // Vérification des paramètres d'entrée
    if (image == NULL) {
        printf("L'image est vide ");
        return;
    }
    if (image->data == NULL) {
        printf(" Erreur : Les données de pixels de l'image sont nulles ");
        return;
    }
    if (file == NULL) {
        printf("Erreur : le fichier est vide .");
        return;
    }
    if (image->width <= 0 || image->height <= 0) {
        printf(" mauvaises indications de format ");
        return;
    }

    long octetsParLigne = (long)image->width * sizeof(t_pixel);

    for (int yMemoire = 0; yMemoire < image->height; yMemoire++) {
        long offsetLigneFichier = (long)(image->height - 1 - yMemoire) * octetsParLigne;
        long positionAbsolueLigne = image->header.offset + offsetLigneFichier;

        fseek(file, positionAbsolueLigne, SEEK_SET);
        size_t elementsEcrits = fwrite(image->data[yMemoire], sizeof(t_pixel), image->width, file);

        if (elementsEcrits != image->width) {
            printf("Avertissement : Échec de l'écriture de la ligne %d (en mémoire) / %d (dans le fichier) dans bmp24_writePixelData.\n", yMemoire, image->height - 1 - yMemoire);
            return;
        }
    }
}

void bmp24_convolution(t_bmp24 *img, float **kernel, int kernelSize) {
    if (img == NULL) {
        printf("Erreur: Le pointeur 'img' (image) est NULL.\n");
        return;
    }
    if (img->data == NULL) {
        printf("Erreur: Les donnees de pixels de l'image (img->data) sont NULL.\n");
        return;
    }
    if (kernel == NULL) {
        printf("Erreur: Le pointeur 'kernel' (noyau de filtre) est NULL.\n");
        return;
    }
    if (kernelSize <= 0) {
        printf("Erreur: La taille du noyau (kernelSize) doit etre superieure a 0.\n");
        return;
    }
    if (kernelSize % 2 == 0) {
        printf("Erreur: La taille du noyau (kernelSize) doit etre un nombre impair (ex: 3, 5, 7...). \n");
        return;
    }

    // Ajout de la vérification de divisibilité par 4
    if (img->width % 4 != 0 || img->height % 4 != 0) {
        printf("Avertissement: La largeur ou la hauteur de l'image n'est pas divisible par 4. Cela peut causer des problemes avec le padding dans le format BMP si non gere.\n");
    }

    int n = kernelSize / 2;
    unsigned int width = img->width;
    unsigned int height = img->height;

    t_pixel **nvdata = (t_pixel **)malloc(height * sizeof(t_pixel *));
    if (nvdata == NULL) {
        printf("Erreur d'allocation memoire pour nvdata (lignes).\n");
        return;
    }
    for (unsigned int y = 0; y < height; y++) {
        nvdata[y] = (t_pixel *)malloc(width * sizeof(t_pixel));
        if (nvdata[y] == NULL) {
            printf("Erreur d'allocation memoire pour nvdata (colonnes).\n");
            for (unsigned int prev_y = 0; prev_y < y; prev_y++) {
                free(nvdata[prev_y]);
            }
            free(nvdata);
            return;
        }
    }

    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            nvdata[y][x] = img->data[y][x];
        }
    }

    for (unsigned int y = n; y < height - n; y++) {
        for (unsigned int x = n; x < width - n; x++) {
            float sum_red = 0.0f;
            float sum_green = 0.0f;
            float sum_blue = 0.0f;

            for (int yy = -n; yy <= n; yy++) {
                for (int xx = -n; xx <= n; xx++) {
                    int neighborX = x + xx;
                    int neighborY = y + yy;

                    t_pixel neighborPixel = img->data[neighborY][neighborX];
                    float coeff = kernel[yy + n][xx + n];

                    sum_red += neighborPixel.red * coeff;
                    sum_green += neighborPixel.green * coeff;
                    sum_blue += neighborPixel.blue * coeff;
                }
            }

            nvdata[y][x].red = (unsigned char)round(fmax(0.0f, fmin(255.0f, sum_red)));
            nvdata[y][x].green = (unsigned char)round(fmax(0.0f, fmin(255.0f, sum_green)));
            nvdata[y][x].blue = (unsigned char)round(fmax(0.0f, fmin(255.0f, sum_blue)));
        }
    }

    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            img->data[y][x] = nvdata[y][x];
        }
    }

    for (unsigned int y = 0; y < height; y++) {
        free(nvdata[y]);
    }
    free(nvdata);
    nvdata = NULL;
}


//Partie 3---------------------------------------------------------------

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
        printf("Echec d'allocation mémoire pour la CDF ou hist_eq.");
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
    // Si cdf_min est 0, on ne peut pas diviser par 0, donc on le remplace par 1
    double denominator = (double)N - cdf_min;
    if (denominator == 0) {
        denominator = 1;
    }

    // Etapes de normalisation de la CDF selon la formule donnée dans le projet
    for (int i = 0; i < 256; i++) {
        double numerator = (double)cdf[i] - cdf_min;
        hist_eq[i] = (unsigned int)round((numerator / denominator) * 255.0);

        // Verifie que la valeur est bien entre 0 et 255
        if (hist_eq[i] > 255) hist_eq[i] = 255;
    }
    free(cdf);
    return hist_eq;

}
void bmp8_equalize(t_bmp8 * img, unsigned int * hist_eq){ /*chaque pixel pour une valeur de gris i est remplacé par une valeur associée déterminée à partir de la cdf et de la normalisation */
    unsigned int * hist= bmp8_computeHistogram(img);
    hist_eq =bmp8_computeCDF(hist);

    for (int i = 0; i < img->dataSize ; i++) {

        img->data[i] = hist_eq[ img->data[i] ];
    }
    free(hist);
    free(hist_eq);
    return;

    }
//Image en couleur 24 bits
void bmp24_equalize(t_bmp24 * img) {
    if (!img || !img->data || img->width <= 0 || img->height <= 0) {
        printf("Erreur : L'image 24 bits fournie est invalide ou vide.\n");
        return;
    }

    unsigned int nombreTotalPixels = img->width * img->height;


    t_pixel_yuv *pixelsYUV = (t_pixel_yuv *)malloc(nombreTotalPixels * sizeof(t_pixel_yuv));
    if (pixelsYUV == NULL) {
        printf("Erreur : Impossible d'allouer la mémoire pour les pixels YUV.\n");
        return;
    }
    unsigned char *valeursYLumieres = (unsigned char *)malloc(nombreTotalPixels * sizeof(unsigned char));


    if (pixelsYUV == NULL || valeursYLumieres == NULL) {
        printf("Erreur : Impossible d'allouer la mémoire pour les pixels YUV ou les valeurs Y pour l'histogramme.\n");
        free(pixelsYUV);
        free(valeursYLumieres);
        return;
    }



    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int indicePixel = y * img->width + x; // Indice linéaire pour les tableaux 1D

            float valeurRouge = img->data[y][x].red;
            float valeurVerte = img->data[y][x].green;
            float valeurBleue = img->data[y][x].blue;

            pixelsYUV[indicePixel].y_comp = 0.299f * valeurRouge + 0.587f * valeurVerte + 0.114f * valeurBleue;
            pixelsYUV[indicePixel].u_comp = -0.14713f * valeurRouge - 0.28886f * valeurVerte + 0.436f * valeurBleue;
            pixelsYUV[indicePixel].v_comp = 0.615f * valeurRouge - 0.51499f * valeurVerte - 0.10001f * valeurBleue;

            valeursYLumieres[indicePixel] = (unsigned char)round(fmax(0.0f, fmin(255.0f, pixelsYUV[indicePixel].y_comp)));
        }
    }

    t_bmp8 imageLumianceTemporaire;
    imageLumianceTemporaire.width = img->width;
    imageLumianceTemporaire.height = img->height;
    imageLumianceTemporaire.dataSize = nombreTotalPixels;
    imageLumianceTemporaire.data = valeursYLumieres;


    unsigned int *histogrammeLuminance = bmp8_computeHistogram(&imageLumianceTemporaire);
    if (histogrammeLuminance == NULL) {
        printf("Erreur d'allocation du tableaux d'histogramme pour la luminance");
        free(pixelsYUV);
        free(valeursYLumieres);
        return;
    }


    unsigned int *tableTransformationLuminance = bmp8_computeCDF(histogrammeLuminance);
    if (tableTransformationLuminance == NULL) {
        printf("Erreur : Échec lors du calcul de la CDF et de la table de transformation pour Y.\n");
        free(histogrammeLuminance);
        free(pixelsYUV);
        free(valeursYLumieres);
        return;
    }

    for (unsigned int i = 0; i < nombreTotalPixels; i++) {
        pixelsYUV[i].y_comp = (float)tableTransformationLuminance[valeursYLumieres[i]];
    }


    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int indicePixel = y * img->width + x;


            float luminanceEgalisee = pixelsYUV[indicePixel].y_comp;
            float chrominanceUOriginale = pixelsYUV[indicePixel].u_comp;
            float chrominanceVOriginale = pixelsYUV[indicePixel].v_comp;


            float valeurRougeFlottante = luminanceEgalisee + 1.13983f * chrominanceVOriginale;
            float valeurVerteFlottante = luminanceEgalisee - 0.39465f * chrominanceUOriginale - 0.58060f * chrominanceVOriginale;
            float valeurBleueFlottante = luminanceEgalisee + 2.03211f * chrominanceUOriginale;


            img->data[y][x].red = (uint8_t)round(fmax(0.0f, fmin(255.0f, valeurRougeFlottante)));
            img->data[y][x].green = (uint8_t)round(fmax(0.0f, fmin(255.0f, valeurVerteFlottante)));
            img->data[y][x].blue = (uint8_t)round(fmax(0.0f, fmin(255.0f, valeurBleueFlottante)));
        }
    }

    free(pixelsYUV);
    free(valeursYLumieres);
    free(histogrammeLuminance);
    free(tableTransformationLuminance);

}


