/* This file is just an amalgamation of Ulf's bitmask.c and bitmask.h
   files into a single file, simply because I'm too stupid to understand
   the complexities of linking, Makefiles, and so on.
          -- Allan Crossman, May 5, 2004.
   */

/*
 *                     bitmask.c 1.0 (prerelease)
 *                     ------------------------- 
 *    Simple and efficient bitmask collision detection routines
 *  Copyright (C) 2002 Ulf Ekstrom except for the bitcount function.
 *
 *  A bitmask is a simple array of bits, which can be used for 
 *  2d collision detection. Set 'unoccupied' area to zero and
 *  occupies areas to one and use the bitmask_overlap*() functions
 *  to check for collisions.
 *  The current implementation uses 32 bit wide stripes to hold  
 *  the masks, but should work just as well with 64 bit sizes.
 *  (Note that the current bitcount function is 32 bit only!)
 *
 *  The overlap tests uses the following offsets (which may be negative):
 *
 *   +----+----------..
 *   |A   | yoffset   
 *   |  +-+----------..
 *   +--|B        
 *   |xoffset      
 *   |  |
 *   :  :  
 *
 *  For optimal collision detection performance combine these functions
 *  with some kind of pre-sorting to avoid comparing objects far from 
 *  each other.
 *
 *  BUGS: No known bugs, even though they may certainly be in here somewhere.
 *  Possible performance improvements could be to remove the div in 
 *  bitmask_overlap_pos() and to implement wider stripes if the masks used
 *  are wider than 64 bits on the average.
 *
 *  For maximum performance on my machine I use gcc with
 *  -O2 -fomit-frame-pointer -funroll-loops 
 *
 *  
 *  Please email bugs and comments to Ulf Ekstrom, ulfek@ifm.liu.se
 *
 */

/*
 *This program is free software; you can redistribute it and/or
 *modify it under the terms of the GNU General Public License
 *as published by the Free Software Foundation; either version 2
 *of the License, or (at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program; if not, write to the Free Software
 *Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


/* Note by AC: This bit coming up was the original bitmask.h file. */

#ifndef BITMASK_H
#define BITMASK_H

#ifndef INLINE
# ifdef __GNUC__
#  define INLINE __inline__
# else
#  define INLINE __inline
# endif
#endif

#define BITW unsigned long int
#define BITW_LEN 32
#define BITW_MASK 31
#define BITN(n) ((BITW)1 << (n))

typedef struct bitmask
{
  int w,h;
  BITW *bits;
} bitmask;

/* Creates a bitmask of width w and height h.
 * The mask is automatically cleared when created.
 */
bitmask *bitmask_create(int w, int h);
void bitmask_free(bitmask *m);

/* Returns nonzero if the bit at (x,y) is set. 
 * Coordinates start at (0,0)
 */
INLINE int bitmask_getbit(bitmask *m,int x,int y) 
{ 
  return m->bits[x/BITW_LEN*m->h + y] & BITN(x & BITW_MASK); 
}


/* Sets the bit at (x,y) */
INLINE void bitmask_setbit(bitmask *m,int x,int y)
{ 
  m->bits[x/BITW_LEN*m->h + y] |= BITN(x & BITW_MASK); 
}


/* Clears the bit at (x,y) */
INLINE void bitmask_clearbit(bitmask *m,int x,int y)
{ 
  m->bits[x/BITW_LEN*m->h + y] &= ~BITN(x & BITW_MASK); 
}


/* Returns nonzero if the masks overlap with the given offset.
 */
int bitmask_overlap(bitmask *a,bitmask *b,int xoffset, int yoffset);

/* Like bitmask_overlap(), but will also give a point of intersection.
 * x and y are given in the coordinates of mask a, and are untouched if the is no overlap 
 */
int bitmask_overlap_pos(bitmask *a,bitmask *b,int xoffset, int yoffset, int *x, int *y);

/* Returns the number of overlapping 'pixels' */
int bitmask_overlap_area(bitmask *a,bitmask *b,int xoffset, int yoffset);

#endif



/* Note by AC: What was originally bitmask.c follows. */


/*
 *                     bitmask.c 1.0 (prerelease)
 *                     ------------------------- 
 *    Simple and efficient bitmask collision detection routines
 *  Copyright (C) 2002 Ulf Ekstrom except for the bitcount function.
 *
 *           >  See the header file for more info. < 
 *  
 *  Please email bugs and comments to Ulf Ekstrom, ulfek@ifm.liu.se
 *
 */

/*
 *This program is free software; you can redistribute it and/or
 *modify it under the terms of the GNU General Public License
 *as published by the Free Software Foundation; either version 2
 *of the License, or (at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program; if not, write to the Free Software
 *Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <malloc.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))

bitmask *bitmask_create(int w, int h)
{
  bitmask *temp = malloc(sizeof(bitmask));
  if (! temp) return 0;
  temp->w = w;
  temp->h = h;
  temp->bits = calloc(h*((w - 1)/BITW_LEN + 1),sizeof(BITW));
  if (! temp->bits) 
    {
      free(temp);
      return 0;
    }
  else
    return temp;
}

void bitmask_free(bitmask *m)
{
  free(m->bits);
  free(m);
}

int bitmask_overlap(bitmask *a,bitmask *b,int xoffset, int yoffset) /* this requires about 40 % of the time of the old function */
{
  BITW *a_entry,*a_end;
  BITW *b_entry;
  BITW *ap,*bp;
  int shift,rshift,i,astripes,bstripes;
  
  if ((xoffset >= a->w) || (yoffset >= a->h) || (yoffset <= - b->h)) 
      return 0;
  
  if (xoffset >= 0) 
    {
      if (yoffset >= 0)
	{
	  a_entry = a->bits + a->h*(xoffset/BITW_LEN) + yoffset;
	  a_end = a_entry + MIN(b->h,a->h - yoffset);
	  b_entry = b->bits;
	}
      else
	{
	  a_entry = a->bits + a->h*(xoffset/BITW_LEN);
	  a_end = a_entry + MIN(b->h + yoffset,a->h);
	  b_entry = b->bits - yoffset;
	}
      shift = xoffset & BITW_MASK;
      if (shift)
	{
	  rshift = BITW_LEN - shift;
	  astripes = (a->w - 1)/BITW_LEN - xoffset/BITW_LEN;
	  bstripes = (b->w - 1)/BITW_LEN + 1;
	  if (bstripes > astripes) /* zig-zag .. zig*/
	    {
	      for (i=0;i<astripes;i++)
		{
		  for (ap = a_entry,bp = b_entry;ap < a_end;) /* Join these two to one loop */
		      if ((*ap++ >> shift) & *bp++) return 1;
		  a_entry += a->h;
		  a_end += a->h;
		  for (ap = a_entry,bp = b_entry;ap < a_end;)
		      if ((*ap++ << rshift) & *bp++) return 1;
		  b_entry += b->h;
		}
	      for (ap = a_entry,bp = b_entry;ap < a_end;)
		  if ((*ap++ >> shift) & *bp++) return 1;
	      return 0;
	    }
	  else /* zig-zag */
	    {
	      for (i=0;i<bstripes;i++)
		{
		  for (ap = a_entry,bp = b_entry;ap < a_end;)
		    if ((*ap++ >> shift) & *bp++) return 1;
		  a_entry += a->h;
		  a_end += a->h;
		  for (ap = a_entry,bp = b_entry;ap < a_end;)
		      if ((*ap++  << rshift) & *bp++) return 1;
		  b_entry += b->h;
		}
	      return 0;
	    }
	}
      else /* xoffset is a multiple of the stripe width, and the above routines wont work */
	{
	  astripes = (MIN(b->w,a->w - xoffset) - 1)/BITW_LEN + 1;
	  for (i=0;i<astripes;i++)
	    {
	      for (ap = a_entry,bp = b_entry;ap < a_end;)
		if (*ap++ & *bp++) return 1;
	      a_entry += a->h;
	      a_end += a->h;
	      b_entry += b->h;
	    }
	  return 0;
	}
    }
  else  
      return bitmask_overlap(b,a,-xoffset,-yoffset);
}

/* Will hang if there are no bits set in w! */
static INLINE int firstsetbit(BITW w)
{
  int i = 0;
  while ((w & 1) == 0) 
    {
      i++;
      w/=2;
    }
  return i;
}

/* x and y are given in the coordinates of mask a, and are untouched if the is no overlap */
int bitmask_overlap_pos(bitmask *a,bitmask *b,int xoffset, int yoffset, int *x, int *y)
{
  BITW *a_entry,*a_end;
  BITW *b_entry;
  BITW *ap,*bp;
  int shift,rshift,i,astripes,bstripes;
  
  if ((xoffset >= a->w) || (yoffset >= a->h) || (yoffset <= - b->h)) 
      return 0;
  
  if (xoffset >= 0) 
    {
      if (yoffset >= 0)
	{
	  a_entry = a->bits + a->h*(xoffset/BITW_LEN) + yoffset;
	  a_end = a_entry + MIN(b->h,a->h - yoffset);
	  b_entry = b->bits;
	}
      else
	{
	  a_entry = a->bits + a->h*(xoffset/BITW_LEN);
	  a_end = a_entry + MIN(b->h + yoffset,a->h);
	  b_entry = b->bits - yoffset;
	}
      shift = xoffset & BITW_MASK;
      if (shift)
	{
	  rshift = BITW_LEN - shift;
	  astripes = (a->w - 1)/BITW_LEN - xoffset/BITW_LEN;
	  bstripes = (b->w - 1)/BITW_LEN + 1;
	  if (bstripes > astripes) /* zig-zag .. zig*/
	    {
	      for (i=0;i<astripes;i++)
		{
		  for (ap = a_entry,bp = b_entry;ap < a_end;ap++,bp++)
		      if (*ap & (*bp << shift)) 
			{
			  *y = (ap - a->bits) % a->h;
			  *x = ((ap - a->bits) / a->h)*BITW_LEN + firstsetbit(*ap & (*bp << shift));
			  return 1;
			}
		  a_entry += a->h;
		  a_end += a->h;
		  for (ap = a_entry,bp = b_entry;ap < a_end;ap++,bp++)
		      if (*ap & (*bp >> rshift)) 
			{
			  *y = (ap - a->bits) % a->h;
			  *x = ((ap - a->bits) / a->h)*BITW_LEN + firstsetbit(*ap & (*bp >> rshift));
			  return 1;
			}
		  b_entry += b->h;
		}
	      for (ap = a_entry,bp = b_entry;ap < a_end;ap++,bp++)
		if (*ap & (*bp << shift)) 
		  {
		    *y = (ap - a->bits) % a->h;
		    *x = ((ap - a->bits) / a->h)*BITW_LEN + firstsetbit(*ap & (*bp << shift));
		    return 1;
		  }
	      return 0;
	    }
	  else /* zig-zag */
	    {
	      for (i=0;i<bstripes;i++)
		{
		  for (ap = a_entry,bp = b_entry;ap < a_end;ap++,bp++)
		      if (*ap & (*bp << shift)) 
			{
			  *y = (ap - a->bits) % a->h;
			  *x = ((ap - a->bits) / a->h)*BITW_LEN + firstsetbit(*ap & (*bp << shift));
			  return 1;
			}
		  a_entry += a->h;
		  a_end += a->h;
		  for (ap = a_entry,bp = b_entry;ap < a_end;ap++,bp++)
		      if (*ap & (*bp >> rshift)) 
			{
			  *y = (ap - a->bits) % a->h;
			  *x = ((ap - a->bits) / a->h)*BITW_LEN + firstsetbit(*ap & (*bp >> rshift));
			  return 1;
			}
		  b_entry += b->h;
		}
	      return 0;
	    }
	}
      else /* xoffset is a multiple of the stripe width, and the above routines won't work. */
	{
	  astripes = (MIN(b->w,a->w - xoffset) - 1)/BITW_LEN + 1;
	  for (i=0;i<astripes;i++)
	    {
	      for (ap = a_entry,bp = b_entry;ap < a_end;ap++,bp++)
		{
		  if (*ap & *bp) 
		    {
		      *y = (ap - a->bits) % a->h;
		      *x = ((ap - a->bits) / a->h)*BITW_LEN + firstsetbit(*ap & *bp); 
		      return 1;
		    }
		}
	      a_entry += a->h;
	      a_end += a->h;
	      b_entry += b->h;
	    }
	  return 0;
	}
    }
  else  
    {
      if (bitmask_overlap_pos(b,a,-xoffset,-yoffset,x,y))
	{
	  *x *=-1;
	  *y *=-1;
	  return 1;
	}
      else
	return 0;
    }
}



/* (C) Donald W. Gillies, 1992.  All rights reserved.  You may reuse
   this bitcount() function anywhere you please as long as you retain
   this Copyright Notice. */
static INLINE int bitcount(unsigned long n)
{
  register unsigned long tmp;
  return (tmp = (n) - (((n) >> 1) & 033333333333) - (((n) >> 2) & 011111111111),\
	  tmp = ((tmp + (tmp >> 3)) & 030707070707),			\
	  tmp =  (tmp + (tmp >> 6)),					\
	  tmp = (tmp + (tmp >> 12) + (tmp >> 24)) & 077);
}
/* End of Donald W. Gillies bitcount code */


int bitmask_overlap_area(bitmask *a,bitmask *b,int xoffset, int yoffset) /* Runs at approx 60% of time of older function */
{
  BITW *a_entry,*a_end;
  BITW *b_entry;
  BITW *ap,*bp;
  int shift,rshift,i,astripes,bstripes;
  int count = 0;

  if ((xoffset >= a->w) || (yoffset >= a->h) || (yoffset <= - b->h)) 
      return 0;
  
  if (xoffset >= 0) 
    {
      if (yoffset >= 0)
	{
	  a_entry = a->bits + a->h*(xoffset/BITW_LEN) + yoffset;
	  a_end = a_entry + MIN(b->h,a->h - yoffset);
	  b_entry = b->bits;
	}
      else
	{
	  a_entry = a->bits + a->h*(xoffset/BITW_LEN);
	  a_end = a_entry + MIN(b->h + yoffset,a->h);
	  b_entry = b->bits - yoffset;
	}
      shift = xoffset & BITW_MASK;
      if (shift)
	{
	  rshift = BITW_LEN - shift;
	  astripes = (a->w - 1)/BITW_LEN - xoffset/BITW_LEN;
	  bstripes = (b->w - 1)/BITW_LEN + 1;
	  if (bstripes > astripes) /* zig-zag .. zig*/
	    {
	      for (i=0;i<astripes;i++)
		{
		  for (ap = a_entry,bp = b_entry;ap < a_end;ap++,bp++)
		    count += bitcount(((*ap >> shift) | (*(ap + a->h) << rshift)) & *bp);
		  a_entry += a->h;
		  a_end += a->h;
		  b_entry += b->h;
		}
	      for (ap = a_entry,bp = b_entry;ap < a_end;)
		count += bitcount((*ap++ >> shift) & *bp++);
	      return count;
	    }
	  else /* zig-zag */
	    {
	      for (i=0;i<bstripes;i++)
		{
		  for (ap = a_entry,bp = b_entry;ap < a_end;ap++,bp++)
		    count += bitcount(((*ap >> shift) | (*(ap + a->h) << rshift)) & *bp);
		  a_entry += a->h;
		  a_end += a->h;
		  b_entry += b->h;
		}
	      return count;
	    }
	}
      else /* xoffset is a multiple of the stripe width, and the above routines wont work */
	{
	  astripes = (MIN(b->w,a->w - xoffset) - 1)/BITW_LEN + 1;
	  for (i=0;i<astripes;i++)
	    {
	      for (ap = a_entry,bp = b_entry;ap < a_end;)
		count += bitcount(*ap++ & *bp++);

	      a_entry += a->h;
	      a_end += a->h;
	      b_entry += b->h;
	    }
	  return count;
	}
    }
  else  
      return bitmask_overlap_area(b,a,-xoffset,-yoffset);
}



