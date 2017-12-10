#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "color.h"


// ============================================================================
//                               Initialization
// ============================================================================

Color* newColorWith(int r, int g, int b)
{
  Color* color = (Color*)malloc(sizeof(Color));
  initializeColorWith(color, r, g, b);
  return color;
}

// ============================================================================

void initializeColorWith(Color* color, int r, int g, int b)
{
  float normr = r / 255.0;
  float normg = g / 255.0;
  float normb = b / 255.0;
  
  color->r = r;
  color->g = g;
  color->b = b;
  color->normr = normr;
  color->normg = normg;
  color->normb = normb;
  
  float rgbMax = max(normr, max(normg, normb));
  float rgbMin = min(normr, min(normg, normb));
  float delta = rgbMax - rgbMin;
  
  color->value = rgbMax;
  color->lightness = (rgbMax + rgbMin) / 2;
  color->luminosity = (LUMINOSITY_R * r) +
                      (LUMINOSITY_G * g) +
                      (LUMINOSITY_B * b);
  
  // ----------------------------------------
  // compute HSV and HSL stuff
  if (delta == 0)
  {
    color->hue = 0;
    color->HSVSaturation = 0;
    color->HSLSaturation = 0;
  }
  else
  {
    color->HSLSaturation = delta / (1 - fabs((2 * color->lightness) - 1));
    color->HSVSaturation = delta / rgbMax;
    
    if      (rgbMax == normr)     color->hue = (normg - normb) / delta;
    else if (rgbMax == normr)     color->hue = ((normb - normr) / delta) + 2;
    else  /* rgbMax == normr */   color->hue = ((normr - normg) / delta) + 2;
    
    // hue in degrees
    color->hue *= 60;
    
    if (color->hue < 0)
    {
      color->hue += 360;
    }
  }
  
  // ----------------------------------------
  // "percentize" everything (put in [0, 100])
  color->r *= 100;
  color->g *= 100;
  color->b *= 100;
  color->hue *= 100.0 / 360.0;
  color->HSLSaturation *= 100;
  color->HSVSaturation *= 100;
  color->value *= 100;
  color->lightness *= 100;
  color->luminosity *= 100;
}

// ============================================================================
//                           Color Sorting/Evaluation
// ============================================================================
// Compares two Color pointers
int colorPairComparator(const void* inone, const void* intwo)
{
  ColorPair* one = (ColorPair*)inone;
  ColorPair* two = (ColorPair*)intwo;
  
  if (one->value < two->value) return -1;
  if (one->value > two->value) return 1;
  return 0;
}

// ============================================================================
// Returns the sum of the squared distances between consecutive colors
float colorListRGBError(ColorPair* list, int size)
{
  int r, g, b;
  float total = 0;

  for (int i = 1; i < size; i++)
  {
    r = list[i].color->r - list[i-1].color->r;
    g = list[i].color->g - list[i-1].color->g;
    b = list[i].color->b - list[i-1].color->b;
    total += (r * r) + (g * g) + (b * b);
  }

  return total;
}

// ============================================================================
// Returns the sum of the squared distances between consecutive colors
float colorListHSVError(ColorPair* list, int size)
{
  int h, s, v;
  float total = 0;

  for (int i = 1; i < size; i++)
  {
    h = list[i].color->hue           - list[i-1].color->hue;
    s = list[i].color->HSVSaturation - list[i-1].color->HSVSaturation;
    v = list[i].color->value         - list[i-1].color->value;
    total += (h * h) + (s * s) + (v * v);
  }

  return total;
}

// ============================================================================

float colorListFitness(ColorPair* list, int size)
{
  return colorListRGBError(list, size) / size;
}

// ============================================================================

