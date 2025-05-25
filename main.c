#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"
#include "bmp24.h"


int main() {
    int choix;

    do {
        printf("\n-----------------MENU ---------------\n");
        printf("\nChoisissez le type d'image a traiter \n");
        printf("1. Image en niveaux de gris : lena_gray.bmp\n");
        printf("2. Image couleur : flowers_colors.bmp \n");
        printf("0. Quitter\n");
        printf("Choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1: menu_partie1_et_3_gris(); break;
            case 2: menu_partie2_et_3_couleur(); break;
            case 0: printf("Au revoir .\n"); break;
            default: printf("Choix invalide.\n");
        }
    } while (choix != 0);

    return 0;
}



void menu_partie1_et_3_gris() {
    t_bmp8 *img = bmp8_loadImage("../lena_gray.bmp");
    if (!img) return;

    int choix;
    do {
        printf("1. Informations image\n");
        printf("2. Négatif\n");
        printf("3. Luminosité\n");
        printf("4. Seuillage\n");
        printf("5. Appliquer un filtre (convolution)\n");
        printf("6. Égalisation d'histogramme\n");
        printf("7. Sauvegarder\n");
        printf("0. Retour\n");
        printf("Choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                bmp8_printInfo(img);
                break;
            case 2:
                bmp8_negative(img);
                break;
            case 3: {
                int val;
                printf("Valeur de luminosité (+/-) : ");
                scanf("%d", &val);
                bmp8_brightness(img, val);
                break;
            }
            case 4:
                bmp8_threshold(img);
                break;
            case 5: {
                // Noyaux du PDF
                float box[3][3] = {
                    {1, 1, 1},
                    {1, 1, 1},
                    {1, 1, 1}
                };
                float gaussian[3][3] = {
                    {1, 2, 1},
                    {2, 4, 2},
                    {1, 2, 1}
                };
                float outline[3][3] = {
                    {-1, -1, -1},
                    {-1, 8, -1},
                    {-1, -1, -1}
                };
                float emboss[3][3] = {
                    {-2, -1, 0},
                    {-1, 1, 1},
                    {0, 1, 2}
                };
                float sharpen[3][3] = {
                    {0, -1, 0},
                    {-1, 5, -1},
                    {0, -1, 0}
                };

                float (*selected)[3] = NULL;
                int filtre;
                printf("\nChoisissez un filtre :\n");
                printf("1. Box blur\n2. Gaussian blur\n3. Outline\n4. Emboss\n5. Sharpen\n");
                printf("Choix : ");
                scanf("%d", &filtre);

                switch (filtre) {
                    case 1: selected = box; break;
                    case 2: selected = gaussian; break;
                    case 3: selected = outline; break;
                    case 4: selected = emboss; break;
                    case 5: selected = sharpen; break;
                    default: printf("Filtre invalide.\n"); continue;
                }

                float **kernel = malloc(3 * sizeof(float *));
                for (int i = 0; i < 3; i++) {
                    kernel[i] = malloc(3 * sizeof(float));
                    for (int j = 0; j < 3; j++) {
                        if (filtre == 1)
                            kernel[i][j] = selected[i][j] / 9.0f;
                        else if (filtre == 2)
                            kernel[i][j] = selected[i][j] / 16.0f;
                        else
                            kernel[i][j] = selected[i][j];
                    }
                }

                bmp8_applyFilter(img, kernel, 3);
                printf("Filtre appliqué.\n");

                for (int i = 0; i < 3; i++) free(kernel[i]);
                free(kernel);
                break;
            }
            case 6:
                bmp8_equalize(img, NULL);
                printf("Égalisation appliquée.\n");
                break;
            case 7:
                bmp8_saveImage("output_gris.bmp", img);
                printf("Image sauvegardée.\n");
                break;
            case 0: break;
            default: printf("Choix invalide.\n");
        }
    } while (choix != 0);

    bmp8_free(img);
}


void menu_partie2_et_3_couleur() {
    t_bmp24 *img = bmp24_loadImage("../flowers_color.bmp");
    if (!img) return;

    int choix;
    do {
        printf("1. Négatif\n");
        printf("2. Luminosité\n");
        printf("3. Conversion en niveaux de gris\n");
        printf("4. Seuillage\n");
        printf("5. Égalisation histogramme couleur\n");
        printf("6. Appliquer un filtre (convolution couleur)\n");
        printf("7. Sauvegarder\n");
        printf("0. Retour\n");
        printf("Choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                bmp24_negative(img);
            break;

            case 2: {
                int val;
                printf("Valeur de luminosité (+/-) : ");
                scanf("%d", &val);
                bmp24_brightness(img, val);
                break;
            }

            case 3:
                bmp24_grayscale(img);
            break;
            case 4: {
                int seuil;
                printf("Seuil (0–255) : ");
                scanf("%d", &seuil);
                bmp24_threshold(img, seuil);
                break;
            }
            case 5:
                bmp24_equalize(img);
            break;
            case 6: {
                // Noyaux du PDF
                float box[3][3] = {
                    {1, 1, 1},
                    {1, 1, 1},
                    {1, 1, 1}
                };
                float gaussian[3][3] = {
                    {1, 2, 1},
                    {2, 4, 2},
                    {1, 2, 1}
                };
                float outline[3][3] = {
                    {-1, -1, -1},
                    {-1, 8, -1},
                    {-1, -1, -1}
                };
                float emboss[3][3] = {
                    {-2, -1, 0},
                    {-1, 1, 1},
                    {0, 1, 2}
                };
                float sharpen[3][3] = {
                    {0, -1, 0},
                    {-1, 5, -1},
                    {0, -1, 0}
                };

                float (*selected)[3] = NULL;
                int filtre;
                printf("\nChoisissez un filtre :\n");
                printf("1. Box blur\n2. Gaussian blur\n3. Outline\n4. Emboss\n5. Sharpen\n");
                printf("Choix : ");
                scanf("%d", &filtre);

                switch (filtre) {
                    case 1: selected = box; break;
                    case 2: selected = gaussian; break;
                    case 3: selected = outline; break;
                    case 4: selected = emboss; break;
                    case 5: selected = sharpen; break;
                    default: printf("Filtre invalide.\n"); continue;
                }

                float **kernel = malloc(3 * sizeof(float *));
                for (int i = 0; i < 3; i++) {
                    kernel[i] = malloc(3 * sizeof(float));
                    for (int j = 0; j < 3; j++) {
                        if (filtre == 1)
                            kernel[i][j] = selected[i][j] / 9.0f;
                        else if (filtre == 2)
                            kernel[i][j] = selected[i][j] / 16.0f;
                        else
                            kernel[i][j] = selected[i][j];
                    }
                }

                bmp24_convolution(img, kernel, 3);
                printf("Filtre appliqué.\n");

                for (int i = 0; i < 3; i++) free(kernel[i]);
                free(kernel);
                break;
            }

            case 7:
                bmp24_saveImage(img, "image_finale.bmp");
                printf("Image sauvegardée.\n");
                break;

            case 0: break;
            default: printf("Choix invalide.\n");
        }
    } while (choix != 0);

    bmp24_free(img);
}

