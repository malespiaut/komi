// gfx.h -- some drawing routines for SDL

/*
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to:

       The Free Software Foundation, Inc.
       59 Temple Place - Suite 330
       Boston, MA  02111-1307
       USA
*/

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#define setrgb(screen, x, y, red, green, blue) putpixel(screen, x, y, SDL_MapRGB(screen->format, red, green, blue))

// From Bill Kendrick's GPL game Vectoroids (http://www.newbreedsoftware.com/vectoroids)
// (though something similar exists on the SDL docs site)
// Puts a pixel on the SDL surface. Surface should be locked.

void
putpixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
  int bpp;
  Uint8* p;

  /* Assuming the X/Y values are within the bounds of this surface... */

  if (x >= 0 && y >= 0 && x < surface->w && y < surface->h)
  {
    /* Determine bytes-per-pixel for the surface in question: */

    bpp = surface->format->BytesPerPixel;

    /* Set a pointer to the exact location in memory of the pixel
       in question: */

    p = (((Uint8*)surface->pixels) + /* Start at beginning of RAM */
         (y * surface->pitch) +      /* Go down Y lines */
         (x * bpp));                 /* Go in X pixels */

    /* Set the (correctly-sized) piece of data in the surface's RAM
       to the pixel value sent in: */

    if (bpp == 1)
    {
      *p = pixel;
    }
    else if (bpp == 2)
    {
      *(Uint16*)p = pixel;
    }
    else if (bpp == 3)
    {
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
      {
        p[0] = (pixel >> 16) & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = pixel & 0xff;
      }
      else
      {
        p[0] = pixel & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = (pixel >> 16) & 0xff;
      }
    }
    else if (bpp == 4)
    {
      *(Uint32*)p = pixel;
    }
  }

  return;
}

void
_line_straight(SDL_Surface* bitmapstruct, int x1, int y1, int x2, int y2, int red, int green, int blue)
{
  int pixels;
  int n;

  if (x1 == x2)
  {
    pixels = abs(y1 - y2) + 1;
  }
  else
  {
    pixels = abs(x1 - x2) + 1;
  }

  for (n = 0; n < pixels; n++)
  {
    setrgb(bitmapstruct, x1, y1, red, green, blue);
    if (x2 > x1)
      x1++;
    if (x2 < x1)
      x1--;
    if (y2 > y1)
      y1++;
    if (y2 < y1)
      y1--;
  }
  return;
}

// See http://195.195.128.170/staff/scotty/fcg/fcgline.htm for the ideas behind _line_gentle and _line_steep.

void
_line_gentle(SDL_Surface* bitmapstruct, int x1, int y1, int x2, int y2, int red, int green, int blue)
{
  int error;
  int DXtimes2, DYtimes2;
  int n;
  int additive; // What to add to y1 when error
  int temp;

  if (x1 > x2)
  {
    temp = x1;
    x1 = x2;
    x2 = temp;
    temp = y1;
    y1 = y2;
    y2 = temp;
  }

  error = 0;
  if (y1 < y2)
  {
    additive = 1;
  }
  else
  {
    additive = -1;
  }

  DYtimes2 = abs((y2 - y1) * 2); // Might be going up or down, so abs.
  DXtimes2 = (x2 - x1) * 2;      // But we know we're going right.
  error = x1 - x2;
  for (n = x1; n <= x2; n++)
  {
    setrgb(bitmapstruct, n, y1, red, green, blue);
    error = error + DYtimes2;
    if (error > 0)
    {
      y1 = y1 + additive;
      error = error - DXtimes2;
    }
  }
  return;
}

void
_line_steep(SDL_Surface* bitmapstruct, int x1, int y1, int x2, int y2, int red, int green, int blue)
{
  int error;
  int DXtimes2, DYtimes2;
  int n;
  int additive; // What to add to x1 when error
  int temp;

  if (y1 > y2)
  {
    temp = x1;
    x1 = x2;
    x2 = temp;
    temp = y1;
    y1 = y2;
    y2 = temp;
  }

  error = 0;
  if (x1 < x2)
  {
    additive = 1;
  }
  else
  {
    additive = -1;
  }

  DYtimes2 = (y2 - y1) * 2;      // We know we're going down.
  DXtimes2 = abs((x2 - x1) * 2); // But we might be going left or right, so abs.

  error = y1 - y2;
  for (n = y1; n <= y2; n++)
  {
    setrgb(bitmapstruct, x1, n, red, green, blue);
    error = error + DXtimes2;
    if (error > 0)
    {
      x1 = x1 + additive;
      error = error - DYtimes2;
    }
  }
  return;
}

void
line(SDL_Surface* bitmapstruct, int x1, int y1, int x2, int y2, int red, int green, int blue)
{

  // Lock the surface before calling.

  if (x1 == x2 || y1 == y2 || abs(x1 - x2) == abs(y1 - y2))
  {
    _line_straight(bitmapstruct, x1, y1, x2, y2, red, green, blue);
    return;
  }
  if ((float)(fabs((float)(y2 - y1) / (float)(x2 - x1))) < 1)
  {
    _line_gentle(bitmapstruct, x1, y1, x2, y2, red, green, blue);
  }
  else
  {
    _line_steep(bitmapstruct, x1, y1, x2, y2, red, green, blue);
  }
  return;
}

void
frect(SDL_Surface* bitmapstruct, int x1, int y1, int x2, int y2, int red, int green, int blue)
{
  // This should be safe for unlocked surfaces, assuming SDL_FillRect() is safe...

  SDL_Rect therect;

  if ((x2 <= x1) || (y2 <= y1))
  {
    fprintf(stderr, "Frect: Failed because of coordinate inversion.\n");
    return;
  }

  therect.x = x1;
  therect.y = y1;
  therect.w = x2 - x1;
  therect.h = y2 - y1;

  SDL_FillRect(bitmapstruct, &therect, SDL_MapRGB(bitmapstruct->format, red, green, blue));

  return;
}

void
cls(SDL_Surface* bitmapstruct, int red, int green, int blue)
{
  SDL_FillRect(bitmapstruct, NULL, SDL_MapRGB(bitmapstruct->format, red, green, blue));
  return;
}
