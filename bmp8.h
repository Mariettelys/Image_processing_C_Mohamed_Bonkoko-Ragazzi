
typedef struct {
unsigned char header[54]; /* En tête */
unsigned char colorTable[1024]; /* table de couleur */
unsigned char * data; /*données de l'image sous fprme de tableau*/
unsigned int width; /* largeur de l'image en pixels (offset 18) */
unsigned int height; /* hauteur de l'image en pixels (offset 22)*/
unsigned int colorDepth ; /* profondeur de couleur de l'image (8)*/
unsigned int dataSize; /* taille des données de l'image en octets (offset 28 )*/

} t_bmp8;
