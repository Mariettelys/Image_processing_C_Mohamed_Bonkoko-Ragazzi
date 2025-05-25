
#include "bmp8.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>



// Partie 1-----------------------------------------------------------------

int bmp8_saveImage(const char *filename, t_bmp8 *image) {
    // La fonction sauvegarde une image bmp8
    FILE *imagedep = fopen(filename, "wb"); // Ouverture de l'image de départ en mode écriture binaire
    if (imagedep == NULL) { // Vérifie si l'ouverture a fonctionné
        printf( "Erreur d'ouverture de fichier ");
        return 0;
    }

    size_t temp; // permet la vérification

    // Écriture de l'en-tête
    temp = fwrite(image->header, sizeof(unsigned char), 54, imagedep);
    if (temp != 54) { // verifie que l'ecriture a bien fonctionné
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
    //Affiche les informations liées à l'image bmp
    printf("Image Info:\n");
    printf("\tWidth: %u pixels\n", image->width);
    printf("\tHeight: %u pixels\n", image->height);
    printf("\tColor Depth: %u bits\n", image->colorDepth);
    printf("\tData Size: %u bytes\n", image->dataSize);
}
void bmp8_brightness(t_bmp8 *image, int value) { // la fonction permet d'ajuster la lumiosité d'une image : eclaircir ou foncer , elle permet d'ajouter ou de retirer une valeur à des pixels selon si la
    //si value est negatif on assombri, sinon on eclaircis
    for (unsigned int i = 0; i < image->dataSize; i++) { // parcours chaque pixels
        int nvpixel = image->data[i] + value; //ajoute ou retir à la veleur actuelle du pixel
        // vérifications
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
        printf("Erreur: Paramètres invalides pour bmp8_applyFilter.\n");
        return;
    }

    int n = kernelSize / 2;
    unsigned int width = img->width;
    unsigned int height = img->height;

    unsigned char *nvdata = (unsigned char *)malloc(img->dataSize);
    if (nvdata == NULL) {
        printf("Erreur d'allocation mémoire pour nvdata dans bmp8_applyFilter.\n");
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

            if (somme_ponderee > 255.0f) {
                somme_ponderee = 255.0f;
            } else if (somme_ponderee < 0.0f) {
                somme_ponderee = 0.0f;
            }

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
    unsigned int * hist= bmp8_computeHistogram(img); // creation de l'histogramme
    hist_eq =bmp8_computeCDF(hist); // Calcul de l’histogramme cumulé et normalisation

    for (int i = 0; i < img->dataSize ; i++) {

        img->data[i] = hist_eq[ img->data[i] ]; // remplace la valeur du pixel de l'image de départ par sa nouvelle valeur calculée à partir de la normalisation
    }
    free(hist);
    free(hist_eq);
    return;

}
//Image en couleur 24 bits
void bmp24_equalize(t_bmp24 * img) {
    //cette fonction prend une photo couleur, travaille sur sa luminosité pour la rendre plus équilibrée et avec un meilleur
    //contraste, puis remet les couleurs d'origine avec cette nouvelle luminosité.

    // vérifications
    if (!img || !img->data || img->width <= 0 || img->height <= 0) {
        printf("Erreur : L'image 24 bits fournie est invalide ou vide.\n");
        return;
    }

    unsigned int nombreTotalPixels = img->width * img->height; //calcule le nombre de pixels total


    t_pixel_yuv *pixelsYUV = (t_pixel_yuv *)malloc(nombreTotalPixels * sizeof(t_pixel_yuv)); // création d'un tableau temporaire pour stocker stocker la luminosité/ luminance et les couleurs de chaque pixel au format yuv
    if (pixelsYUV == NULL) {
        printf("Erreur : Impossible d'allouer la mémoire pour les pixels YUV.\n"); //
        return;
    }
    unsigned char *valeursYLumieres = (unsigned char *)malloc(nombreTotalPixels * sizeof(unsigned char)); //  on alloue de la memoire pour un tableau qui permet de stocker uniquement Y la luminescence


    if (pixelsYUV == NULL || valeursYLumieres == NULL) {
        printf("Erreur d'allocation de pixelsYUV OU valeursYlumieres\n"); // on test si les allocations on fonctionnés et on vide les tableaux sinon
        free(pixelsYUV);
        free(valeursYLumieres);
        return;
    }



    for (int y = 0; y < img->height; y++) { //
        for (int x = 0; x < img->width; x++) {
            int indicePixel = y * img->width + x; // on utilise des indices de ligne

            //On extrait les composantes rouge, verte et bleue du pixel situé à la position (x, y) dans l’image,
            //et on les convertit en floats pour passer de rgb à yuv.
            float valeurRouge = img->data[y][x].red;
            float valeurVerte = img->data[y][x].green;
            float valeurBleue = img->data[y][x].blue;


            pixelsYUV[indicePixel].y_comp = 0.299f * valeurRouge + 0.587f * valeurVerte + 0.114f * valeurBleue; //calcule la luminance
            pixelsYUV[indicePixel].u_comp = -0.14713f * valeurRouge - 0.28886f * valeurVerte + 0.436f * valeurBleue; //calcule la chrominance donne la teinte et la saturation
            pixelsYUV[indicePixel].v_comp = 0.615f * valeurRouge - 0.51499f * valeurVerte - 0.10001f * valeurBleue;//calcule la chrominance

            float valeurYlimTemp = round(pixelsYUV[indicePixel].y_comp);

            // On délimite la valeur du pixel entre 0 et 255
            if (valeurYlimTemp < 0.0f) {
                valeurYlimTemp = 0.0f;
            } else if (valeurYlimTemp > 255.0f) {
                valeurYlimTemp = 255.0f;
            }

            // la valeur finale de la luminance est mise dans le tableau des niveaux de luminance après calcul .
            unsigned int valeurYLimiteeArrondie = (unsigned int)valeurYlimTemp;
            valeursYLumieres[indicePixel] = (unsigned char)valeurYLimiteeArrondie;

        }
    }

    t_bmp8 imageLumianceTemporaire; // creation d'une copie de l'image d'origine où les couleurs sont remplacées par le luminace précédemment calculées
    imageLumianceTemporaire.width = img->width;
    imageLumianceTemporaire.height = img->height;
    imageLumianceTemporaire.dataSize = nombreTotalPixels;
    imageLumianceTemporaire.data = valeursYLumieres;


    unsigned int *histogrammeLuminance = bmp8_computeHistogram(&imageLumianceTemporaire); // calcul de l'histogramme compte combien de pixels ont chaque niveau de luminosité.
    if (histogrammeLuminance == NULL) { //verifications
        printf("Erreur d'allocation du tableaux d'histogramme pour la luminance");
        free(pixelsYUV);
        free(valeursYLumieres);
        return;
    }


    // On utilise les comptes l'histogramme pour fabriquer une règle de calcul représentée par la table de transformation.
    // Cette règle va nous dire comment transformer chaque niveau de lumière actuel pour l'egalisation .
    unsigned int *tableTransformationLuminance = bmp8_computeCDF(histogrammeLuminance);
    if (tableTransformationLuminance == NULL) { // teste l'allocation et libere la memoire allouée en cas d'echec
        printf("Erreur : Échec lors du calcul de la CDF et de la table de transformation pour Y.\n");
        free(histogrammeLuminance);
        free(pixelsYUV);
        free(valeursYLumieres);
        return;
    }
    // On parcourt toutes les luminosités de nos pixels.
    for (unsigned int i = 0; i < nombreTotalPixels; i++) {
        // Pour chaque pixel, on calcule sa nouvelle luminosité grace à la table de transformation  .
        pixelsYUV[i].y_comp = (float)tableTransformationLuminance[valeursYLumieres[i]];
    }

    // On  remets les couleurs
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int indicePixel = y * img->width + x;

            // On prend la luminosité qu'on vient de calculer et les informations de couleur U et V d'origine du pixel.
            float luminanceEgalisee = pixelsYUV[indicePixel].y_comp;
            float chrominanceUOriginale = pixelsYUV[indicePixel].u_comp;
            float chrominanceVOriginale = pixelsYUV[indicePixel].v_comp;

            // On calcule les nouvelles valeurs rgb -> reconstruire le pixel original avec les couleurs rouge, verte et bleue mais avec la nouvelle luminosité qu'on a calculée,
            //combinée avec les couleurs pures d'origine
            float valeurRougeFlottante = luminanceEgalisee + 1.13983f * chrominanceVOriginale;
            float valeurVerteFlottante = luminanceEgalisee - 0.39465f * chrominanceUOriginale - 0.58060f * chrominanceVOriginale;
            float valeurBleueFlottante = luminanceEgalisee + 2.03211f * chrominanceUOriginale;

            // On verifie que les nouvelles valeurs des pixels sont comprises entre  0 et 255
            // Pour le Rouge :
            int valeurRougeEntier = round(valeurRougeFlottante); // Arrondir la valeur
            if (valeurRougeEntier < 0) { // Si inférieur à 0, mettre à 0
                valeurRougeEntier = 0;
            }
            if (valeurRougeEntier > 255) { // Si supérieur à 255, mettre à 255
                valeurRougeEntier = 255;
            }
            img->data[y][x].red = (uint8_t)valeurRougeEntier; // on modifie le rouge de l'image originale




            // Pour le Vert :
            int valeurVerteEntier = round(valeurVerteFlottante); // Arrondir la valeur
            if (valeurVerteEntier < 0) { // Si inférieur à 0, mettre à 0
                valeurVerteEntier = 0;
            }
            if (valeurVerteEntier > 255) { // Si supérieur à 255, mettre à 255
                valeurVerteEntier = 255;
            }
            img->data[y][x].green = (uint8_t)valeurVerteEntier; // On modifie le vert de l'image originale





            // Pour le Bleu :
            int valeurBleueEntier = round(valeurBleueFlottante); // Arrondir la valeur
            if (valeurBleueEntier < 0) { // Si inférieure à 0, mettre à 0
                valeurBleueEntier = 0;
            }
            if (valeurBleueEntier > 255) { // Si supérieur à 255, mettre à 255
                valeurBleueEntier = 255;
            }
            img->data[y][x].blue = (uint8_t)valeurBleueEntier; // On modifie le Bleu de l'image originale
        }
    }

    free(pixelsYUV);
    free(valeursYLumieres);
    free(histogrammeLuminance);
    free(tableTransformationLuminance);

}


