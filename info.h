/* info.h */


#define INFO_GPL_BRIEF "\
Copyright Allan Crossman, 2004\n\
Uses SDL library and Ulf Ekstrom's Bitmask functions\n\
\n\
This program is free software; you can redistribute it\n\
and/or modify it under the terms of the GNU General Public\n\
License as published by the Free Software Foundation;\n\
either version 2 of the License, or (at your option) any\n\
later version.\n\
\n\
This program is distributed in the hope that it will be\n\
amusing, but WITHOUT ANY WARRANTY; without even the implied\n\
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR\n\
PURPOSE.\n\
\n\
See the GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public\n\
License along with this program; if not, write to the Free\n\
Software Foundation, Inc., 59 Temple Place, Suite 330,\n\
Boston, MA  02111-1307  USA\n\n"


#define INFO_HELP "\
Komi options:\n\
\n\
  Speed:\n\
     --delay <x>        (wait x milliseconds between frames,\n\
                         so lower is faster)\n\
\n\
  Technical options:\n\
     --fastdraw         (experimental)\n\
     --fullscreen\n\
     --hog              (hog CPU, may reduce jerkiness)\n\
     --nosound\n\
     --nostars          (turn the starfield off)\n\
     --path directory/  (directory of data files)\n\
     --resetprefs       (reset prefs and high score)\n\
\n\
  Information:\n\
     --copying          (Komi is under the GNU GPL)\n\
     --help\n\
     --version\n\
\n\
  To play:\n\
     Move Komi with arrow keys, or KP4 and KP6.\n\
     Use Komi's tongue with spacebar.\n\
     Capture all coins and diamonds.\n\n"


void printcopyinfo (void)
{
   fprintf(stdout, "\nKomi the Space Frog %s\n", VERSION);
   fprintf(stdout, "%s", INFO_GPL_BRIEF);
   return;
}

void printhelp (void)
{
   fprintf(stdout, "\nKomi the Space Frog %s\n", VERSION);
   fprintf(stdout, "%s", INFO_HELP);
   return;
}

void printversion (void)
{
   fprintf(stdout, "Komi the Space Frog %s\n", VERSION);
   return;
}
