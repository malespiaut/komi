// sprites.h -- sprite functions
// Copyright Allan Crossman, 2004

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

///////////////////////////////////////////////////////////////////////////////////
//
// Interface to Ulf Ekstrom's bitmask overlap function.
// Do the 2 sprites (or specifically, their collisionmasks) overlap?


int sprite_collision (struct sprite_struct * sprite1, int x1, int y1, struct sprite_struct * sprite2, int x2, int y2)
{
   int leftx1, topy1, leftx2, topy2;

   leftx1 = x1 - sprite1->width / 2;
   topy1 = y1 - sprite1->height / 2;
   
   leftx2 = x2 - sprite2->width / 2;
   topy2 = y2 - sprite2->height / 2;
   
   if (leftx1 > leftx2 + sprite2->width) return 0;
   if (leftx1 + sprite1->width < leftx2) return 0;
   if (topy1 > topy2 + sprite2->height) return 0;
   if (topy1 + sprite1->height < topy2) return 0;
   
   return bitmask_overlap(sprite1->collisionmask, sprite2->collisionmask, leftx2 - leftx1, topy2 - topy1);
}


///////////////////////////////////////////////////////////////////////////////////
//
// Set sprite's width, height and allocate memory for pixels and visibility mask.

void init_spriteimagemap(struct sprite_struct * thesprite, int width, int height)
{
   thesprite->width = width;
   thesprite->height = height;
   thesprite->pixels = (unsigned char *) malloc(thesprite->width * thesprite->height * 3 * sizeof(char));
   thesprite->visible = (unsigned char *) malloc(thesprite->width * thesprite->height * sizeof(char));
   if (thesprite->pixels == NULL || thesprite->visible == NULL)
   {
      printf("   Memory allocation for sprites failed. Quitting.\n");
      cleanexit(1);
   }
   return;
}


///////////////////////////////////////////////////////////////////////////////////
//
// Draws a sprite CENTRED on the coordinates given...

void drawsprite (struct sprite_struct * thesprite, SDL_Surface * screen, int centrex, int centrey)
{
   int x, y;
   int leftx, topy;
   
   leftx = centrex - thesprite->width / 2;
   topy = centrey - thesprite->height / 2;
   
   if (fastdraw)
   {
      updaterectsarray(leftx, topy, thesprite->width, thesprite->height);
   }
   
   for (x = 0; x < thesprite->width; x++)
   {
      for (y = 0; y < thesprite->height; y++)
      {
         if (thesprite->visible[x + (y * thesprite->width)])
	 {
	    setrgb(screen, leftx + x, topy + y, thesprite->pixels[(x * 3) + (y * thesprite->width * 3)], thesprite->pixels[(x * 3) + (y * thesprite->width * 3) + 1], thesprite->pixels[(x * 3) + (y * thesprite->width * 3) + 2]);
	 }
      }
   }
   return;
}


///////////////////////////////////////////////////////////////////////////////////
//
// Like drawsprite, but just draws black pixels...

void clearsprite (struct sprite_struct * thesprite, SDL_Surface * screen, int centrex, int centrey)
{
   int x, y;
   int leftx, topy;
   
   leftx = centrex - thesprite->width / 2;
   topy = centrey - thesprite->height / 2;
   
   if (fastdraw)
   {
      updaterectsarray(leftx, topy, thesprite->width, thesprite->height);
   }
   
   for (x = 0; x < thesprite->width; x++)
   {
      for (y = 0; y < thesprite->height; y++)
      {
         if (thesprite->visible[x + (y * thesprite->width)])
	 {
	    setrgb(screen, leftx + x, topy + y, 0, 0, 0);
	 }
      }
   }
   return;
}


///////////////////////////////////////////////////////////////////////////////////
//
// Load a sprite from a BMP file into an instance of my own sprite structure.
// Set pixels to be transparent if colour matches all 3 of hider,g,b variables.
// ie pass 255, 255, 255 for white pixels in the source to be transparent in game.
//
// Pixels of colour nocollider,g,b will be visible but will not trigger collision.
//
// FIXME: Currently only files with sizes a multiple of 4 will work, due to that silly BMP redundancy thing.

void loadsprite (struct sprite_struct * thesprite, char * directory, char * filename, int width, int height, int hider, int hideb, int hideg, int nocollider, int nocollideg, int nocollideb) {

   FILE * infile;
   int x, y;
   char fullpath[1024];
   
   if (strlen(directory) + strlen(filename) >= sizeof(fullpath))    // Check for buffer overflow on fullpath
   {
      fprintf(stderr, "Fatal error while loading %s:\n", filename);
      fprintf(stderr, "Size of directory name (%d chars) plus size of file name (%d chars)\n", strlen(directory), strlen(filename));
      fprintf(stderr, "is too long (over %d chars), and would cause a buffer overflow...\n", sizeof(fullpath) - 1);
      cleanexit(1);
   }
   strcpy(fullpath, directory);
   strcat(fullpath, filename);
   
   if ((infile = fopen(fullpath, "rb")) == NULL)
   {
      fprintf(stderr, "Fatal error: Failed to open %s\n", fullpath);
      fprintf(stderr, "NOTE: If you need to locate the data directory,\n");
      fprintf(stderr, "try komi --path /foo/bar/komidata/ or whatever,\n");
      fprintf(stderr, "and BE SURE TO use the trailing slash.\n");
      cleanexit(1);
   }
   
   fseek(infile, 0, SEEK_END);
   if (ftell(infile) != 54 + (width * height * 3))
   {
      fprintf(stderr, "Fatal error: Sprite file %s seems to be mis-sized or in wrong format.\n", filename);
      fprintf(stderr, "Expected %d bytes, got %d\n", 54 + (width * height * 3), (int)ftell(infile));
      cleanexit(1);
   }
   
   init_spriteimagemap(thesprite, width, height);
   
   thesprite->collisionmask = bitmask_create(width, height);
   if (thesprite->collisionmask == 0)
   {
      fprintf(stderr, "Fatal error: Failed to create collision mask.\n");
      cleanexit(1);
   }
   
   thesprite->width = width;
   thesprite->height = height;
   
   fseek(infile, 54, SEEK_SET);    // The start of the actual data is at the 55th byte of a BMP file...

   for (y = height - 1; y >= 0; y--)
   {
      for (x = 0; x <= width - 1; x++)
      {
         thesprite->pixels[(x * 3) + (y * width * 3) + 2] = (unsigned char) getc(infile);
	 thesprite->pixels[(x * 3) + (y * width * 3) + 1] = (unsigned char) getc(infile);
	 thesprite->pixels[(x * 3) + (y * width * 3)] = (unsigned char) getc(infile);
	 if (thesprite->pixels[(x * 3) + (y * width * 3)] == hider && thesprite->pixels[(x * 3) + (y * width * 3) + 1] == hideg && thesprite->pixels[(x * 3) + (y * width * 3) + 2] == hideb)
	 {
	    thesprite->visible[x + (y * width)] = 0;
	 } else {
	    thesprite->visible[x + (y * width)] = 1;
	    if (thesprite->pixels[(x * 3) + (y * width * 3)] != nocollider || thesprite->pixels[(x * 3) + (y * width * 3) + 1] != nocollideg || thesprite->pixels[(x * 3) + (y * width * 3) + 2] != nocollideb)
	    {
	       bitmask_setbit(thesprite->collisionmask, x, y);
	    }
	 }
      }
   }
   
   fclose(infile);
   return;
}
