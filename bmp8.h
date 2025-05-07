#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


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

// Constantes pour les offsets des champs de l'en-tête BMP
#define BITMAP_MAGIC       0x00 // offset 0
#define BITMAP_SIZE        0x02 // offset 2
#define BITMAP_OFFSET      0x0A // offset 10
#define BITMAP_WIDTH       0x12 // offset 18
#define BITMAP_HEIGHT      0x16 // offset 22
#define BITMAP_DEPTH       0x1C // offset 28
#define BITMAP_SIZE_RAW    0x22 // offset 34
 // Constante pour le type de fichier BMP
 #define BMP_TYPE          0x4D42 // 'BM' en hexadécimal
 // Constantes pour les tailles des champs de l'en-tête BMP
 #define HEADER_SIZE       0x0E // 14 octets
 #define INFO_SIZE         0x28 // 40 octets
 // Constantes pour les valeurs de profondeur de couleur
 #define DEFAULT_DEPTH     0x18 // 24

// Définition d’un pixel RGB (8 bits par canal)
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} t_pixel;

// En-tête BMP (14 octets)
typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} t_bmp_header;

// En-tête d’informations de l’image (40 octets)
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
    t_bmp_header header;
    t_bmp_info header_info;
    int width;
    int height;
    int colorDepth;
    t_pixel **data;  // Matrice 2D de pixels
} t_bmp24;


