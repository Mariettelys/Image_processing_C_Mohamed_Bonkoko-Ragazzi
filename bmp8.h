#include <stdio.h>
#include <stdlib.h>


typedef struct {
    unsigned char header[54]; /* En tête */
    unsigned char colorTable[1024]; /* table de couleur */
    unsigned char * data; /*données de l'image sous forme de tableau*/
    unsigned int width; /* largeur de l'image en pixels (offset 18) */
    unsigned int height; /* hauteur de l'image en pixels (offset 22)*/
    unsigned int colorDepth ; /* profondeur de couleur de l'image (8)*/
    unsigned int dataSize; /* taille des données de l'image en octets (offset 28 )*/

} t_bmp8;

t_bmp8 * bmp8_loadImage( const char * filename) ;

void bmp8_free(t_bmp8 * img);

void bmp8_negative(t_bmp8 * img);

void bmp8_threshold(t_bmp8 * img);

void bmp8_applyFilter(t_bmp8 * img, float ** kernel, int kernelSize);

// Partie 2

typedef struct {
    t_bmp_header header;
    t_bmp_info header_info;
    int width;
    int height;
    int colorDepth;
    t_pixel **data;
} t_bmp24;

typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} t_bmp_header;
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

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} t_pixel;

// Constantes pour les offsets des champs de l'en-tête BMP
#define BITMAP_MAGIC
0x00 // offset 0
#define BITMAP_SIZE
#define BITMAP_OFFSET
#define BITMAP_WIDTH
#define BITMAP_HEIGHT
#define BITMAP_DEPTH
0x02 // offset 2
0x0A // offset 10
0x12 // offset 18
0x16 // offset 22
#define BITMAP_SIZE_RAW
 0x22 // offset 34
 // Constante pour le type de fichier BMP
 #define BMP_TYPE
 0x4D42 // 'BM' en hexadécimal
 // Constantes pour les tailles des champs de l'en-tête BMP
 #define HEADER_SIZE
 #define INFO_SIZE
 0x0E // 14 octets
 0x28 // 40 octets
 // Constantes pour les valeurs de profondeur de couleur
 #define DEFAULT_DEPTH
 0x18 // 24

t_pixel ** bmp24_allocateDataPixels (int width, int height);
void bmp24_freeDataPixels (t_pixel ** pixels, int height);
t_bmp24 * bmp24_allocate (int width, int height, int colorDepth);
void bmp24_free (t_bmp * img);

t_bmp24 * bmp24_loadImage (const char * filename);
void bmp24_saveImage (t_bmp * img, const char * filename);

t_bmp_header header;
file_rawRead(BITMAP_MAGIG, &header, sizeof(t_bmp_header), 1, file);

void bmp24_readPixelValue (t_bmp * image, int x, int y, FILE * file);
void bmp24_readPixelData (t_bmp * image, FILE * file);
void bmp24_writePixelValue (t_bmp * image, int x, int y, FILE * file);
void bmp24_writePixelData (t_bmp * image, FILE * file);
void bmp24_writePixel (t_bmp * image, FILE * file); // fonction à supprimer
