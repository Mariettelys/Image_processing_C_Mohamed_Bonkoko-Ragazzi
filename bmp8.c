#include "bmp8.h"
#include "bmp24.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>



// Partie 1-----------------------------------------------------------------

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

    // création d'un tableau pour stocker temporairement les nouvelles valeurs des pixels 
    unsigned char *nvdata = (unsigned char *)malloc(img->dataSize);
    // Erreur d'indentation ici, le if devait englober le printf et le return
    if (nvdata == NULL) {
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

