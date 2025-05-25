
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
    /* Applique un filtre à une image img  8 bits en niveaux de gris à l'aide du kernel en paramètre  (noyau ) qui correspond à un filtre et de la taille du noyau kernelSize on traite un pixel en fonction des pixels autour  */
    if (!img || !img->data || !kernel || kernelSize <= 0 || kernelSize % 2 == 0) {//Verifications 
        printf("Erreur: Paramètres invalides pour bmp8_applyFilter.\n");
        return;
    }

    int n = kernelSize / 2; // Rayon du pixel, utilisé pour traiter les pixels autour 
    unsigned int width = img->width; 
    unsigned int height = img->height;

    
    unsigned char *nvdata = (unsigned char *)malloc(img->dataSize);// création d'un tableau pour stocker temporairement les nouvelles valeurs des pixels 
        printf("Erreur d'allocation mémoire pour nvdata dans bmp8_applyFilter.\n");
        return;
    }

    // Copie les données originales dans le tableau temporaire pour le traitement
    for (unsigned int i = 0; i < img->dataSize; i++) {
        nvdata[i] = img->data[i];
    }

    // Parcourt les pixels de l'image en évitant les bords 
    for (unsigned int y = n; y < height - n; y++) {
        for (unsigned int x = n; x < width - n; x++) {
            float somme_ponderee = 0.0f; // Initialise  la somme utilisée pour calculer la nouvelle valeur du pixel en fonction du kernel et de ses pixels voisins pour chaque pixel 
            
            // Parcourt les voisins du pixel central selon la taille du noyau
            for (int yy = -n; yy <= n; yy++) {
                for (int xx = -n; xx <= n; xx++) {
                    int voisinX = x + xx; // Coordonnées du pixel voisin
                    int voisinY = y + yy;
                    unsigned char pixelVoisin = img->data[voisinY * width + voisinX]; // recupere la aleur du pixel voisin
                    float coeff = kernel[yy + n][xx + n]; // Coefficient du noyau correspondant
                    somme_ponderee += pixelVoisin * coeff; // Ajoute à somme ponderee la somme des valeurs des pixels voisins à x  modifiée par le coefficient du kernel 
                }
            }

            // limite la valeur entre 0 et 255
            if (somme_ponderee > 255.0f) {
                somme_ponderee = 255.0f;
            } else if (somme_ponderee < 0.0f) {
                somme_ponderee = 0.0f;
            }

            // Affecte la nouvelle valeur arrondie au pixel dans le tableau temporaire nvdata 
            nvdata[y * width + x] = (unsigned char)round(somme_ponderee);
        }
    }

    // Modifie l'image originale 
    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = nvdata[i];
    }

    
    free(nvdata);
    nvdata = NULL;
}


//Partie 3---------------------------------------------------------------

// Images en gris
unsigned int * bmp8_computeHistogram(t_bmp8 * img) {
    /*calcule l'histogramme de l'image le nombre d'apparitions de chaque niveau de gris dans l'image */
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


