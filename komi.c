/*
    Komi...
    Copyright Allan Crossman, 2004.


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


// Hmm, so you're reading the code?
// This will only show you how terrible a programmer I am. :-/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <time.h>

// SDL...

#include <SDL.h>
#include <SDL_mixer.h>

// Bitmask...

#include "bitmask.h"

// Komi-specific files...

#include "declarations.h"    // Definitions, globals, etc...
#include "prototypes.h"

#include "gfx.h"

#include "sprites.h"
#include "loaders.h"
#include "info.h"
#include "keys.h"            // Edit that file and recompile to change keys...

///////////////////////////////////////////////////////////////////////////////////

int main (int argc, char * argv[]) {

   int n;

   for (n = 1; n < argc; n++)
   {
      if (strcmp(argv[n], "--fullspeed") == 0)
      {
         fullspeedflag = 1;
         delay = 1;            // But this will not be used in the main loop.
      }
      if (strcmp(argv[n], "--superfast") == 0)
      {
         delay = 4;
      }
      if (strcmp(argv[n], "--fast") == 0)
      {
         delay = 8;
      }
      if (strcmp(argv[n], "--medium") == 0)
      {
         delay = 12;
      }
      if (strcmp(argv[n], "--slow") == 0)
      {
         delay = 16;
      }
      if (strcmp(argv[n], "--glacial") == 0)
      {
         delay = 20;
      }
      if (strcmp(argv[n], "--nostars") == 0)
      {
         nostarsflag = 1;
      }
      if (strcmp(argv[n], "--fullscreen") == 0)
      {
         fullscreen = 1;
      }
      if (strcmp(argv[n], "--nosound") == 0 || strcmp(argv[n], "--nosounds") == 0)
      {
         nosound = 1;
      }
      if (strcmp(argv[n], "--fastdraw") == 0)
      {
         fastdraw = 1;
         nostarsflag = 1;
      }
      if (strcmp(argv[n], "--hog") == 0)
      {
         hog = 1;
      }
      if (strcmp(argv[n], "--algorithmic") == 0)
      {
         algorithmicenemies = 1;
      }
      if (strcmp(argv[n], "--cheats") == 0 || strcmp(argv[n], "--cheat") == 0)
      {
         cheats = 1;
      }
      if (strcmp(argv[n], "--invincible") == 0)
      {
         invincible = 1;
      }
      if (strcmp(argv[n], "--help") == 0 || strcmp(argv[n], "-h") == 0 || strcmp(argv[n], "--usage") == 0)
      {
         printhelp();
         exit(0);
      }
      if (strcmp(argv[n], "--version") == 0 || strcmp(argv[n], "-v") == 0)
      {
         printversion();
         exit(0);
      }
      if (strcmp(argv[n], "--copying") == 0 || strcmp(argv[n], "-v") == 0)
      {
         printcopyinfo();
         exit(0);
      }
      if (strcmp(argv[n], "--path") == 0 && n < argc - 1)
      {
         sprintf(filepath, "%s", argv[n + 1]);
         if (filepath[strlen(filepath) - 1] != '/')
         {
            fprintf(stderr, "Warning: Directory name given did not end with a '/'\n");
            fprintf(stderr, "Are you sure you've passed the full pathname?\n");
         }
      }
   }
   
   if (nosound == 0)
   {
      if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
      {
         fprintf(stderr, "\nFatal error: Unable to initialize SDL!\n\n");
         exit(1);
      }
   } else {
      if (SDL_Init(SDL_INIT_VIDEO) < 0)
      {
         fprintf(stderr, "\nFatal error: Unable to initialize SDL!\n\n");
         exit(1);
      }
   }
   
   if (nosound == 0)
   {
      if (Mix_OpenAudio(44100, AUDIO_S16, 2, 512))
      {
         fprintf(stderr, "Unable to open audio. %s\n", SDL_GetError());
         nosound = 1;
      } else {
         loadsounds();
      }
   }
   
   if (fullscreen == 0)
   {
      virtue = SDL_SetVideoMode(WIDTH, HEIGHT, 0, SDL_ANYFORMAT | SDL_SWSURFACE);
      SDL_WM_SetCaption("Komi " VERSION, NULL);
   } else {
      virtue = SDL_SetVideoMode(WIDTH, HEIGHT, 0, SDL_ANYFORMAT | SDL_FULLSCREEN);
   }
   
   srand((int)time(NULL));
   
   for (n = 0; n < MAXSTARS; n++)
   {
      star[n].speed = 0;
   }
   
   loadsprites();

   menu();

   cleanexit(0);
   return 0;      // Will never get here.
}

///////////////////////////////////////////////////////////////////////////////////

void menu (void)
{
   drawmenu();
   while (1)
   {
      keymap.escape = 0;      // So we don't respond to escape pressed in game.
      mousemap.button = 0;    // Likewise for the mouse.
      
      manageevents();
      
      if (keymap.escape)
      {
         cleanexit(0);
      }
      if (mousemap.button)
      {
         if (abs(mousemap.clickx - STARTBUTTON_X) < start_title.width / 2 && abs(mousemap.clicky - STARTBUTTON_Y) < start_title.height / 2)
         {
            SDL_ShowCursor(SDL_DISABLE);
            game();
            SDL_ShowCursor(SDL_ENABLE);
            SDL_WM_SetCaption("Komi " VERSION, NULL);
            drawmenu();
         } else if (abs(mousemap.clickx - QUITBUTTON_X) < quit_title.width / 2 && abs(mousemap.clicky - QUITBUTTON_Y) < quit_title.height / 2) {
            cleanexit(0);
         }
      }
      SDL_Delay(1);
   }
}

///////////////////////////////////////////////////////////////////////////////////

void game (void)
{
   int gameoverflag;
   
   level = 1;
   lives = START_LIVES;
   komispeedx = KOMI_SPEED;
   fastretract = 0;
   score = 0;
   resetmoney = 1;
   
   gameoverflag = 0;
   while (gameoverflag == 0)
   {
      switch (playlevel())
      {
      case LEVEL_COMPLETE :
         level++;
         resetmoney = 1;
         break;
      case DEATH :
         lives--;
         if (lives < 1)
         {
            gameoverflag = 1;
         }
         resetmoney = 0;
         break;
      case QUIT :
         gameoverflag = 1;
         break;
      default :
         fprintf(stderr, "Fatal error: playlevel() returned an unexpected value.\n\n");
         cleanexit(1);
      }
      
      manageevents();
      if (keymap.escape)
      {
         gameoverflag = 1;
      }
   }
   return;
}

///////////////////////////////////////////////////////////////////////////////////

int playlevel (void)
{
   int n;
   int notedtime;

   blanklevel();
   choosenumbers();
   makelevel();
   
   komix = WIDTH / 2;
   komiy = HEIGHT - (SPRITE_SIZE / 2);
   
   tonguelength = 0;
   tonguespeed = 0;

   shotsavailable = 0;

   caughttype = 0;
   caughtnumber = -1;
   
   tick = 0;
   
   rects = 0;
   
   keymap.levelskip = 0;
   keymap.pause = 0;
   
   mousemap.button = 0;

   cls(virtue, 0, 0, 0);
   SDL_Flip(virtue);
   updatetitlebar();
   
   for (n = 0; n < MAX_ENEMIES; n++)
   {
      if (enemy[n].exists && enemy[n].type == LASERGUN)
      {
         playsound(-1, laserentry_sound, 0);
         break;
      }
   }
   
   while(1)
   {
      tick++;
      
      if (lightningy >= levelinfo.fastlightningy)
      {
         if (tick % levelinfo.fastlightningcheck == 0)
         {
            lightningy++;
         }
      } else if (tick % lightningcheck == 0) {
         lightningy++;
         if (lightningy >= levelinfo.fastlightningy)
         {
            playsound(-1, lightningwarning_sound, 0);
         }
      }
      
      if (nostarsflag == 0)
      {
         dostars();
      }
         
      if (fullspeedflag == 0)
      {
         if (hog)
         {
            notedtime = SDL_GetTicks();
            while (SDL_GetTicks() - notedtime < delay)
            {
               ;
            }
         } else {
            SDL_Delay(delay);
         }
      }
      
      movesprites();
      drawsprites();
      if (fastdraw)
      {
         SDL_UpdateRects(virtue, rects, updaterect);
      } else {
         SDL_Flip(virtue);
      }
      rects = 0;
      
      if (invincible == 0)
      {
         if (playerdeath())
         {
            SDL_Delay(500);
            if (lives == 1)
            {
               playsound(-1, gameover_sound, 0);
            }
            fadeout();
            return DEATH;
         }
      }
      if (leveldone())
      {
         fadeout();
         return LEVEL_COMPLETE;
      }
      
      manageevents();
      if (keymap.escape)
      {
         return QUIT;
      }
      if (cheats && keymap.levelskip)
      {
         return LEVEL_COMPLETE;
      }
      if (keymap.screenshot)
      {
         screenshot(virtue, filepath, "screenshot.bmp");
         keymap.screenshot = 0;
      }
      if (keymap.pause)
      {
         playsound(-1, pause_sound, 0);
         keymap.pause = 0;
         keymap.left1 = 0;
         keymap.right1 = 0;
         keymap.left2 = 0;
         keymap.right2 = 0;
         keymap.fire = 0;
         while (keymap.pause == 0 &&
                keymap.pause == 0 &&
                keymap.left1 == 0 &&
                keymap.right1 == 0 &&
                keymap.left2 == 0 &&
                keymap.right2 == 0 &&
                keymap.fire == 0)
         {
            manageevents();
            if (keymap.escape)
            {
               return QUIT;
            }
               if (keymap.screenshot)
            {
               screenshot(virtue, filepath, "screenshot.bmp");
               keymap.screenshot = 0;
            }
            SDL_Delay(1);
         }
         keymap.pause = 0;
      }
      
      clearsprites();    // Do this last.
      
   }
}

///////////////////////////////////////////////////////////////////////////////////

void movesprites (void)
{
   int n;
   int i;

   for (n = 0; n < MAX_ENEMIES; n++)
   {
      if (enemy[n].exists && enemy[n].type == SKULL)
      {
         if (enemy[n].x < komix)
         {
            komix -= 1;
         } else if (enemy[n].x > komix) {
            komix += 1;
         }
      }
   }

   if (keymap.left1 || keymap.left2)
   {
      komix = komix - komispeedx;
   }
   if (keymap.right1 || keymap.right2)
   {
      komix = komix + komispeedx;
   }
   
   if (keymap.fire && tonguelength == 0)
   {
      if (shotsavailable > 0)
      {
         addkomishot();
         shotsavailable--;
         keymap.fire = 0;
      } else {
         tonguespeed = TONGUE_SPEED;
      }
   }
   if (tonguespeed > 0 && keymap.fire == 0)
   {
      if (fastretract == 0)
      {
         tonguespeed = TONGUE_SPEED * -1;
      } else {
         tonguespeed = TONGUE_SPEED * -2;
      }
   }
   
   tonguelength = tonguelength + tonguespeed;
   if (tonguelength <= 0)
   {
      tonguelength = 0;
      tonguespeed = 0;
   }
   if (tonguelength > komiy - SPRITE_SIZE / 2)
   {
      if (fastretract == 0)
      {
         tonguespeed = TONGUE_SPEED * -1;
      } else {
         tonguespeed = TONGUE_SPEED * -2;
      }
   }
   
   if (caughttype == 0)
   {
      checktonguepickup();
   }
   
   if (komix < WALL) komix = WALL;
   if (komix > WIDTH - WALL) komix = WIDTH - WALL;
   
   for (n = 0; n < MAX_COINS; n++)
   {
      if (caughttype == COIN && caughtnumber == n)
      {
         coin[n].x = caughtoffsetx + komix;
         coin[n].y = caughtoffsety + komiy - tonguelength;
         if (coin[n].y > komiy - 10 || tonguelength == 0)
         {
            coin[n].exists = 0;
            score += COIN_SCORE;
            updatetitlebar();
            caughttype = 0;
            caughtnumber = -1;
            tonguelength = 0;
            tonguespeed = 0;
            playsound(-1, eat_sound, 0);
         }
      } else {
         coin[n].x += coin[n].speedx;
         coin[n].y += coin[n].speedy;
         if (coin[n].x < WALL)
         {
            coin[n].x = WALL;
            coin[n].speedx = fabs(coin[n].speedx);
         }
         if (coin[n].x > WIDTH - WALL)
         {
            coin[n].x = WIDTH - WALL;
            coin[n].speedx = fabs(coin[n].speedx) * -1;
         }
         if (coin[n].y < SPRITE_SIZE / 2)
         {
            coin[n].y = SPRITE_SIZE / 2;
            coin[n].speedy = fabs(coin[n].speedy);
         }
         if (coin[n].y > lightningy - SPRITE_SIZE / 2)
         {
            coin[n].y = lightningy - SPRITE_SIZE / 2;
            coin[n].speedy = fabs(coin[n].speedy) * -1;
         }
      }
   }
   
   for (n = 0; n < MAX_DIAMONDS; n++)
   {
      if (caughttype == DIAMOND && caughtnumber == n)
      {
         diamond[n].x = caughtoffsetx + komix;
         diamond[n].y = caughtoffsety + komiy - tonguelength;
         if (diamond[n].y > komiy - 10 || tonguelength == 0)
         {
            diamond[n].exists = 0;
            score += DIAMOND_SCORE;
            updatetitlebar();
            caughttype = 0;
            caughtnumber = -1;
            tonguelength = 0;
            tonguespeed = 0;
            playsound(-1, eat_sound, 0);
         }
      } else {
         diamond[n].x += diamond[n].speedx;
         diamond[n].y += diamond[n].speedy;
         if (diamond[n].x < WALL)
         {
            diamond[n].x = WALL;
            diamond[n].speedx = fabs(diamond[n].speedx);
         }
         if (diamond[n].x > WIDTH - WALL)
         {
            diamond[n].x = WIDTH - WALL;
            diamond[n].speedx = fabs(diamond[n].speedx) * -1;
         }
         if (diamond[n].y < SPRITE_SIZE / 2)
         {
            diamond[n].y = SPRITE_SIZE / 2;
            diamond[n].speedy = fabs(diamond[n].speedy);
         }
         if (diamond[n].y > lightningy - SPRITE_SIZE / 2)
         {
            diamond[n].y = lightningy - SPRITE_SIZE / 2;
            diamond[n].speedy = fabs(diamond[n].speedy) * -1;
         }
      }
   }
   
   for (n = 0; n < MAX_ENEMYSHOTS; n++)
   {
      if (enemyshot[n].exists)
      {
         enemyshot[n].x += enemyshot[n].speedx;
         enemyshot[n].y += enemyshot[n].speedy;
         if (enemyshot[n].x < (SPRITE_SIZE / 2) * -1 || enemyshot[n].x > WIDTH + (SPRITE_SIZE / 2) || enemyshot[n].y < (SPRITE_SIZE / 2) * -1 || enemyshot[n].y > HEIGHT + (SPRITE_SIZE / 2))
         {
            enemyshot[n].exists = 0;
         }
      }
   }

   for (n = 0; n < MAX_FRIENDLYSHOTS; n++)
   {
      if (friendlyshot[n].exists)
      {
         friendlyshot[n].x += friendlyshot[n].speedx;
         friendlyshot[n].y += friendlyshot[n].speedy;
         if (friendlyshot[n].x < (SPRITE_SIZE / 2) * -1 || friendlyshot[n].x > WIDTH + (SPRITE_SIZE / 2) || friendlyshot[n].y < (SPRITE_SIZE / 2) * -1 || friendlyshot[n].y > HEIGHT + (SPRITE_SIZE / 2))
         {
            friendlyshot[n].exists = 0;
         }
         
         for (i = 0; i < MAX_ENEMIES; i++)
         {
            if (enemy[i].exists && sprite_collision(spritemap[enemy[i].type], enemy[i].x, enemy[i].y, &friendlyshot_sprite, friendlyshot[n].x, friendlyshot[n].y))
            {
               enemy[i].exists = 0;
               friendlyshot[n].exists = 0;
               playsound(-1, destructorkill_sound, 0);
               break;                             // Don't kill multiple enemies with one shot.
            }
         }
      }
   }
   
   if (goodie.exists)
   {
      if (caughttype == POWERUP)
      {
         goodie.x = caughtoffsetx + komix;
         goodie.y = caughtoffsety + komiy - tonguelength;
         if (goodie.y > komiy - 10 || tonguelength == 0)
         {
            goodie.exists = 0;
            caughttype = 0;
            caughtnumber = -1;
            tonguelength = 0;
            tonguespeed = 0;
            goodieaction(goodie.type);
         }
      } else {
         goodie.x += goodie.speedx;
         goodie.y += goodie.speedy;
         if (goodie.x < SPRITE_SIZE / 2)
         {
            goodie.x = SPRITE_SIZE / 2;
            goodie.speedx = fabs(goodie.speedx);
         }
         if (goodie.x > WIDTH - (SPRITE_SIZE / 2))
         {
            goodie.x = WIDTH - (SPRITE_SIZE / 2);
            goodie.speedx = fabs(goodie.speedx) * -1;
         }
         if (goodie.y < SPRITE_SIZE / 2)     // Don't change speedy in this case, to allow gradual entry into the screen.
         {
            goodie.speedy = fabs(goodie.speedy);
         }
         if (goodie.y > lightningy - (SPRITE_SIZE / 2))
         {
            goodie.y = lightningy - (SPRITE_SIZE / 2);
            goodie.speedy = fabs(goodie.speedy) * -1;
         }
      }
   }
   
   for (n = 0; n < POWERUPTYPES; n++)
   {
      if (goodie.exists == 0 && rnd() < levelinfo.powerup_prob[n] && tick < POWERUPCUTOFFTIME)
      {
         goodie.exists = 1;
         goodie.type = n;
         goodie.x = rnd() * WIDTH;
         goodie.y = 0;
         if (goodie.x > WIDTH / 2)
         {
            goodie.speedx = -2;
         } else {
            goodie.speedx = 2;
         }
         goodie.speedy = 1;
         playsound(-1, powerup_sound, 0);
         break;
      }
   }
   
   for (n = 0; n < MAX_ENEMIES; n++)
   {
      if (enemy[n].exists)
      {
         switch (enemy[n].type)
         {
            case DIVER :
               movediver(n);
               break;
            case SCROLLERLEFT :
               movescroller(n);
               break;
            case SCROLLERRIGHT :
               movescroller(n);
               break;
            case BOUNCER :
               movebouncer(n);
               break;
            case ROAMER :
               moveroamer(n);
               break;
            case BROWNIAN :
               movebrownian(n);
               break;
            case ACCELERATOR :
               moveaccelerator(n);
               break;
            case GUNNER :
               movegunner(n);
               break;
            case ELECTRA :
               moveelectra(n);
               break;
            case LASERGUN :
               movelasergun(n);
               break;
            case SKULL :
               moveskull(n);
               break;
            case DROPPER :
               movedropper(n);
               break;
         }
      }
   }
                  
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void movescroller (int n)
{
   enemy[n].y = lightningy + levelinfo.scrolleroffset * sign(enemy[n].speedx);
   enemy[n].x += enemy[n].speedx;
   if (enemy[n].x > WIDTH + SPRITE_SIZE / 2)
   {
      if (lightningy >= levelinfo.fastlightningy)
      {
         enemy[n].exists = 0;
      } else {
         enemy[n].x = SPRITE_SIZE / -2;
      }
   }
   if (enemy[n].x < SPRITE_SIZE / -2)
   {
      if (lightningy >= levelinfo.fastlightningy)
      {
         enemy[n].exists = 0;
      } else {
         enemy[n].x = WIDTH + SPRITE_SIZE / 2;
      }
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void movediver (int n)
{

   if (enemy[n].intvar >= 1)   // Holds countdown to dive.
   {
      enemy[n].intvar = enemy[n].intvar - 1;
      enemy[n].x += enemy[n].speedx;
      if (enemy[n].x < SPRITE_SIZE / 2)
      {
         enemy[n].x = SPRITE_SIZE / 2;
         enemy[n].speedx = fabs(enemy[n].speedx);
      }
      if (enemy[n].x > WIDTH - (SPRITE_SIZE / 2))
      {
         enemy[n].x = WIDTH - (SPRITE_SIZE / 2);
         enemy[n].speedx = fabs(enemy[n].speedx) * -1;
      }
   } else {       // Either we're in dive, or we've past it and are back at top.
      if (enemy[n].y >= levelinfo.diverhoverlevel)
      {
         enemy[n].y += levelinfo.divespeedy;
         if (enemy[n].x > komix)
         {
            enemy[n].x -= levelinfo.divespeedx;
            if (enemy[n].x < komix) enemy[n].x = komix;
         } else if (enemy[n].x < komix) {
            enemy[n].x += levelinfo.divespeedx;
            if (enemy[n].x > komix) enemy[n].x = komix;
         }
         if (enemy[n].y > HEIGHT + SPRITE_SIZE / 2)
         {
            enemy[n].y = (SPRITE_SIZE / 2) * -1;
         }             
      } else {
         enemy[n].y += 1;
         if (enemy[n].y >= levelinfo.diverhoverlevel)
         {
            enemy[n].intvar = enemy[n].intvar2;   // Reset countdown. Cycle complete.
         }
      }
   }

   return;
}

///////////////////////////////////////////////////////////////////////////////////

void movebouncer (int n)
{
   enemy[n].x += enemy[n].speedx;
   enemy[n].y += enemy[n].speedy;
   if (enemy[n].speedy < enemy[n].floatvar)
   {
      enemy[n].speedy += 0.02;
   }
   if (enemy[n].y > komiy)      // Bounce...
   {
      enemy[n].y = komiy;
      enemy[n].speedy = enemy[n].floatvar * -1;
   }
   if (enemy[n].x < SPRITE_SIZE / 2)
   {
      enemy[n].x = SPRITE_SIZE / 2;
      enemy[n].speedx = fabs(enemy[n].speedx);
   }
   if (enemy[n].x > WIDTH - (SPRITE_SIZE / 2))
   {
      enemy[n].x = WIDTH - (SPRITE_SIZE / 2);
      enemy[n].speedx = fabs(enemy[n].speedx) * -1;
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void moveroamer (int n)
{
   enemy[n].x += enemy[n].speedx;
   enemy[n].y += enemy[n].speedy;
   if (enemy[n].x < SPRITE_SIZE / 2)
   {
      enemy[n].x = SPRITE_SIZE / 2;
      enemy[n].speedx = fabs(enemy[n].speedx);
   }
   if (enemy[n].x > WIDTH - (SPRITE_SIZE / 2))
   {
      enemy[n].x = WIDTH - (SPRITE_SIZE / 2);
      enemy[n].speedx = fabs(enemy[n].speedx) * -1;
   }
   if (enemy[n].y < SPRITE_SIZE / 2)
   {
      enemy[n].y = SPRITE_SIZE / 2;
      enemy[n].speedy = fabs(enemy[n].speedy);
   }
   if (enemy[n].y > HEIGHT - (SPRITE_SIZE / 2))
   {
      enemy[n].y = HEIGHT - (SPRITE_SIZE / 2);
      enemy[n].speedy = fabs(enemy[n].speedy) * -1;
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void movebrownian (int n)
{
   enemy[n].speedx += (rnd() - 0.5) / 2;
   enemy[n].speedy += (rnd() - 0.5) / 2;
   if (fabs(enemy[n].speedx) > levelinfo.brownianmaxspeed)
   {
      enemy[n].speedx = sign(enemy[n].speedx) * levelinfo.brownianmaxspeed;
   }
   if (fabs(enemy[n].speedy) > levelinfo.brownianmaxspeed)
   {
      enemy[n].speedy = sign(enemy[n].speedy) * levelinfo.brownianmaxspeed;
   }
   enemy[n].x += enemy[n].speedx;
   enemy[n].y += enemy[n].speedy;
   if (enemy[n].x < SPRITE_SIZE / 2)
   {
      enemy[n].x = SPRITE_SIZE / 2;
      enemy[n].speedx = fabs(enemy[n].speedx);
   }
   if (enemy[n].x > WIDTH - (SPRITE_SIZE / 2))
   {
      enemy[n].x = WIDTH - (SPRITE_SIZE / 2);
      enemy[n].speedx = fabs(enemy[n].speedx) * -1;
   }
   if (enemy[n].y < SPRITE_SIZE / 2)
   {
      enemy[n].y = SPRITE_SIZE / 2;
      enemy[n].speedy = fabs(enemy[n].speedy);
   }
   if (enemy[n].y > lightningy - (SPRITE_SIZE / 2))
   {
      enemy[n].y = lightningy - (SPRITE_SIZE / 2);
      enemy[n].speedy = fabs(enemy[n].speedy) * -1;
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void moveaccelerator (int n)
{
   if (enemy[n].y >= komiy - tonguelength)
   {
      enemy[n].speedx += (float)sign(komix - enemy[n].x) / (enemy[n].floatvar * 2);
   }
   if (fabs(enemy[n].speedx) > enemy[n].floatvar)
   {
      enemy[n].speedx = enemy[n].floatvar * sign(enemy[n].speedx);
   }
   
   if (levelinfo.guardianaccels)
   {
      enemy[n].y = lightningy + enemy[n].floatvar2;
   }
   
   enemy[n].x += enemy[n].speedx;
   if (enemy[n].x < SPRITE_SIZE / 2)
   {
      enemy[n].x = SPRITE_SIZE / 2;
      enemy[n].speedx = fabs(enemy[n].speedx);
   }
   if (enemy[n].x > WIDTH - (SPRITE_SIZE / 2))
   {
      enemy[n].x = WIDTH - (SPRITE_SIZE / 2);
      enemy[n].speedx = fabs(enemy[n].speedx) * -1;
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void movegunner (int n)
{
   int i;

   enemy[n].x += enemy[n].speedx;
   enemy[n].y += enemy[n].speedy;
   if (enemy[n].x < SPRITE_SIZE / 2)
   {
      enemy[n].x = SPRITE_SIZE / 2;
      enemy[n].speedx = fabs(enemy[n].speedx);
   }
   if (enemy[n].x > WIDTH - (SPRITE_SIZE / 2))
   {
      enemy[n].x = WIDTH - (SPRITE_SIZE / 2);
      enemy[n].speedx = fabs(enemy[n].speedx) * -1;
   }
   if (enemy[n].y < SPRITE_SIZE / 2)
   {
      enemy[n].y = SPRITE_SIZE / 2;
      enemy[n].speedy = fabs(enemy[n].speedy);
   }
   if (enemy[n].y > lightningy - (SPRITE_SIZE / 2))
   {
      enemy[n].y = lightningy - (SPRITE_SIZE / 2);
      enemy[n].speedy = fabs(enemy[n].speedy) * -1;
   }
   if (rnd() < levelinfo.gunnershootprob && tick - enemy[n].intvar > GUNNERRELOADTIME)
   {
      for (i = 0; i < MAX_ENEMYSHOTS; i++)
      {
         if (enemyshot[i].exists == 0)
         {
            enemyshot[i].exists = 1;
            enemyshot[i].x = enemy[n].x;
            enemyshot[i].y = enemy[n].y;
            enemyshot[i].speedx = komix - enemy[n].x;
            enemyshot[i].speedy = komiy - enemy[n].y;
            if (fabs(enemyshot[i].speedx) > levelinfo.enemyshotbasespeed)
            {
               enemyshot[i].speedy = enemyshot[i].speedy / (fabs(enemyshot[i].speedx) / levelinfo.enemyshotbasespeed);
               enemyshot[i].speedx = enemyshot[i].speedx / (fabs(enemyshot[i].speedx) / levelinfo.enemyshotbasespeed);
            }
            if (fabs(enemyshot[i].speedy) > levelinfo.enemyshotbasespeed)
            {
               enemyshot[i].speedx = enemyshot[i].speedx / (fabs(enemyshot[i].speedy) / levelinfo.enemyshotbasespeed);
               enemyshot[i].speedy = enemyshot[i].speedy / (fabs(enemyshot[i].speedy) / levelinfo.enemyshotbasespeed);
            }
            playsound(-1, shoot_sound, 0);
            enemy[n].intvar = tick;
            break;
         }
      }
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void moveelectra (int n)
{
   enemy[n].x += enemy[n].speedx;
   enemy[n].y = lightningy + levelinfo.electraoffset;
   if (enemy[n].x < 40)
   {
      enemy[n].x = 40;
      enemy[n].speedx = fabs(enemy[n].speedx);
   }
   if (enemy[n].x > WIDTH - 40)
   {
      enemy[n].x = WIDTH - 40;
      enemy[n].speedx = fabs(enemy[n].speedx) * -1;
   }
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void movelasergun (int n)
{
   enemy[n].x += enemy[n].speedx;
   enemy[n].y += enemy[n].speedy;
   if (enemy[n].x < SPRITE_SIZE / 2)
   {
      enemy[n].x = SPRITE_SIZE / 2;
      enemy[n].speedx = fabs(enemy[n].speedx);
   }
   if (enemy[n].x > WIDTH - (SPRITE_SIZE / 2))
   {
      enemy[n].x = WIDTH - (SPRITE_SIZE / 2);
      enemy[n].speedx = fabs(enemy[n].speedx) * -1;
   }
   if (enemy[n].y < SPRITE_SIZE / 2)
   {
      enemy[n].y = SPRITE_SIZE / 2;
      enemy[n].speedy = fabs(enemy[n].speedy);
   }
   if (enemy[n].y > lightningy - (SPRITE_SIZE / 2))
   {
      enemy[n].y = lightningy - (SPRITE_SIZE / 2);
      enemy[n].speedy = fabs(enemy[n].speedy) * -1;
   }
   
   if (tick % enemy[n].intvar == enemy[n].intvar - LASERWARNTIME)
   {
      playsound(-1, laserpowerup_sound, 0);
   }
   if (tick % enemy[n].intvar == 0 && tick >= enemy[n].intvar)
   {
      playsound(-1, laser_sound, 0);
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void moveskull (int n)
{
   enemy[n].x += enemy[n].speedx;
   if (enemy[n].x < SPRITE_SIZE / 2)
   {
      enemy[n].x = SPRITE_SIZE / 2;
      enemy[n].speedx = fabs(enemy[n].speedx);
   }
   if (enemy[n].x > WIDTH - (SPRITE_SIZE / 2))
   {
      enemy[n].x = WIDTH - (SPRITE_SIZE / 2);
      enemy[n].speedx = fabs(enemy[n].speedx) * -1;
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void movedropper (int n)
{
   if (enemy[n].speedy > 0)    // In the dive already.
   {
      enemy[n].y += enemy[n].speedy;
      if (enemy[n].y > HEIGHT - (SPRITE_SIZE / 2))
      {
         enemy[n].y = HEIGHT - (SPRITE_SIZE / 2);
         enemy[n].speedy = -1;
      }
   } else if (enemy[n].speedy < 0) {
      enemy[n].y += enemy[n].speedy;
      if (enemy[n].y < lightningy + levelinfo.dropperhoveroffset)
      {
         enemy[n].y = lightningy + levelinfo.dropperhoveroffset;
         enemy[n].speedy = 0;
      }
   } else {
      enemy[n].y = lightningy + levelinfo.dropperhoveroffset;
      if (fabs(komix - enemy[n].x) < 10 && enemy[n].y < komiy)
      {
         if (enemy[n].intvar)       // So he can't dive if he started the level above Komi (intvar initialised as 0).
         {
            enemy[n].speedy = levelinfo.dropperspeedy;
         }
      } else {
         enemy[n].intvar = 1;
      }
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////
   
void drawsprites (void)   /*  everything drawn here needs to be cleared at clearsprites()  */
{
   int n;
   
   // Electricity...
   doelectricity();
   
   // Skull pull...
   for (n = 0; n < MAX_ENEMIES; n++)
   {
      if (enemy[n].exists && enemy[n].type == SKULL)
      {
         drawskullpull(0, n);
      }
   }
   
   // Draw pre-laser
   for (n = 0; n < MAX_ENEMIES; n++)
   {
      if (enemy[n].exists && enemy[n].type == LASERGUN && (tick % enemy[n].intvar) >= (enemy[n].intvar - LASERWARNTIME))
      { 
         frect(virtue, 0, enemy[n].y - (LASERWIDTH / 2), WIDTH, enemy[n].y + (LASERWIDTH / 2), WARNLASER_R, WARNLASER_G, WARNLASER_B);
      }
   }
   
   // Draw laser...
   for (n = 0; n < MAX_ENEMIES; n++)
   {
      if (enemy[n].exists && enemy[n].type == LASERGUN && tick % enemy[n].intvar < LASERDURATION && tick >= enemy[n].intvar)
      {
         frect(virtue, 0, enemy[n].y - (LASERWIDTH / 2), WIDTH, enemy[n].y + (LASERWIDTH / 2), LASER_R, LASER_G, LASER_B);
      }
   }
   
   for (n = 0; n < MAX_ENEMIES; n++)
   {
      if (enemy[n].exists && enemy[n].type == ELECTRA && enemy[n].intvar > n && enemy[enemy[n].intvar].exists)
      {
         doelectrabolts(enemy[n].x, enemy[n].y, enemy[enemy[n].intvar].x);
      }
   }
   
   // Draw tongue...
   if (tonguelength)
   {
      frect(virtue, (komix - TONGUE_WIDTH / 2), komiy - tonguelength, (komix + TONGUE_WIDTH / 2), komiy, TONGUE_R, TONGUE_G, TONGUE_B);
      line(virtue, komix - TONGUE_WIDTH / 2, komiy - tonguelength, komix - TONGUE_WIDTH / 2, komiy, 0, 0, 0);
      line(virtue, komix + (TONGUE_WIDTH / 2) - 1, komiy - tonguelength, komix + (TONGUE_WIDTH / 2) - 1, komiy, 0, 0, 0);
      drawsprite(&tip_sprite, virtue, komix, komiy - tonguelength);
   }

   // Draw Coins...
   for (n = 0; n < MAX_COINS; n++)
   {
      if (coin[n].exists)
      {
         drawsprite(&coin_sprite, virtue, coin[n].x, coin[n].y);
      }
   }
   
   // Draw Diamonds...
   for (n = 0; n < MAX_DIAMONDS; n++)
   {
      if (diamond[n].exists)
      {
         drawsprite(&diamond_sprite, virtue, diamond[n].x, diamond[n].y);
      }
   }
   
   // Draw Powerup...
   if (goodie.exists)
   {
      drawsprite(powerupspritemap[goodie.type], virtue, goodie.x, goodie.y);
   }

   // Draw Friendly Shots...
   for (n = 0; n < MAX_FRIENDLYSHOTS; n++)
   {
      if (friendlyshot[n].exists)
      {
         drawsprite(&friendlyshot_sprite, virtue, friendlyshot[n].x, friendlyshot[n].y);
      }
   }
   
   // Draw Komi...
   if (lightningdeath() == 0)
   {
      if (shotsavailable > 0)
      {
         drawsprite(&shooterkomi_sprite, virtue, komix, komiy);
      } else {
         drawsprite(&komi_sprite, virtue, komix, komiy);
      }
   } else {
      drawsprite(&electrickomi_sprite, virtue, komix, komiy);
   }
   
   // Draw Enemy Shots...
   for (n = 0; n < MAX_ENEMYSHOTS; n++)
   {
      if (enemyshot[n].exists)
      {
         drawsprite(&enemyshot_sprite, virtue, enemyshot[n].x, enemyshot[n].y);
      }
   }
   
   // Draw enemies...
   for (n = 0; n < MAX_ENEMIES; n++)
   {
      if (enemy[n].exists)
      {
         drawsprite(spritemap[enemy[n].type], virtue, enemy[n].x, enemy[n].y);
      }
   }
   
   // Draw generators...
   drawsprite(&generator_sprite, virtue, GEN_X, lightningy);
   drawsprite(&generator_sprite, virtue, WIDTH - GEN_X, lightningy);
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void clearsprites (void)
{  
   int n;
   
   // Clear pre-laser
   for (n = 0; n < MAX_ENEMIES; n++)
   {
      if (enemy[n].exists && enemy[n].type == LASERGUN && (tick % enemy[n].intvar) >= (enemy[n].intvar - LASERWARNTIME))
      {
         frect(virtue, 0, enemy[n].y - (LASERWIDTH / 2), WIDTH, enemy[n].y + (LASERWIDTH / 2), 0, 0, 0);
      }
   }
   
   // Clear Skull pull...
   for (n = 0; n < MAX_ENEMIES; n++)
   {
      if (enemy[n].exists && enemy[n].type == SKULL)
      {
         drawskullpull(1, n);
      }
   }
   
   // Clear laser...
   for (n = 0; n < MAX_ENEMIES; n++)
   {
      if (enemy[n].exists && enemy[n].type == LASERGUN && tick % enemy[n].intvar < LASERDURATION && tick >= enemy[n].intvar)
      {
         frect(virtue, 0, enemy[n].y - (LASERWIDTH / 2), WIDTH, enemy[n].y + (LASERWIDTH / 2), 0, 0, 0);
      }
   }
   
   // Clear tongue...
   if (tonguelength)
   {
      frect(virtue, (komix - TONGUE_WIDTH / 2), komiy - tonguelength, (komix + TONGUE_WIDTH / 2), komiy, 0, 0, 0);
      clearsprite(&tip_sprite, virtue, komix, komiy - tonguelength);
   }
   
   // Clear lightning...
   frect(virtue, 0, (lightningy - LIGHTNINGVARIANCE) - 1, WIDTH, (lightningy + LIGHTNINGVARIANCE) + 1, 0, 0, 0);
   clearsprite(&generator_sprite, virtue, GEN_X, lightningy);
   clearsprite(&generator_sprite, virtue, WIDTH - GEN_X, lightningy);

   for (n = 0; n < MAX_ENEMIES; n++)
   {
      if (enemy[n].exists && enemy[n].type == ELECTRA && enemy[enemy[n].intvar].exists && enemy[n].x < enemy[enemy[n].intvar].x)
      {
         frect(virtue, enemy[n].x - (SPRITE_SIZE / 2), (enemy[n].y - LIGHTNINGVARIANCE) - 1, enemy[enemy[n].intvar].x + (SPRITE_SIZE / 2), enemy[enemy[n].intvar].y + LIGHTNINGVARIANCE + 1, 0, 0, 0);
      }
   }

   // Clear Komi...
   if (lightningdeath() == 0)
   {
      if (shotsavailable > 0)
      {
         clearsprite(&shooterkomi_sprite, virtue, komix, komiy);
      } else {
         clearsprite(&komi_sprite, virtue, komix, komiy);
      }
   } else {
      clearsprite(&electrickomi_sprite, virtue, komix, komiy);
   }
   
   // Clear Coins...
   for (n = 0; n < MAX_COINS; n++)
   {
      if (coin[n].exists)
      {
         clearsprite(&coin_sprite, virtue, coin[n].x, coin[n].y);
      }
   }
   
   // Clear Diamonds...
   for (n = 0; n < MAX_DIAMONDS; n++)
   {
      if (diamond[n].exists)
      {
         clearsprite(&diamond_sprite, virtue, diamond[n].x, diamond[n].y);
      }
   }

   // Clear Powerup...
   if (goodie.exists)
   {
      clearsprite(powerupspritemap[goodie.type], virtue, goodie.x, goodie.y);
   }

   // Clear Enemy Shots...
   for (n = 0; n < MAX_ENEMYSHOTS; n++)
   {
      if (enemyshot[n].exists)
      {
         clearsprite(&enemyshot_sprite, virtue, enemyshot[n].x, enemyshot[n].y);
      }
   }

   // Clear Friendly Shots...
   for (n = 0; n < MAX_FRIENDLYSHOTS; n++)
   {
      if (friendlyshot[n].exists)
      {
         clearsprite(&friendlyshot_sprite, virtue, friendlyshot[n].x, friendlyshot[n].y);
      }
   }

   // Clear enemies...
   for (n = 0; n < MAX_ENEMIES; n++)
   {
      if (enemy[n].exists)
      {
         clearsprite(spritemap[enemy[n].type], virtue, enemy[n].x, enemy[n].y);
      }
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void checktonguepickup (void)
{
   int n;

   for (n = 0; n < MAX_COINS; n++)
   {
      if (coin[n].exists)
      {
         if (sprite_collision(&coin_sprite, coin[n].x, coin[n].y, &pickupmask_sprite, komix, komiy - tonguelength))
         {
            caughttype = COIN;
            caughtnumber = n;
            caughtoffsetx = coin[n].x - komix;
            caughtoffsety = coin[n].y - (komiy - tonguelength);
            playsound(-1, stick_sound, 0);
            if (fastretract == 0)
            {
               tonguespeed = TONGUE_SPEED * -1;
            } else {
               tonguespeed = TONGUE_SPEED * -2;
            }
            return;
         }
      }
   }
   
   for (n = 0; n < MAX_DIAMONDS; n++)
   {
      if (diamond[n].exists)
      {
         if (sprite_collision(&diamond_sprite, diamond[n].x, diamond[n].y, &pickupmask_sprite, komix, komiy - tonguelength))
         {
            caughttype = DIAMOND;
            caughtnumber = n;
            caughtoffsetx = diamond[n].x - komix;
            caughtoffsety = diamond[n].y - (komiy - tonguelength);
            playsound(-1, stick_sound, 0);
            if (fastretract == 0)
            {
               tonguespeed = TONGUE_SPEED * -1;
            } else {
               tonguespeed = TONGUE_SPEED * -2;
            }
            return;
         }
      }
   }
   
   if (goodie.exists)
   {
      if (sprite_collision(powerupspritemap[goodie.type], goodie.x, goodie.y, &pickupmask_sprite, komix, komiy - tonguelength))
      {
         caughttype = POWERUP;
         caughtnumber = 0;
         caughtoffsetx = goodie.x - komix;
         caughtoffsety = goodie.y - (komiy - tonguelength);
         playsound(-1, stick_sound, 0);
         if (fastretract == 0)
         {
            tonguespeed = TONGUE_SPEED * -1;
         } else {
            tonguespeed = TONGUE_SPEED * -2;
         }
         return;
      }
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void blanklevel (void)
{
   int n;
   
   /* Anything that can be set by choosenumbers() needs to be set to a default value here.      */
   /* Also clear all objects (with the exception of coins + diamonds if this isn't a new level. */

   lightningcheck = LIGHTNINGCHECK_DEFAULT;
   lightningy = START_LIGHTNING_Y;
   
   levelinfo.fastlightningy = DEFAULT_FASTLIGHTNINGY;
   levelinfo.fastlightningcheck = FASTLIGHTNINGCHECK_DEFAULT;

   if (resetmoney)
   {
      levelinfo.coins = 0;
      for (n = 0; n < MAX_COINS; n++)
      {
         coin[n].exists = 0;
      }
   
      levelinfo.diamonds = 0;
      levelinfo.topdiamonds = 1;
      for (n = 0; n < MAX_DIAMONDS; n++)
      {
         diamond[n].exists = 0;
      }
   }
   
   goodie.exists = 0;
   
   for (n = 0; n < MAX_ENEMYSHOTS; n++)
   {
      enemyshot[n].exists = 0;
   }
   
   for (n = 0; n < MAX_FRIENDLYSHOTS; n++)
   {
      friendlyshot[n].exists = 0;
   }
   
   for (n = 0; n < ENEMYTYPES; n++)
   {
      levelinfo.enemycount[n] = 0;
   }
   for (n = 0; n < MAX_ENEMIES; n++)
   {
      enemy[n].exists = 0;
   }
   
   levelinfo.scrollerspeedx = 1;
   levelinfo.scrolleroffset = SPRITE_SIZE;
   
   levelinfo.diverhoverlevel = SPRITE_SIZE + SPRITE_SIZE / 4;
   levelinfo.divespeedy = 4;
   levelinfo.divespeedx = 2;
   
   levelinfo.roamerminspeed = 0.5;
   levelinfo.roamermaxspeed = 1;

   levelinfo.brownianmaxspeed = 1;
   
   levelinfo.guardianaccels = 0;
   
   levelinfo.gunnershootprob = 0.01;
   levelinfo.enemyshotbasespeed = 3;
   
   levelinfo.electrasflag = 0;
   levelinfo.electraoffset = -32;
   
   levelinfo.dropperhoveroffset = SPRITE_SIZE;
   levelinfo.dropperspeedy = 5;
   
   for (n = 0; n < POWERUPTYPES; n++)
   {
      levelinfo.powerup_prob[n] = BASEPOWERUPPROB;
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void choosenumbers (void)
{

   int coins;       // Local variables which will only be copied to the globals
   int diamonds;    // if resetmoney is flagged (due to this being a new level).

   // Default values, may be over-ridden in a moment.
   coins = (level / 2.5) + 2;
   diamonds = (level % 2) + 1;
   
   if (algorithmicenemies == 0)
   {
      switch (level)
      {
         case 1 :
            levelinfo.enemycount[BOUNCER] = 1;
            levelinfo.enemycount[SCROLLERLEFT] = 1;
            levelinfo.enemycount[DROPPER] = 2;
            levelinfo.topdiamonds = 0;
            break;
         case 2 :
            levelinfo.enemycount[SCROLLERLEFT] = 1;
            levelinfo.enemycount[SCROLLERRIGHT] = 1;
            levelinfo.enemycount[BOUNCER] = 1;
            levelinfo.enemycount[ROAMER] = 1;
            levelinfo.enemycount[GUNNER] = 1;
            break;
         case 3 :
            levelinfo.enemycount[SCROLLERLEFT] = 1;
            levelinfo.enemycount[SCROLLERRIGHT] = 2;
            levelinfo.enemycount[ROAMER] = 1;
            levelinfo.enemycount[DIVER] = 1;
            levelinfo.enemycount[LASERGUN] = 1;
            break;
         case 4 :
            levelinfo.enemycount[SCROLLERLEFT] = 2;
            levelinfo.enemycount[SCROLLERRIGHT] = 2;
            levelinfo.enemycount[BROWNIAN] = 1;
            levelinfo.enemycount[BOUNCER] = 2;
            break;
         case 5 :
            levelinfo.electrasflag = 1;
            levelinfo.enemycount[DIVER] = 2;
            levelinfo.enemycount[DROPPER] = 3;
            break;
         case 6 :
            levelinfo.enemycount[SCROLLERLEFT] = 2;
            levelinfo.enemycount[SCROLLERRIGHT] = 2;
            levelinfo.enemycount[LASERGUN] = 1;
            levelinfo.enemycount[BROWNIAN] = 4;
            break;
         case 7 :
            levelinfo.enemycount[ROAMER] = 8;
            levelinfo.enemycount[DIVER] = 1;
               coins = 8;
               diamonds = 1;
            break;
         case 8 :
            levelinfo.enemycount[ACCELERATOR] = 3;
            levelinfo.enemycount[BROWNIAN] = 2;
            levelinfo.enemycount[BOUNCER] = 2;
               coins = 0;
               diamonds = 3;
            break;
         case 9 :
            levelinfo.enemycount[GUNNER] = 2;
            levelinfo.enemycount[DIVER] = 4;
            levelinfo.topdiamonds = 0;
               coins = 7;
               diamonds = 2;
            break;
         case 10 :
            levelinfo.electrasflag = 1;
            levelinfo.enemycount[GUNNER] = 3;
            levelinfo.enemycount[SCROLLERRIGHT] = 2;
            levelinfo.enemycount[BROWNIAN] = 2;
            lightningcheck = 40;
            levelinfo.topdiamonds = 0;
            levelinfo.electraoffset = 0;
            break;
         case 11 :
            levelinfo.enemycount[SCROLLERLEFT] = 3;
            levelinfo.enemycount[SCROLLERRIGHT] = 3;
            levelinfo.enemycount[ROAMER] = 3;
            levelinfo.enemycount[BROWNIAN] = 2;
            levelinfo.enemycount[DIVER] = 1;
            lightningcheck = 100000000;
            break;
         case 12 :
            levelinfo.electrasflag = 1;
            levelinfo.enemycount[SCROLLERRIGHT] = 1;
            levelinfo.enemycount[DIVER] = 1;
            levelinfo.enemycount[ROAMER] = 1;
            levelinfo.enemycount[BROWNIAN] = 1;
            levelinfo.enemycount[ACCELERATOR] = 1;
            levelinfo.enemycount[LASERGUN] = 1;
            levelinfo.enemycount[SKULL] = 1;
            levelinfo.enemycount[GUNNER] = 1;
               coins = 5;
               diamonds = 0;
            break;
         case 13 :
            levelinfo.enemycount[BOUNCER] = 2;
            levelinfo.enemycount[GUNNER] = 2;
            levelinfo.enemycount[SCROLLERLEFT] = 2;
            levelinfo.enemycount[SCROLLERRIGHT] = 2;
            levelinfo.enemycount[ROAMER] = 3;
               coins = 5;
               diamonds = 1;
            break;
         case 14 :
            levelinfo.enemycount[DIVER] = 4;
            levelinfo.enemycount[SKULL] = 1;
            levelinfo.enemycount[BOUNCER] = 2;
               coins = 5;
               diamonds = 0;
            break;
         case 15 :
            levelinfo.enemycount[DROPPER] = 4;
            levelinfo.enemycount[GUNNER] = 3;
            levelinfo.gunnershootprob = 0.005;
               coins = 4;
               diamonds = 2;
            break;
         case 16 :
            levelinfo.enemycount[BOUNCER] = 4;
            levelinfo.enemycount[SCROLLERLEFT] = 2;
            levelinfo.enemycount[SCROLLERRIGHT] = 3;
            levelinfo.enemycount[DIVER] = 1;
            lightningcheck = 100000000;
               diamonds = 2;
            break;
         case 18 :
            levelinfo.enemycount[SKULL] = 1;
            levelinfo.enemycount[LASERGUN] = 4;
            levelinfo.enemycount[GUNNER] = 2;
            levelinfo.enemycount[ACCELERATOR] = 1;
            levelinfo.guardianaccels = 1;
            levelinfo.gunnershootprob = 0.005;
            break;
         case 21 :
            levelinfo.enemycount[BOUNCER] = 3;
            levelinfo.enemycount[ROAMER] = 6;
            levelinfo.enemycount[BROWNIAN] = 6;
            levelinfo.enemycount[DIVER] = 1;
            levelinfo.enemycount[SCROLLERLEFT] = 1;
            levelinfo.powerup_prob[SHOOTPOWER] = 1;
            lightningcheck = 100000000;
            break;
         case 22 :
            levelinfo.enemycount[DROPPER] = 5;
            levelinfo.enemycount[GUNNER] = 2;
            lightningcheck = 100000000;
              coins = 6;
              diamonds = 3;
            break;
         case 26 :
            levelinfo.enemycount[SCROLLERLEFT] = 3;
            levelinfo.enemycount[DROPPER] = 4;
            levelinfo.enemycount[ROAMER] = 6;
            levelinfo.enemycount[SKULL] = 1;
            lightningcheck = 100;
               diamonds = 3;
            break;
         default :
            algorithmicenemynumbers(level);
            break;
      }
   } else {
      algorithmicenemynumbers(level);
   }
   
   if (resetmoney)
   {
      levelinfo.coins = coins;
      levelinfo.diamonds = diamonds;
   }
   
   return;
}
   
///////////////////////////////////////////////////////////////////////////////////

void algorithmicenemynumbers (int thislevel)
{

   if (thislevel % 5 == 0) levelinfo.electrasflag = 1;
   if (levelinfo.electrasflag)
   {
      levelinfo.enemycount[SCROLLERRIGHT] = 3;
   } else {
      levelinfo.enemycount[SCROLLERRIGHT] = 2;
      levelinfo.enemycount[SCROLLERLEFT] = 2;
   }
   if (thislevel % 2 == 1 && thislevel >= 14)
   {
      levelinfo.enemycount[ACCELERATOR] = 2;
      if (thislevel % 4 == 1)
      {
         levelinfo.guardianaccels = 1;
         levelinfo.enemycount[SCROLLERRIGHT] = 0;
         levelinfo.enemycount[SCROLLERLEFT] = 0;
      }
   }
   if (thislevel % 2 == 0)
   {
      levelinfo.enemycount[BROWNIAN] = thislevel / 9;
      if (levelinfo.enemycount[BROWNIAN] > 12) levelinfo.enemycount[BROWNIAN] = 12;
   }
   levelinfo.enemycount[ROAMER] = ((thislevel - 2) / 9) + 1 - levelinfo.enemycount[BROWNIAN];
   if (levelinfo.enemycount[ROAMER] < 0) levelinfo.enemycount[ROAMER] = 0;
   if (levelinfo.enemycount[ROAMER] > 12) levelinfo.enemycount[ROAMER] = 12;
   if (thislevel % 3 == 0) levelinfo.enemycount[LASERGUN] = 1;
   if (thislevel % 7 == 0) levelinfo.enemycount[SKULL] = 1;
   if (thislevel % 4 == 0) levelinfo.enemycount[DIVER] = thislevel / 4;
   if (levelinfo.enemycount[DIVER] > 5) levelinfo.enemycount[DIVER] = 5;
   if (thislevel % 4 == 1) levelinfo.enemycount[GUNNER] = 2;
   if (thislevel % 5 == 4) levelinfo.enemycount[BOUNCER] = (thislevel / 10) + 1;
   
   if (thislevel > 10) levelinfo.topdiamonds = 0;
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void makelevel (void)
{
   int n;
   int t;
   float spacing;
   
   for (n = 0; n < levelinfo.coins && n < MAX_COINS; n++)
   {
      if (resetmoney)
      {
         coin[n].exists = 1;
      }
      coin[n].x = (rnd() * (WIDTH - (SPRITE_SIZE * 2))) + SPRITE_SIZE;
      coin[n].y = (rnd() * (START_LIGHTNING_Y - (SPRITE_SIZE * 2))) + SPRITE_SIZE;
      coin[n].speedx = rnd() * 5 - 2.5;
      if (fabs(coin[n].speedx) < 0.5) coin[n].speedx = sign(coin[n].speedx) * 0.5;
      coin[n].speedy = rnd() * 6 - 3;
      if (fabs(coin[n].speedy) < 0.5) coin[n].speedy = sign(coin[n].speedy) * 0.5;
   }

   for (n = 0; n < levelinfo.diamonds && n < MAX_DIAMONDS; n++)
   {
      if (resetmoney)
      {
         diamond[n].exists = 1;
      }
      diamond[n].x = (rnd() * (WIDTH - (SPRITE_SIZE * 2))) + SPRITE_SIZE;
      diamond[n].speedx = rnd() * 5 - 2.5;
      if (fabs(diamond[n].speedx) < 0.5) diamond[n].speedx = sign(diamond[n].speedx) * 0.5;
      if (levelinfo.topdiamonds == 0)
      {
         diamond[n].y = (rnd() * (START_LIGHTNING_Y - (SPRITE_SIZE * 2))) + SPRITE_SIZE;
         diamond[n].speedy = rnd() * 6 - 3;
         if (fabs(diamond[n].speedy) < 0.5) diamond[n].speedy = sign(diamond[n].speedy) * 0.5;
      } else {
         diamond[n].y = SPRITE_SIZE / 2;
         diamond[n].speedy = 0;
      }
   }
   
   // Make the enemies...
   
   for (n = 0; n < levelinfo.enemycount[SCROLLERLEFT]; n++)
   {
      for (t = 0; t < MAX_ENEMIES; t++)
      {
         if (enemy[t].exists == 0)
         {
            enemy[t].exists = 1;
            enemy[t].type = SCROLLERLEFT;
            spacing = (float)(WIDTH + SPRITE_SIZE) / levelinfo.enemycount[SCROLLERLEFT];
            enemy[t].x = ((spacing / 2) + (n * spacing)) - SPRITE_SIZE / 2;
            enemy[t].y = lightningy - levelinfo.scrolleroffset;
            enemy[t].speedx = levelinfo.scrollerspeedx * -1;
            enemy[t].speedy = 0;
            break;
         }
      }
   }

   for (n = 0; n < levelinfo.enemycount[SCROLLERRIGHT]; n++)
   {
      for (t = 0; t < MAX_ENEMIES; t++)
      {
         if (enemy[t].exists == 0)
         {
            enemy[t].exists = 1;
            enemy[t].type = SCROLLERRIGHT;
            spacing = (float)(WIDTH + SPRITE_SIZE) / levelinfo.enemycount[SCROLLERRIGHT];
            enemy[t].x = ((spacing / 2) + (n * spacing)) - SPRITE_SIZE / 2;
            enemy[t].y = lightningy + levelinfo.scrolleroffset;
            enemy[t].speedx = levelinfo.scrollerspeedx;
            enemy[t].speedy = 0;
            break;
         }
      }
   }
   
   for (n = 0; n < levelinfo.enemycount[DROPPER]; n++)
   {
      for (t = 0; t < MAX_ENEMIES; t++)
      {
         if (enemy[t].exists == 0)
         {
            enemy[t].exists = 1;
            enemy[t].type = DROPPER;
            spacing = (float)(WIDTH + SPRITE_SIZE) / levelinfo.enemycount[DROPPER];
            enemy[t].x = ((spacing / 2) + (n * spacing)) - SPRITE_SIZE / 2;
            enemy[t].y = lightningy + levelinfo.dropperhoveroffset;
            enemy[t].speedx = levelinfo.scrollerspeedx;
            enemy[t].speedy = 0;
            enemy[t].intvar = 0;
            break;
         }
      }
   }
   
   for (n = 0; n < levelinfo.enemycount[DIVER]; n++)
   {
      for (t = 0; t < MAX_ENEMIES; t++)
      {
         if (enemy[t].exists == 0)
         {
            enemy[t].exists = 1;
            enemy[t].type = DIVER;
            enemy[t].x = (rnd() * (WIDTH - (SPRITE_SIZE * 2))) + SPRITE_SIZE;
            enemy[t].y = levelinfo.diverhoverlevel;
            enemy[t].speedx = sign(rnd() - 0.5) * 2; if (enemy[t].speedx == 0) enemy[t].speedx = 2;
            enemy[t].speedy = 0;
            enemy[t].intvar = 150 + (n * 40);
            enemy[t].intvar2 = enemy[t].intvar;
            break;
         }
      }
   }

   for (n = 0; n < levelinfo.enemycount[BOUNCER]; n++)
   {
      for (t = 0; t < MAX_ENEMIES; t++)
      {
         if (enemy[t].exists == 0)
         {
            enemy[t].exists = 1;
            enemy[t].type = BOUNCER;
            enemy[t].x = (rnd() * (WIDTH - (SPRITE_SIZE * 2))) + SPRITE_SIZE;
            enemy[t].y = (rnd() * (lightningy - SPRITE_SIZE)) + (SPRITE_SIZE / 2);
            enemy[t].speedx = sign(rnd() - 0.5); if (enemy[t].speedx == 0) enemy[t].speedx = 1;
            enemy[t].floatvar = 2;
            enemy[t].speedy = enemy[t].floatvar;
            break;
         }
      }
   }

   for (n = 0; n < levelinfo.enemycount[ROAMER]; n++)
   {
      for (t = 0; t < MAX_ENEMIES; t++)
      {
         if (enemy[t].exists == 0)
         {
            enemy[t].exists = 1;
            enemy[t].type = ROAMER;
            enemy[t].x = (rnd() * (WIDTH - (SPRITE_SIZE * 2))) + SPRITE_SIZE;
            enemy[t].y = (rnd() * (lightningy - SPRITE_SIZE)) + (SPRITE_SIZE / 2);
            enemy[t].speedx = (rnd() * (levelinfo.roamermaxspeed - levelinfo.roamerminspeed)) + levelinfo.roamerminspeed;
            if (rnd() < 0.5) enemy[t].speedx *= -1;
            enemy[t].speedy = (rnd() * (levelinfo.roamermaxspeed - levelinfo.roamerminspeed)) + levelinfo.roamerminspeed;
            if (rnd() < 0.5) enemy[t].speedy *= -1;
            break;
         }
      }
   }
   
   for (n = 0; n < levelinfo.enemycount[BROWNIAN]; n++)
   {
      for (t = 0; t < MAX_ENEMIES; t++)
      {
         if (enemy[t].exists == 0)
         {
            enemy[t].exists = 1;
            enemy[t].type = BROWNIAN;
            enemy[t].x = (rnd() * (WIDTH - (SPRITE_SIZE * 2))) + SPRITE_SIZE;
            enemy[t].y = (rnd() * (lightningy - SPRITE_SIZE)) + (SPRITE_SIZE / 2);
            enemy[t].speedx = 0;
            enemy[t].speedy = 0;
            break;
         }
      }
   }

   for (n = 0; n < levelinfo.enemycount[ACCELERATOR]; n++)
   {
      for (t = 0; t < MAX_ENEMIES; t++)
      {
         if (enemy[t].exists == 0)
         {
            enemy[t].exists = 1;
            enemy[t].type = ACCELERATOR;
            enemy[t].x = (rnd() * (WIDTH - (SPRITE_SIZE * 2))) + SPRITE_SIZE;
            if (levelinfo.guardianaccels)
            {
               enemy[t].y = lightningy + ((n % 2) - 0.5) * SPRITE_SIZE * 2;
            } else {
               enemy[t].y = lightningy - ((n + 1) * SPRITE_SIZE);
            }
            enemy[t].speedx = 0;
            enemy[t].speedy = 0;
            enemy[t].floatvar = n + 2;
            if (levelinfo.guardianaccels)
            {
               enemy[t].floatvar2 = lightningy - enemy[t].y;    // Offset from lightning.
            }
            
            break;
         }
      }
   }

   for (n = 0; n < levelinfo.enemycount[GUNNER]; n++)
   {
      for (t = 0; t < MAX_ENEMIES; t++)
      {
         if (enemy[t].exists == 0)
         {
            enemy[t].exists = 1;
            enemy[t].type = GUNNER;
            enemy[t].x = (rnd() * (WIDTH - (SPRITE_SIZE * 2))) + SPRITE_SIZE;
            enemy[t].y = (rnd() * (lightningy - SPRITE_SIZE)) + (SPRITE_SIZE / 2);
            enemy[t].speedx = (rnd() * (levelinfo.roamermaxspeed - levelinfo.roamerminspeed)) + levelinfo.roamerminspeed;
            if (rnd() < 0.5) enemy[t].speedx *= -1;
            enemy[t].speedy = (rnd() * (levelinfo.roamermaxspeed - levelinfo.roamerminspeed)) + levelinfo.roamerminspeed;
            if (rnd() < 0.5) enemy[t].speedy *= -1;
            enemy[t].intvar = 0;
            break;
         }
      }
   }
   
   if (levelinfo.electrasflag)
   {
      for (t = 0; t < MAX_ENEMIES - 1; t++)
      {
         if (enemy[t].exists == 0)
         {
            enemy[t].exists = 1;
            enemy[t].type = ELECTRA;
            enemy[t].x = 40;
            enemy[t].y = lightningy + levelinfo.electraoffset;
            enemy[t].speedx = 1;
            enemy[t].speedy = 0;
            enemy[t].intvar = t + 1;
            
            assert(enemy[t + 1].exists == 0);
            
            enemy[t + 1].exists = 1;
            enemy[t + 1].type = ELECTRA;
            enemy[t + 1].x = 40;
            enemy[t + 1].y = lightningy + levelinfo.electraoffset;
            enemy[t + 1].speedx = 1.5;
            enemy[t + 1].speedy = 0;
            enemy[t + 1].intvar = t;
            
            levelinfo.enemycount[ELECTRA] = 2;
            
            break;
         }
      }
   }
   
   for (n = 0; n < levelinfo.enemycount[LASERGUN]; n++)
   {
      for (t = 0; t < MAX_ENEMIES; t++)
      {
         if (enemy[t].exists == 0)
         {
            enemy[t].exists = 1;
            enemy[t].type = LASERGUN;
            enemy[t].x = (rnd() * (WIDTH - (SPRITE_SIZE * 2))) + SPRITE_SIZE;
            enemy[t].y = (rnd() * (lightningy - SPRITE_SIZE)) + (SPRITE_SIZE / 2);
            enemy[t].speedx = (rnd() * (levelinfo.roamermaxspeed - levelinfo.roamerminspeed)) + levelinfo.roamerminspeed;
            if (rnd() < 0.5) enemy[t].speedx *= -1;
            enemy[t].speedy = (rnd() * (levelinfo.roamermaxspeed - levelinfo.roamerminspeed)) + levelinfo.roamerminspeed;
            if (rnd() < 0.5) enemy[t].speedy *= -1;
            enemy[t].intvar = 400 + (n * 73);
            break;
         }
      }
   }
   
   for (n = 0; n < levelinfo.enemycount[SKULL]; n++)
   {
      for (t = 0; t < MAX_ENEMIES; t++)
      {
         if (enemy[t].exists == 0)
         {
            enemy[t].exists = 1;
            enemy[t].type = SKULL;
            enemy[t].x = (rnd() * (WIDTH - (SPRITE_SIZE * 2))) + SPRITE_SIZE;
            enemy[t].y = SKULLY;
            enemy[t].speedx = sign(rnd() - 0.5);
            if (enemy[t].speedx == 0)
            {
               enemy[t].speedx = 1;
            }
            enemy[t].speedy = 0;
            break;
         }
      }
   }
            
   return;
}

///////////////////////////////////////////////////////////////////////////////////

int playerdeath (void)
{
   int komideadflag = 0;
   int n;

   if (lightningdeath())
   {
      komideadflag = 1;
      playsound(-1, electricdeath_sound, 0);
      return 1;                  // Must return now else we'll get the standard death sound.
   }
   
   for (n = 0; n < MAX_ENEMIES; n++)
   {
      if (enemy[n].exists && sprite_collision(&komi_sprite, komix, komiy, spritemap[enemy[n].type], enemy[n].x, enemy[n].y))
      {
         komideadflag = 1;
      }
   }

   for (n = 0; n < MAX_ENEMYSHOTS; n++)
   {
      if (enemyshot[n].exists && sprite_collision(&komi_sprite, komix, komiy, &enemyshot_sprite, enemyshot[n].x, enemyshot[n].y))
      {
         komideadflag = 1;
      }
   }
   
   if (sprite_collision(&komi_sprite, komix, komiy, &generator_sprite, GEN_X, lightningy))
   {
      komideadflag = 1;
   }
   if (sprite_collision(&komi_sprite, komix, komiy, &generator_sprite, WIDTH - GEN_X, lightningy))
   {
      komideadflag = 1;
   }
   
   if (tonguelength)
   {
      for (n = 0; n < MAX_ENEMIES; n++)
      {
         if (enemy[n].exists && enemy[n].y < komiy)
         {
            if (sprite_collision(&tip_sprite, komix, komiy - tonguelength, spritemap[enemy[n].type], enemy[n].x, enemy[n].y))
            {
               komideadflag = 1;
            }
            
            // Now we check for a collision between the enemy and the long bit of the tongue.
            
            if (enemy[n].y > (komiy - tonguelength) + (SPRITE_SIZE / 2))      // FIXME ? This bit assumes all sprites equally sized.
            {
               if (sprite_collision(&tonguemask_sprite, komix, enemy[n].y, spritemap[enemy[n].type], enemy[n].x, enemy[n].y))
               {
                  komideadflag = 1;
               }
            }
         }
      }
      for (n = 0; n < MAX_ENEMYSHOTS; n++)
      {
         if (enemyshot[n].exists && enemyshot[n].y < komiy)
         {
            if (sprite_collision(&tip_sprite, komix, komiy - tonguelength, &enemyshot_sprite, enemyshot[n].x, enemyshot[n].y))
            {
               komideadflag = 1;
            }
            
            // Now we check for a collision between the shot and the long bit of the tongue.
            
            if (enemyshot[n].y > (komiy - tonguelength) + (SPRITE_SIZE / 2))      // FIXME ? This bit assumes all sprites equally sized.
            {
               if (sprite_collision(&tonguemask_sprite, komix, enemyshot[n].y, &enemyshot_sprite, enemyshot[n].x, enemyshot[n].y))
               {
                  komideadflag = 1;
               }
            }
         }
      }
      
      for (n = 0; n < MAX_ENEMIES; n++)
      {
         if (enemy[n].exists && enemy[n].type == ELECTRA && enemy[enemy[n].intvar].exists && enemy[n].x < enemy[enemy[n].intvar].x)
         {
            if (enemy[n].y > komiy - tonguelength && enemy[n].x < komix && enemy[enemy[n].intvar].x > komix)
            {
               komideadflag = 1;
               playsound(-1, electricdeath_sound, 0);
               return 1;                  // Must return now else we'll get the standard death sound.
            }
         }
      }
      
      for (n = 0; n < MAX_ENEMIES; n++)
      {
         if (enemy[n].exists && enemy[n].type == LASERGUN && tick % enemy[n].intvar < LASERDURATION && tick >= enemy[n].intvar)
         {
            if (enemy[n].y > komiy - tonguelength)
            {
               komideadflag = 1;
            }
         }
      }
      
   }  // if (tonguelength)
   
   if (komideadflag)
   {
      playsound(-1, contactdeath_sound, -1);
   }
   return komideadflag;
}

///////////////////////////////////////////////////////////////////////////////////

int lightningdeath (void)     // This will be called from several places for aesthetic reasons.
                              // ie so Komi can get drawn electrified, and possibly so that the
                              // lightning can actually pass through him.
{
   if (komiy - lightningy < 16)
   {
      return 1;
   }
   return 0;
}

///////////////////////////////////////////////////////////////////////////////////

void cleanexit (int exitstatus)
{
   if (nosound == 0)
   {
      Mix_CloseAudio();
   }
   SDL_Quit();
   exit(exitstatus);
}

///////////////////////////////////////////////////////////////////////////////////
//
// Mmmm. Pointers to pointers. Yuk...

void loadsound(Mix_Chunk ** thesound, char * directory, char * filename)
{
   char fullpath[256];
   
   FILE * testopen;

   strcpy(fullpath, directory);
   strcat(fullpath, filename);

   if ((testopen = fopen(fullpath, "rb")) == NULL)
   {
      fprintf(stderr, "Failed to open sound %s\n", fullpath);
      return;
   } else {
      fclose(testopen);
   }

   *thesound = Mix_LoadWAV(fullpath);
   if (*thesound == NULL)
   {
      fprintf(stderr, "Failed to load sound %s\n", fullpath);
   }
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void playsound (int channel, Mix_Chunk * thesound, int loops)
{
   if (nosound == 0)
   {
      Mix_PlayChannel(-1, thesound, 0);
   }
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void drawmenu (void)
{
   rects = 0;
   cls(virtue, 0, 0, 0);
   drawsprite(&maintitle_title, virtue, MAINTITLE_X, MAINTITLE_Y);
   drawsprite(&gpl_title, virtue, GPL_X, GPL_Y);
   
   drawsprite(&start_title, virtue, STARTBUTTON_X, STARTBUTTON_Y);
   drawsprite(&quit_title, virtue, QUITBUTTON_X, QUITBUTTON_Y);
   
   drawsprite(&bolts_title, virtue, BOLTSTITLE_X, BOLTSTITLE_Y);
   drawsprite(&diamond_sprite, virtue, DIAMONDTITLE_X, DIAMONDTITLE_Y);
   drawsprite(&komi_sprite, virtue, KOMITITLE_X, KOMITITLE_Y);
   drawsprite(&diver_sprite, virtue, DIVERTITLE_X, DIVERTITLE_Y);
   
   drawsprite(&generator_sprite, virtue, (BOLTSTITLE_X - (bolts_title.width / 2)) - ((SPRITE_SIZE / 2) - (SPRITE_SIZE - LIGHTNINGADJUST)), BOLTSTITLE_Y);
   drawsprite(&generator_sprite, virtue, (BOLTSTITLE_X + (bolts_title.width / 2)) + ((SPRITE_SIZE / 2) - (SPRITE_SIZE - LIGHTNINGADJUST)), BOLTSTITLE_Y);
   
   SDL_Flip(virtue);
   return;
}

///////////////////////////////////////////////////////////////////////////////////
//
// Keeps track in a global structure which keys are actually down.

void updatekeymap (void)
{
   if (event.type == SDL_KEYDOWN)
   {
      if (event.key.keysym.sym == SDLK_ESCAPE)
         keymap.escape = 1;
      if (event.key.keysym.sym == LeftKey1)
         keymap.left1 = 1;
      if (event.key.keysym.sym == LeftKey2)
         keymap.left2 = 1;
      if (event.key.keysym.sym == RightKey1)
         keymap.right1 = 1;
      if (event.key.keysym.sym == RightKey2)
         keymap.right2 = 1;
      if (event.key.keysym.sym == FireKey)
         keymap.fire = 1;
      if (event.key.keysym.sym == LevelSkip)
         keymap.levelskip = 1;
      if (event.key.keysym.sym == PauseKey)
         keymap.pause = 1;
      if (event.key.keysym.sym == ScreenshotKey)
         keymap.screenshot = 1;
   } else if (event.type == SDL_KEYUP) {
      if (event.key.keysym.sym == SDLK_ESCAPE)
         keymap.escape = 0;
      if (event.key.keysym.sym == LeftKey1)
         keymap.left1 = 0;
      if (event.key.keysym.sym == LeftKey2)
         keymap.left2 = 0;
      if (event.key.keysym.sym == RightKey1)
         keymap.right1 = 0;
      if (event.key.keysym.sym == RightKey2)
         keymap.right2 = 0;
      if (event.key.keysym.sym == FireKey)
         keymap.fire = 0;
      if (event.key.keysym.sym == LevelSkip)
         keymap.levelskip = 0;
      if (event.key.keysym.sym == PauseKey)
         keymap.pause = 0;
      if (event.key.keysym.sym == ScreenshotKey)
         keymap.screenshot = 0;
   }
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void updatemousemap (void)
{
   if (event.type == SDL_MOUSEBUTTONDOWN)
   {
      mousemap.button = 1;
      mousemap.clickx = event.button.x;
      mousemap.clicky = event.button.y;
   } else if (event.type == SDL_MOUSEBUTTONUP) {
      mousemap.button = 0;
   }
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void manageevents (void)
{
   while (SDL_PollEvent(&event))
   {
      if (event.type == SDL_QUIT)
      {
         cleanexit(1);
      } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
         updatekeymap();
      } else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
         updatemousemap();
      }
   }
   return;
}

///////////////////////////////////////////////////////////////////////////////////

int leveldone (void)
{
   int n;
   
   for (n = 0; n < MAX_COINS; n++)
   {
      if (coin[n].exists) return 0;
   }
   for (n = 0; n < MAX_DIAMONDS; n++)
   {
      if (diamond[n].exists) return 0;
   }
   
   return 1;
}

///////////////////////////////////////////////////////////////////////////////////

void dostars (void)
{
   int n;
   
   for (n = 0; n < MAXSTARS; n++)
   {
      if (star[n].speed != 0)     // ie if it exists at all...
      {
         setrgb(virtue, star[n].x, star[n].y, 0, 0, 0);
         star[n].x = star[n].x + (((star[n].x - (WIDTH / 2)) / 35) * star[n].speed);
         star[n].y = star[n].y + (((star[n].y - (HEIGHT / 2)) / 35) * star[n].speed);
         if (star[n].x < 0 || star[n].x >= WIDTH || star[n].y < 0 || star[n].y >= HEIGHT)
         {
            resetstar(n);
         }
         star[n].age++;
         if (star[n].age < YOUNG_STAR)
         {
            setrgb(virtue, (int)star[n].x, (int)star[n].y, ((float)star[n].brightness / YOUNG_STAR) * star[n].age, ((float)star[n].brightness / YOUNG_STAR) * star[n].age, ((float)star[n].brightness / YOUNG_STAR) * star[n].age);
         } else {
            setrgb(virtue, (int)star[n].x, (int)star[n].y, star[n].brightness, star[n].brightness, star[n].brightness);
         }
      } else {
         if (rand() < RAND_MAX / 40)     // ie a 1 in 40 chance...
         {
            resetstar(n);
         }
      }
   }
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void resetstar (int n)
{
   float adjust;

   star[n].speed = (float)rand() / RAND_MAX;
   if (star[n].speed < SLOWEST_STAR)
   {
      star[n].speed = SLOWEST_STAR;
   }
   star[n].age = 0;
   adjust = ((rand() % 50) - 25) + ((float)rand() / RAND_MAX);
   star[n].x = (WIDTH / 2) + adjust;
   adjust = ((rand() % 50) - 25) + ((float)rand() / RAND_MAX);
   star[n].y = (HEIGHT / 2) + adjust;
   star[n].brightness = rand() % 256;
   if (star[n].speed < DIM_STAR_BOUNDARY)
   {
      star[n].brightness /= 1.5;
   }
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void doelectricity (void)
{
   int n;
   int currentx, currenty;
   int nextx, nexty;
   int finishedthisline;
   int brightness;
   int de_lightningdeath_flag = 0;
   int brokenlightningflag = 0;
   
   if (lightningdeath())
   {
      de_lightningdeath_flag = 1;
   }
   
   if (levelinfo.electrasflag && levelinfo.electraoffset == 0)
   {
      for (n = 0; n < MAX_ENEMIES; n++)
      {
         if (enemy[n].exists && enemy[n].type == ELECTRA && enemy[enemy[n].intvar].exists)
         {
            brokenlightningflag = 1;
            dobrokenlightning(enemy[n].x, enemy[enemy[n].intvar].x);
            break;
         }
      }
   }
   
   if (brokenlightningflag == 0)
   {
      for (n = 0; n < FORKSTODRAW; n++)
      {
         currentx = LIGHTNINGADJUST;
         if (rnd() < 0.5)
         {
            currenty = lightningy;
         } else {
            currenty = lightningy - 1;   // Hack needed cos generator's "gun" is 2 pixels wide
         }
         finishedthisline = 0;
         while (1)
         {
            nextx = currentx + (rnd() * BOLTMAXLENGTH) + 1;
            nexty = lightningy + (rnd() * LIGHTNINGVARIANCE * 2) - LIGHTNINGVARIANCE;
         
            if (de_lightningdeath_flag && nextx >= komix && currentx < komix && lightningy <= komiy)
            {
               nextx = komix;
               nexty = lightningy + LIGHTNINGVARIANCE;
            }
        
            if (nextx >= WIDTH - LIGHTNINGADJUST)
            {
               nextx = WIDTH - LIGHTNINGADJUST;
               if (rnd() < 0.5)
               {
                  nexty = lightningy;
               } else {
                  nexty = lightningy - 1;
               }
               finishedthisline = 1;
            }
            brightness = rnd() * 155;
            line (virtue, currentx, currenty, nextx, nexty, brightness, brightness, 255);
            currentx = nextx;
            currenty = nexty;
            if (finishedthisline)
            {
               break;
            }
         }
      }
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

int sign (float value)
{
   if (value < 0) return -1;
   if (value > 0) return 1;
   return 0;
}

///////////////////////////////////////////////////////////////////////////////////

float rnd (void)
{
   return (float)rand() / RAND_MAX;
}

///////////////////////////////////////////////////////////////////////////////////

void fadeout (void)
{
   int n;
   
   for (n = 0; n < HEIGHT; n += 2)
   {
      line(virtue, 0, n, WIDTH - 1, n, 0, 0, 0);
      if (n % 10 == 0)
      {
         SDL_Flip(virtue);
         SDL_Delay(10);
         manageevents();
      }
   }

   for (n = HEIGHT - 1; n > 0; n -= 2)
   {
      line(virtue, 0, n, WIDTH - 1, n, 0, 0, 0);
      if ((n + 1) % 10 == 0)
      {
         SDL_Flip(virtue);
         SDL_Delay(10);
         manageevents();
      }
   }
   
   SDL_Flip(virtue);
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void updatetitlebar (void)
{
   char thestring[256];
   
   sprintf(thestring, "Komi  -  Level: %d  -  Score: %d  -  Lives: %d", level, score, lives);
   SDL_WM_SetCaption(thestring, NULL);
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void screenshot (SDL_Surface * surface, char * directory, char * filename)
{
   char fullpath[256];

   strcpy(fullpath, directory);
   strcat(fullpath, filename);
   
   SDL_SaveBMP(surface, fullpath);
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void doelectrabolts (int x1, int y, int x2)
{
   int n;
   int currentx, currenty;
   int nextx, nexty;
   int finishedthisline;
   int brightness;
   int temp;

   if (x1 > x2)
   {
      temp = x1;
      x1 = x2;
      x2 = temp;
   }

   for (n = 0; n < FORKSTODRAW; n++)
   {
      currentx = x1;
      currenty = y;

      finishedthisline = 0;
      while (1)
      {
         nextx = currentx + (rnd() * BOLTMAXLENGTH) + 1;
         nexty = y + (rnd() * LIGHTNINGVARIANCE * 2) - LIGHTNINGVARIANCE;
         
         if (nextx >= komix && currentx < komix && y >= komiy - tonguelength && y <= komiy)
         {
            nextx = komix;
            nexty = y + LIGHTNINGVARIANCE;
         }
        
         if (nextx >= x2)
         {
            nextx = x2;
            nexty = y;
            finishedthisline = 1;
         }
         brightness = rnd() * 155;
         line (virtue, currentx, currenty, nextx, nexty, 255, brightness, brightness);
         currentx = nextx;
         currenty = nexty;
         if (finishedthisline)
         {
            break;
         }
      }
   }
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void dobrokenlightning (int leftelectrax, int rightelectrax)
{

   int n;
   int currentx, currenty;
   int nextx, nexty;
   int finishedthisline;
   int brightness;
   int de_lightningdeath_flag = 0;
   int temp;
   
   if (leftelectrax > rightelectrax)
   {
      temp = leftelectrax;
      leftelectrax = rightelectrax;
      rightelectrax = temp;
   }
   
   for (n = 0; n < FORKSTODRAW; n++)
   {
      currentx = LIGHTNINGADJUST;
      if (rnd() < 0.5)
      {
         currenty = lightningy;
      } else {
         currenty = lightningy - 1;   // Hack needed cos generator's "gun" is 2 pixels wide
      }
      finishedthisline = 0;
      while (1)
      {
         nextx = currentx + (rnd() * BOLTMAXLENGTH) + 1;
         nexty = lightningy + (rnd() * LIGHTNINGVARIANCE * 2) - LIGHTNINGVARIANCE;
         
         if (de_lightningdeath_flag && nextx >= komix && currentx < komix)
         {
            nextx = komix;
            nexty = komiy;
         }
         
         if (nextx > leftelectrax && currentx < leftelectrax)
         {
            nextx = leftelectrax;
            nexty = lightningy;
         }
        
         if (nextx >= WIDTH - LIGHTNINGADJUST)
         {
            nextx = WIDTH - LIGHTNINGADJUST;
            if (rnd() < 0.5)
            {
               nexty = lightningy;
            } else {
               nexty = lightningy - 1;
            }
            finishedthisline = 1;
         }
         brightness = rnd() * 155;
         line (virtue, currentx, currenty, nextx, nexty, brightness, brightness, 255);
         currentx = nextx;
         currenty = nexty;
         
         if (currentx == leftelectrax)
         {
            currentx = rightelectrax;
         }
         
         if (finishedthisline)
         {
            break;
         }
      }
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void drawskullpull (int clearflag, int n)
{
   int pullstarty, pullstartx;
   int i;

   pullstarty = enemy[n].y + 16 - (tick % 16);

   for (i = pullstarty; i <= komiy; i += 16)
   {
      if (clearflag)
      {
         clearsprite(&skullpull_sprite, virtue, enemy[n].x, i);
      } else {
         drawsprite(&skullpull_sprite, virtue, enemy[n].x, i);
      }
   }

   if (komix < enemy[n].x)
   {
      pullstartx = (enemy[n].x - 16) + (tick % 16);
      for (i = pullstartx; i >= komix; i -= 16)
      {
         if (clearflag)
         {
            clearsprite(&skullpull_sprite, virtue, i, komiy);
         } else {
            drawsprite(&skullpull_sprite, virtue, i, komiy);
         }
      }
   }
   
   if (komix > enemy[n].x)
   {
      pullstartx = enemy[n].x + 16 - (tick % 16);
      for (i = pullstartx; i <= komix; i += 16)
      {
         if (clearflag)
         {
            clearsprite(&skullpull_sprite, virtue, i, komiy);
         } else {
            drawsprite(&skullpull_sprite, virtue, i, komiy);
         }
      }
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void goodieaction (int type)   // This function should play some sound, be it the standard eat sound or something else.
{
   switch (type)
   {
      case EXTRALIFE :
         lives++;
         updatetitlebar();
         playsound(-1, oneup_sound, 0);
         break;
      case SHOOTPOWER :
         shotsavailable += 3;
         playsound(-1, eat_sound, 0);
         break;
      case DESTRUCTOR :
         addcornershots();
         playsound(-1, shoot_sound, 0);
         break;
      case RANDOM :
         switch (intrnd(2))
         {
            case 0 :
               goodieaction(EXTRALIFE);
               break;
            case 1 :
               goodieaction(SHOOTPOWER);
               break;
            case 2 :
               goodieaction(DESTRUCTOR);
               break;
            default :
               fprintf(stderr, "Warning: Random goodie-type got bad number from intrnd()\n");
               break;
         }
         break;
      default :
         fprintf(stderr, "Warning: unrecognised powerup type.\n");
         break;
   }

   return;
}

///////////////////////////////////////////////////////////////////////////////////

void addcornershots (void)
{
   int n;

   for (n = 0; n < MAX_FRIENDLYSHOTS; n++)
   {
      if (friendlyshot[n].exists == 0)
      {
         friendlyshot[n].exists = 1;
         friendlyshot[n].x = 0;
         friendlyshot[n].y = 0;
         friendlyshot[n].speedx = 2;
         friendlyshot[n].speedy = 2;
         break;
      }
   }
   for (n = 0; n < MAX_FRIENDLYSHOTS; n++)
   {
      if (friendlyshot[n].exists == 0)
      {
         friendlyshot[n].exists = 1;
         friendlyshot[n].x = WIDTH;
         friendlyshot[n].y = 0;
         friendlyshot[n].speedx = -2;
         friendlyshot[n].speedy = 2;
         break;
      }
   }
   for (n = 0; n < MAX_FRIENDLYSHOTS; n++)
   {
      if (friendlyshot[n].exists == 0)
      {
         friendlyshot[n].exists = 1;
         friendlyshot[n].x = 0;
         friendlyshot[n].y = HEIGHT;
         friendlyshot[n].speedx = 2;
         friendlyshot[n].speedy = -2;
         break;
      }
   }
   for (n = 0; n < MAX_FRIENDLYSHOTS; n++)
   {
      if (friendlyshot[n].exists == 0)
      {
         friendlyshot[n].exists = 1;
         friendlyshot[n].x = WIDTH;
         friendlyshot[n].y = HEIGHT;
         friendlyshot[n].speedx = -2;
         friendlyshot[n].speedy = -2;
         break;
      }
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void addkomishot (void)
{
   int n;

   for (n = 0; n < MAX_FRIENDLYSHOTS; n++)
   {
      if (friendlyshot[n].exists == 0)
      {
         friendlyshot[n].exists = 1;
         friendlyshot[n].x = komix;
         friendlyshot[n].y = komiy;
         friendlyshot[n].speedx = 0;
         friendlyshot[n].speedy = -3;
         playsound(-1, shoot_sound, 0);
         break;
      }
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

int intrnd (int max)    // Return integer between 0 and max inclusive.
{
   int result;
   
   result = rnd() * (max + 1);
   result %= max + 1;
   return result;
}

