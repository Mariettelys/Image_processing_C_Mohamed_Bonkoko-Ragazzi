
#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"

void  bmp8_saveImage( char filename, *t_bmp8 image){ /* Cette fonction permet d’écrire une image en niveaux de gris dans un fichier BMP dont le nom (chemin) est renseigné par le paramètre filename. Cette fonction prend en paramètre un pointeur vers une image de type t_bmp8 et écrit cette image dans le fichier. Si une erreur survient lors de l’écriture du fichier, la fonction affichera un message d’erreur.
Afin d’écrire cette fonction, documentez-vous sur les fonctions fopen, fwrite, fclose de la bibliothèque standard du langage C.
  */
    File* imagedep = fopen( filename, "wb"); //Ouverture de l'image de départ en mode écriture
    if (f == NULL) { // verifie si l'ouverture a fonctionné
        printf("Erreur d'ouverture");
        return ;
        }

    unsigned int temp; // sert aux vérifications

    //Ecriture de l'entête

    temp = fwrite(image->header, sizeof( unsigned char,54, imagedep );
    if (temp != 54 ){
        printf("Erreur d'ecriture de l'entête");
        fclose(file);
        return ;
    }

    //Ecriture de la table de couleurs

    temp=fwrite(image->colorTable, sizeof(unsigned char , 1024 , imagedep );
    if ( temp != 1024 ) {
        printf("Erreur d'ecriture de la table des couleurs ");
        return ;
        fclose(file);
       }

    // ecriture des données des pixels

    temp = fwrite(image->data, sizeof(unsigned char), image->dataSize, file);
    if (temp != image->dataSize) {
        printf("Erreur lors de l'écriture des données de l'image.");
        fclose(file);
        return ;

    fclose(file);
    return;
    }


};


void bmp8_printInfo( t_bmp8* image ){
     printf("Image Info: ");
     printf("\t Width: %d \n", image->width);
     printf("\t Height: %d \n", image->height);
     printf("\t Color Depth : %d \n", image->colorDepth);
     printf("\t Data Size: %d \n", image->dataSize);

 }


void bmp_brightness(t_bmp8* image, int value){
     for (int i = image->header[10]; i<image->datasize)
         int nvpixel=image->data[i]+value;
         if (nvpixel > 255 ){
             nvpixel = 255;}
         if (nvpixel <0) { nvpixel = 0 }

         image->data[i] = nvpixel;
         }
         return
}

void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
    int n = 1; // taille moitié du noyau
    unsigned int width = img->width;
    unsigned int height = img->height;


    unsigned char *nvdata = (unsigned char *)malloc(img->dataSize);

    for ( int i = 0; i < img->dataSize; i++) {
        nvdata[i] = img->data[i];
    }

    for (int y = 1; y < height - 2; y++) {
        for (int x = 1; x < width - 2; x++) {
            int somme = 0;


            for (int yy = -n; yy <= n; yy++) {

                for (int dx = -n; xx <= n; xx++) {
                    int voisinX = x + xx;
                    int voisinY = y + yy;

                    unsigned char pixelVoisin = img->data[voisinY * width + voisinX];
                    int coeff = kernel[yy + n][xx + n];

                    somme += pixelVoisin * coeff;
                }
            }


            if (somme < 0){ somme = 0;}

            if (somme > 255){ somme = 255;}


            nvdata[y * width + x] = (unsigned char)somme;
        }
    }


    for (int i = 0; i < img->dataSize; i++) {
        img->data[i] = newData[i];
    }
    free(newData);
}



}