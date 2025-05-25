# Image_processing_C/MOHAMED_BONKOKO--RAGAZZI

Projet C image processing : Le projet C image processing est un projet de traitement d'image en format BMP de 8 bits en niveaux de gris et 24 bits en couleurs.

Instructions de compilation et d'exécution :
Dans un premier temps , pour compiler le projet, il faut être munis d'un compilateur C (nous avons utilisé GCC).
Puis pour exécuter le projet, il faut être munis d'un environnement de développement (nous avons utilisé CLION).
Le programme est à exécuter dans le terminal (bash) à l'aide du fichier principal main.c qui inclut les fichiers header bmp8.h et bmp24.h.

Fonctionnalités implémentées :
1 ) Le traitement d'image en niveaux de gris , 8 bits:
A l'aide d'une structure t_bmp8 représentant une image BMP 8 bits. Cette structure regroupe l'en-tête, la table de couleurs et les données de pixels.
Les fonctionnalités formées à partir de cette structure sont alors :
- Le chargement d'image BMP 8 bits
- La sauvegarde d'image
- L'allocation de mémoire
- La libération de mémoire
- L'affichage d'informations
- L'effet négatif
- L'effet luminosité
- L'effet seuillage
- L'effet égalisation d'histogramme

2 ) Le traitement d'image en couleurs , 24 bits:
A l'aide d'une structure t_bmp24 représentant une image BMP 24 bits.
Cette structure est crée à partir de la structure t_bmp_header et de la structure t_bmp_info qui forment l'en-tête de l'image.
Une autre structure a été très utile pour réaliser ce projet : la structure t_pixel qui représente un pixel en couleur avec ces variations de rouge , vert et bleu.
Les fonctionnalités formées à partir de ces structures sont alors :
- L'allocation de mémoire
- La libération de mémoire
- Le chargement d'image 
- La sauvegarde d'image
- L'affichage d'informations
- L'effet négatif
- L'effet luminosité
- L'effet seuillage
- L'effet égalisation d'histogramme

Les bugs connus :
- Le chargement et la sauvegarde de l'image barbara_gray.bmp fonctionne mal.
Alors que le chargement et la sauvegarde de l'image lena_gray.bmp fonctionne très bien.
- Pour charger l'image flowers_colors.bmp il faut la nommer "../flowers_color.bmp"

- Répartition des tâches :

Anissa :
Implémentation du Menu 
Debeugage 

Partie 1 : Image BMP 8 bits
bmp8_saveImage
bmp8_printInfo
bmp8_brightness
bmp8_applyFilter

Partie 2 : Image BMP 24 bits

readPixelData
bmp24_writePixelValue
bmp24_readPixelValue
bmp24_writePixelData
bmp24_convolution

Partie 3 : Histogramme et Égalisation
bmp8_computeHistogram
bmp8_computeCDF
bmp8_equalize
bmp24_equalize


Tâches de Mariette
Read me 
Debeugage
Commit intermediaire 
Partie 1 : Image BMP 8 bits
bmp8_loadImage
bmp8_free
bmp8_negative
bmp8_threshold

Partie 2 : Image BMP 24 bits
bmp24_saveImage
bmp24_negative
bmp24_grayscale
bmp24_brightness
bmp24_free
bmp24_allocate
bmp24_loadImage
file_rawRead
file_rawWrite
bmp24_threshold

Tâches communes à Mariette et Anissa :
Réalisation de la soutenance et du powerpoint 
bmp8_applyFilter
bmp24_allocateDataPixels
bmp24_freeDataPixels
