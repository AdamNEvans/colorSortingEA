#ifndef COLOR_H
#define COLOR_H


#include <stdint.h>


// ============================================================================
//                                Constants
// ============================================================================

#define LUMINOSITY_R   0.4
#define LUMINOSITY_G   0.5
#define LUMINOSITY_B   0.1

#define max(A, B)      ((A) > (B) ? (A) : (B))
#define min(A, B)      ((A) < (B) ? (A) : (B))


// ============================================================================
//                                  Types
// ============================================================================

typedef struct Color
{
  // RGB color space
  float r;                 // [0, 100]
  float g;                 // [0, 100]
  float b;                 // [0, 100]
  float normr;             // [0, 1]
  float normg;             // [0, 1]
  float normb;             // [0, 1]
  
  // HSV and HSL color spaces
  float hue;               // [0, 100]; hue in HSV and HSL
  float HSVSaturation;     // [0, 100]; saturation in HSV
  float HSLSaturation;     // [0, 100]; saturation in HSL
  float value;             // [0, 100]; value in HSV
  float lightness;         // [0, 100]; lightness in HSL
  float luminosity;        // [0, 100]; photometric luminosity
} Color;

// ============================================================================

// Used in sorting. Associate a value with a color, then sort by value
typedef struct ColorPair
{
  float value;
  Color* color;
} ColorPair;

// ============================================================================
//                            Function Declarations
// ============================================================================

// initialization
Color* newColorWith(int r, int g, int b);
void initializeColorWith(Color* color, int r, int g, int b);

// color sorting/evaluation
int colorPairComparator(const void* inone, const void* intwo);
float colorListRGBError(ColorPair* list, int size);
float colorListHSVError(ColorPair* list, int size);
float colorListFitness(ColorPair* list, int size);

// ============================================================================



#endif
