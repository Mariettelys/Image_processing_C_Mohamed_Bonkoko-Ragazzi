
#include "bmp24.h"
#include "bmp8.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>



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

    fseek(file, image->header.offset, SEEK_SET);

    // Calcule le nombre total de pixels
    unsigned int nbPixels = (unsigned int)(image->width * image->height);

    // Lit toutes les données pixels d'un coup (largeur * hauteur pixels)
    unsigned int elementsLus = (unsigned int)fread(image->data, sizeof(t_pixel), nbPixels, file);

    if (elementsLus != nbPixels) { // verification
        printf("Erreur : lecture incomplète des pixels (lu %u, attendu %u).\n", elementsLus, nbPixels);
        return;
        }
    }

void bmp24_writePixelValue(t_bmp24 *image, int x, int y, FILE *file) {
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
        printf("Erreur : Coordonnées (%d, %d) hors limites pour l'image (largeur %d, hauteur %d).\n", x, y, image->width, image->height);
        return;
    }

    // Calcul  de la position du pixel
    long position = image->header.offset + ((long)(image->height - 1 - y) * image->width + x) * sizeof(t_pixel);

    fseek(file, position, SEEK_SET); // déplace le curseur d'écriture dans le fichier vers la position du pixel  avant de l'écrire
    unsigned int elementsEcrits = fwrite(&(image->data[y][x]), sizeof(t_pixel), 1, file);

    if (elementsEcrits != 1) {
        printf("Erreur : Échec de l'écriture d'un pixel ");
    }
}

void bmp24_writePixelData(t_bmp24 *image, FILE *file) {
    // Vérification des paramètres
    if (image == NULL) {
        printf("Erreur : image est NULL.\n");
        return;
    }
    if (image->data == NULL) {
        printf("Erreur : les données de l'image sont NULL.\n");
        return;
    }
    if (file == NULL) {
        printf("Erreur : le fichier est NULL.\n");
        return;
    }
    if (image->width <= 0 || image->height <= 0) {
        printf("Erreur : dimensions de l'image invalides.\n");
        return;
    }

    // Taille d'une ligne de pixels en octets
    long tailleLigne = (long)(image->width * sizeof(t_pixel));

    // Parcours des lignes de l'image (de haut vers le bas) pour écrire la ligne 0,haut de l’image en mémoire, on doit la  dplacer à la fin des lignes dans le fichier BMP
    for (int ligne = 0; ligne < image->height; ligne++) {
        // Calcul de la position dans le fichier : les lignes sont stockées de bas en haut dans un BMP
        long position = image->header.offset + ((long)(image->height - 1 - ligne) * tailleLigne); // on inversie l'ordre des lignes grâce à image->height - 1 - ligne

        // Positionnement dans le fichier
        fseek(file, position, SEEK_SET); // déplace le curseur d'ecriture à la position du pixel

        // Écriture d'une ligne complète de pixels
        unsigned int pixelsEcrits = (unsigned int)fwrite(image->data[ligne], sizeof(t_pixel), image->width, file);

        // Vérification que tous les pixels ont bien été écrits
        if (pixelsEcrits != (unsigned int)image->width) {
            printf("Avertissement : échec de l'écriture de la ligne %d.\n", ligne);
            return;
        }
    }
}



void bmp24_convolution(t_bmp24 *img, float **kernel, int kernelSize) {
    //applique un filtre sur une image BMP 24 img , en traitant chaque pixel selon ses voisins à l’aide d’un noyau **kernel  passé en paramètre et de la taille de ce noyeau kernelsize.

    //vérifications
    if (img == NULL) {
        printf("Erreur: l'image est vide.\n");
        return;
    }
    if (img->data == NULL) {
        printf("Erreur: Les donnees de pixels de l'image n'existent pas ");
        return;
    }
    if (kernel == NULL) {
        printf(" Erreur : Pas de kernel ");
        return;
    }
    if (kernelSize <= 0) {
        printf("Erreur: La taille du noyau doit etre superieure a 0 ");
        return;
    }
    if (kernelSize % 2 == 0) {
        printf("Erreur: La taille du noyau doit etre un nombre impair \n");
        return;
    }

    //  vérification de divisibilité par 4
    if (img->width % 4 != 0 || img->height % 4 != 0) {
        printf("Avertissement: La largeur ou la hauteur de l'image n'est pas divisible par 4. Cela peut causer des problemes avec le padding dans le format BMP si non gere.\n");
    }

    int n = kernelSize / 2; //rayon du noyeau, on aurait pu utiliser 1 puisque les kernels fournis sont toujours de taille 3
    unsigned int width = img->width; // largeur et hauteur de l'image en paramètre
    unsigned int height = img->height;

    t_pixel **nvdata = (t_pixel **)malloc(height * sizeof(t_pixel *)); // creer une nouvelle image pour stocker les pixels que l'ont va traiter
    if (nvdata == NULL) {
        printf("Erreur d'allocation memoire pour nvdata (lignes).\n");
        return;
    }
    for (unsigned int y = 0; y < height; y++) {
        nvdata[y] = (t_pixel *)malloc(width * sizeof(t_pixel)); // pour avoir une image complète, il faut allouer des colonnes
        if (nvdata[y] == NULL) { //teste si l'allocation des colonnes a fonctionné et libère la mèmoire alloué en cas d'echec
            printf("Erreur d'allocation memoire pour nvdata (colonnes).\n");
            for (unsigned int prev_y = 0; prev_y < y; prev_y++) { //
                free(nvdata[prev_y]);
            }
            free(nvdata);
            return;
        }
    }

    for (unsigned int y = 0; y < height; y++) { //copie de l'image dans nvdata
        for (unsigned int x = 0; x < width; x++) {
            nvdata[y][x] = img->data[y][x];
        }
    }

    for (unsigned int y = n; y < height - n; y++) { // délimite le parcours des pixels en n'allant pas aux bords de l'image grâce au n (
        for (unsigned int x = n; x < width - n; x++) {  // initialisation et réinitialisation des sommes de couleurs
            float somme_red = 0.0f;
            float somme_green = 0.0f;
            float somme_blue = 0.0f;
            // x et y sont les coordonées du pixel central, xx et yy concernent la zonne de rayon autour du pixel central
            for (int yy = -n; yy <= n; yy++) { // on limite le parcours au rayon de pixel
                for (int xx = -n; xx <= n; xx++) {
                    int voisinX = x + xx; // permets de trouver les coordonées du pixel voisin à celui qu'on traite
                    int voisinY = y + yy;

                    t_pixel voisinPixel = img->data[voisinY][voisinX]; //écupère la couleur du pixel voisin situé aux coordonnées (voisinX, voisinY) dans l’image bmp 24 fournie en parametre.
                    float coeff = kernel[yy + n][xx + n]; // on prend le nombre dans le kernel  qui correspond à la position actuelle du pixel voisin, pour l’utiliser comme coefficient  dans le calcul de la valeur du  nouveau pixel.

                    somme_red += voisinPixel.red * coeff; // on modifie la valeur des 3 composantes rgb en fonction du coefficient trouvé
                    somme_green += voisinPixel.green * coeff;
                    somme_blue += voisinPixel.blue * coeff;
                }
            }

            float val;
            // on limite les valeurs de rgb entre 0 et 255  puis on ajoute la nouvelle valeur du pixel dans le tableau temporaire nvdata
            // Pour le rouge
            val = round(somme_red);
            if (val < 0.0f) val = 0.0f;
            if (val > 255.0f) val = 255.0f;
            nvdata[y][x].red = (unsigned int)val;

            // Pour le verte
            val = round(somme_green);
            if (val < 0.0f) val = 0.0f;
            if (val > 255.0f) val = 255.0f;
            nvdata[y][x].green = (unsigned int)val;

            // Pour le bleu
            val = round(somme_blue);
            if (val < 0.0f) val = 0.0f;
            if (val > 255.0f) val = 255.0f;
            nvdata[y][x].blue = (unsigned int)val;
                    }
                }

    for (unsigned int y = 0; y < height; y++) { // on applique les modification à l'image originale
        for (unsigned int x = 0; x < width; x++) {
            img->data[y][x] = nvdata[y][x];
        }
    }

    for (unsigned int y = 0; y < height; y++) { // on libère la mémoire allouée au tableau temporaire
        free(nvdata[y]);
    }
    free(nvdata);
    nvdata = NULL;
}
