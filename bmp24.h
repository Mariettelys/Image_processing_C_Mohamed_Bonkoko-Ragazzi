#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>


typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} t_pixel;
// En-tête BMP24
typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} t_bmp_header;
//informations de l’image
typedef struct {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bits;
    uint32_t compression;
    uint32_t imagesize;
    int32_t xresolution;
    int32_t yresolution;
    uint32_t ncolors;
    uint32_t importantcolors;
} t_bmp_info;
// Image BMP 24 bits
typedef struct {
    t_bmp_header header; // En-tête
    t_bmp_info header_info; //les informations de l'image
    int width;  // Largeur de l'image
    int height; // Hauteur de l'image
    int colorDepth; // Profondeur de couleur de l'image
    t_pixel **data;  // données de l'image
} t_bmp24;

typedef struct {
    float y_comp;    // luminance
    float u_comp;    // chrominance
    float v_comp;    // chrominance
} t_pixel_yuv;

// Constantes pour les offsets des champs de l'en-tête BMP
#define BITMAP_MAGIC 0x00 // offset 0
#define BITMAP_SIZE 0x02 // offset 2
#define BITMAP_OFFSET 0x0A // offset 10
#define BITMAP_WIDTH 0x12 // offset 18
#define BITMAP_HEIGHT 0x16 // offset 22
#define BITMAP_DEPTH 0x1C // offset 28
#define BITMAP_SIZE_RAW 0x22 // offset 34
#define BMP_TYPE 0x4D42 // 'BM' en hexadécimal
//
#define HEADER_SIZE 0x0E // 14 octets
#define INFO_SIZE 0x28 // 40 octets
// Constantes pour les valeurs de profondeur de couleur
#define DEFAULT_DEPTH 0x18 // 24

//-------Prototypes--------------------------------

t_pixel ** bmp24_allocateDataPixels (int width, int height);
void bmp24_freeDataPixels(t_pixel ** pixels, int height);

// Fonction qui réserve toute la mémoire nécessaire pour une image couleur, y compris son en-tête et ses pixels.
t_bmp24 * bmp24_allocate (int width, int height, int colorDepth);
// Fonction qui libère la mémoire qui avait été réservée pour une image couleur
void bmp24_free(t_bmp24 * img);


// Lecture et écriture d'image 24 bits

//Fonction qui ouvre un fichier BMP couleur, lit toutes ses informations (en-têtes et pixels) et crée l'image en mémoire
t_bmp24 * bmp24_loadImage (const char * filename);
// Fonction qui prend une image couleur de la mémoire et l'enregistre entièrement dans un nouveau fichier BMP
void bmp24_saveImage (t_bmp24 * img, const char * filename);


// Lecture et écriture dans un fichier
void file_rawRead (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file);
void file_rawWrite (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file);


// Lecture et écriture des données de l'image

// Fonction qui lit les couleurs de chaque pixel depuis un fichier BMP et les met dans la mémoire de l'image
void bmp24_readPixelValue (t_bmp24 * image, int x, int y, FILE * file);
// Fonction qui
void bmp24_readPixelData (t_bmp24 * image, FILE * file);
// Fonction qui écrit les couleurs d'un seul pixel à un endroit précis dans le fichier BMP
void bmp24_writePixelValue (t_bmp24 * image, int x, int y, FILE * file);
// Fonction qui écrit toutes les couleurs de tous les pixels de l'image dans le fichier BMP
void bmp24_writePixelData (t_bmp24 * image, FILE * file);

// Fonctionnalités traitement d'image 24 bits

// Fonction qui inverse toutes les couleurs de l'image, ( rouge -> bleu clair/cyan, vert -> magenta, bleu -> jaune)
void bmp24_negative(t_bmp24 * img);
// Fonction qui convertit une image couleur en une version en niveaux de gris, où chaque pixel devient une nuance de gris allant du noir au blanc
void bmp24_grayscale (t_bmp24 * img);
// Fonction qui rend l'image plus claire ou plus sombre
void bmp24_brightness (t_bmp24 * img, int value);

void bmp24_convolution(t_bmp24 *img, float **kernel, int kernelSize);

// Bonus
// Fonction qui transforme l'image couleur en une image binaire ( que du noir ou blanc )
void bmp24_threshold(t_bmp24 *img, int threshold);

