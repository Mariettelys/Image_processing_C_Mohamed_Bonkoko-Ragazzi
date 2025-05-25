#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>


// Structures

typedef struct {
unsigned char header[54]; /* En tête */
unsigned char colorTable[1024]; /* table de couleur */
unsigned char * data; /*données de l'image sous forme de tableau*/
unsigned int width; /* largeur de l'image en pixels (offset 18) */
unsigned int height; /* hauteur de l'image en pixels (offset 22)*/
unsigned int colorDepth ; /* profondeur de couleur de l'image (8)*/
unsigned int dataSize; /* taille des données de l'image en octets (offset 28 )*/

} t_bmp8;

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

// prototypes-------------------------------------------------------------
//menu
void menu_partie1_et_3_gris();
void menu_partie2_et_3_couleur();

// Partie 1 Image BMP 8 bits

int bmp8_saveImage( const char* filename, t_bmp8 * image);
void bmp8_printInfo(t_bmp8 *image);
void bmp8_brightness(t_bmp8* image, int value);
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize);

// Partie 2 Image BMP 24 bits

t_pixel ** bmp24_allocateDataPixels (int width, int height);
void bmp24_freeDataPixels(t_pixel ** pixels, int height);

t_bmp24 * bmp24_loadImage(const char * filename);
void readPixelData(t_bmp24 * image, FILE * file);
void bmp24_writePixelValue(t_bmp24 * image, int x, int y, FILE * file);
void bmp24_readPixelValue(t_bmp24 * image, int x, int y, FILE * file);
void bmp24_writePixelData(t_bmp24 * image, FILE * file);
void bmp24_convolution(t_bmp24 *img, float **kernel, int kernelSize);


//Partie 3 Histogramme et égalisation-----------------------------------------------
unsigned int * bmp8_computeHistogram(t_bmp8 * img);
unsigned int * bmp8_computeCDF(unsigned int * hist);
void bmp8_equalize(t_bmp8 * img, unsigned int * hist_eq);
void bmp24_equalize(t_bmp24 * img);
