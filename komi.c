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
   
   // Just check here that MAXRECTS has been declared to be big enough (else it might crash in --fastdraw mode)
   // Note that each sprite uses 2 update-rects - one for it's current position, and one for it's old one.
   assert(MAXRECTS > (MAX_ENEMIES * 2) + (MAX_COINS * 2) + (MAX_DIAMONDS * 2) + (MAX_ENEMYSHOTS * 2) + (MAX_FRIENDLYSHOTS * 2) + 50);

   for (n = 1; n < argc; n++)
   {
      if (strcmp(argv[n], "--delay") == 0 && n < argc - 1)
      {
         gotdelayarg = 1;
         delay = atoi(argv[n + 1]);
         if (delay < 0 || delay > 50)
         {
            delay = DEFAULT_DELAY;
            gotdelayarg = 0;
            fprintf(stderr, "Please enter a delay value between 0 and 50.\n");
         }
      }
      if (strcmp(argv[n], "--resetprefs") == 0)
      {
         saveprefs(filepath, "prefs");
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
         nomusic = 1;
      }
      if (strcmp(argv[n], "--nomusic") == 0)
      {
         nomusic = 1;
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
      if (strcmp(argv[n], "--shuffle") == 0)
      {
         shuffle = 1;
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
      if (strcmp(argv[n], "--help") == 0 || strcmp(argv[n], "-h") == 0 || strcmp(argv[n], "--usage") == 0 || strcmp(argv[n], "-help") == 0 || strcmp(argv[n], "help") == 0)   // Accept multiple help commands....
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
         if (strlen(argv[n + 1]) < sizeof(filepath))   // Check for buffer overrun...
         {
            sprintf(filepath, "%s", argv[n + 1]);
         } else {
            fprintf(stderr, "Directory name given is too long, attempting to use default,\n");
            fprintf(stderr, "which is: %s\n", filepath);
         }
         if (filepath[strlen(filepath) - 1] != '/')
         {
            fprintf(stderr, "Warning: Directory name given did not end with a '/'\n");
            fprintf(stderr, "Are you sure you've passed the full pathname?\n");
         }
      }
   }
   
   loadprefs(filepath, "prefs");
   saveprefs(filepath, "prefs");    // In case of command line arguments needing to be saved.
                                    // Or in case of first-run.
   
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
      if (Mix_OpenAudio(44100, AUDIO_S16, 2, 2048))
      {
         fprintf(stderr, "Unable to open audio. %s\n", SDL_GetError());
         nosound = 1;
         nomusic = 1;
      } else {
         loadsounds();
         // if (nomusic == 0)
         // {
         //    playmusic(filepath, "music.mod", -1);
         // }
      }
   }

   if (fullscreen == 0)
   {
      virtue = SDL_SetVideoMode(WIDTH, HEIGHT, 0, SDL_ANYFORMAT);
      setmaintitlebar();
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
   int highlight_start = 0;
   int highlight_quit = 0;

   drawmenu(highlight_start, highlight_quit);
   
   while (1)
   {
      keymap.escape = 0;      // So we don't respond to escape pressed in game.
      mousemap.button = 0;    // Likewise for the mouse.
      
      manageevents();
      
      // Highlight the start button, if necessary...
      if (abs(mousemap.currentx - STARTBUTTON_X) < start_title.pixelmap->w / 2 && abs(mousemap.currenty - STARTBUTTON_Y) < start_title.pixelmap->h / 2)
      {
         if (highlight_start == 0)
         {
            highlight_start = 1;
            drawmenu(highlight_start, highlight_quit);
         }
      } else {
         if (highlight_start == 1)
         {
            highlight_start = 0;
            drawmenu(highlight_start, highlight_quit);
         }
      }

      // Highlight the quit button, if necessary...
      if (abs(mousemap.currentx - QUITBUTTON_X) < quit_title.pixelmap->w / 2 && abs(mousemap.currenty - QUITBUTTON_Y) < quit_title.pixelmap->h / 2)
      {
         if (highlight_quit == 0)
         {
            highlight_quit = 1;
            drawmenu(highlight_start, highlight_quit);
         }
      } else {
         if (highlight_quit == 1)
         {
            highlight_quit = 0;
            drawmenu(highlight_start, highlight_quit);
         }
      }
      
      if (keymap.escape)
      {
         cleanexit(0);
      }
      if (mousemap.button)
      {
         if (abs(mousemap.clickx - STARTBUTTON_X) < start_title.pixelmap->w / 2 && abs(mousemap.clicky - STARTBUTTON_Y) < start_title.pixelmap->h / 2)
         {
            SDL_ShowCursor(SDL_DISABLE);
            game();
            SDL_ShowCursor(SDL_ENABLE);
            setmaintitlebar();
            drawmenu(highlight_start, highlight_quit);
         } else if (abs(mousemap.clickx - QUITBUTTON_X) < quit_title.pixelmap->w / 2 && abs(mousemap.clicky - QUITBUTTON_Y) < quit_title.pixelmap->h / 2) {
            cleanexit(0);
         } else {
            checkspeedadjust();
         }
      }
      SDL_Delay(1);
   }
}

///////////////////////////////////////////////////////////////////////////////////

void game (void)
{
   int gameoverflag;
   
   shufflelevels();
   
   level = 1;
   lives = START_LIVES;
   fastretract = 0;
   score = 0;
   havecheated = 0;
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
            fastretract = 0;    // Currently the only thing that needs to be reset here.
            resetmoney = 0;
            if (lives < 1)
            {
               gameoverflag = 1;
            }
            if (lives == 1)
            {
               givelastlifewarning = 1;
            }
            break;
         case QUIT :
            gameoverflag = 1;
            break;
         default :
            fprintf(stderr, "Fatal error: playlevel() returned an unexpected value.\n\n");
            cleanexit(1);
      }
   }
   
   if (score > highscore && havecheated == 0 && invincible == 0)
   {
      highscore = score;
      saveprefs(filepath, "prefs");
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
   freeze = 0;
   
   rects = 0;
   
   keymap.levelskip = 0;
   keymap.pause = 0;
   
   mousemap.button = 0;

   cls(virtue, 0, 0, 0);
   SDL_UpdateRect(virtue, 0, 0, 0, 0);
   updatetitlebar();
   
   if (lives == 1 && givelastlifewarning && fullscreen)
   {
      playsound(lastlife_sound);
      givelastlifewarning = 0;
   }
   
   for (n = 0; n < MAX_ENEMIES; n++)
   {
      if (enemy[n].exists && enemy[n].type == LASERGUN)
      {
         playsound(laserentry_sound);
         break;
      }
   }
   
   while(1)
   {
      tick++;
      
      rnd();   // Pump rnd() every frame for (hopefully) more randomness when it matters.
      
      playedshootsound = 0;   // shoot_sound can be annoying if played loud due to simultaneous playback of > 1 instance.
      
      if (freeze)
      {
         freeze--;
         if (freeze % FREEZE_WARN_INTERVAL == 0 && freeze && freeze <= FREEZE_WARN_INTERVAL * FREEZE_WARNINGS && freeze > 0)
         {
            playsound(unfreezewarning_sound);
         }
      }
      
      if (nostarsflag == 0)
      {
         dostars();
      }
      
      notedtime = SDL_GetTicks();
      if (delay > 0)
      {
         if (hog)
         {
            while (SDL_GetTicks() - notedtime < delay)
            {
               ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ; ;     // I assume this gets us some NOOPs.
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
         SDL_UpdateRect(virtue, 0, 0, 0, 0);
      }
      rects = 0;
      
      if (invincible == 0)
      {
         if (playerdeath())
         {
            for (n = 0; n < 10; n++)     // 10 delays of 50 ms, with event checking so user can press escape.
            {
               manageevents();
               if (keymap.escape)
               {
                  return QUIT;
               }
               SDL_Delay(50);
            }
            if (lives == 1)
            {
               playsound(gameover_sound);
            }
            fadeout();            // fadeout() returns immediately if escape is pressed while it's doing its thing.
            if (keymap.escape)    // In which case, keymap.escape will still be set.
            {
               return QUIT;
            } else {
               return DEATH;
            }
         }
      }
      if (leveldone())
      {
         fadeout();
         if (keymap.escape)        // See note above.
         {
            return QUIT;
         } else {
            return LEVEL_COMPLETE;
         }
      }
      
      manageevents();
      if (keymap.escape)
      {
         return QUIT;
      }
      if (cheats && keymap.levelskip)
      {
         havecheated = 1;
         return LEVEL_COMPLETE;
      }
      if (keymap.screenshot)
      {
         screenshot(virtue, "", "komiscreen.bmp");
         keymap.screenshot = 0;
      }
      if (keymap.pause)
      {
         playsound(pause_sound);
         keymap.pause = 0;
         keymap.left1 = 0;
         keymap.left2 = 0;
         keymap.left3 = 0;
         keymap.right1 = 0;
         keymap.right2 = 0;
         keymap.right3 = 0;
         keymap.fire1 = 0;
         keymap.fire2 = 0;
         while ((keymap.pause | keymap.left1 | keymap.left2 | keymap.left3 | keymap.right1 | keymap.right2 | keymap.right3 | keymap.fire1 | keymap.fire2) == 0)
         {
            manageevents();
            if (keymap.escape)
            {
               return QUIT;
            }
            if (keymap.screenshot)
            {
               screenshot(virtue, "", "komiscreen.bmp");
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

   if (keymap.left1 || keymap.left2 || keymap.left3)
   {
      komix = komix - KOMI_SPEED;
   }
   if (keymap.right1 || keymap.right2 || keymap.right3)
   {
      komix = komix + KOMI_SPEED;
   }

   if (komix < WALL) komix = WALL;
   if (komix > WIDTH - WALL) komix = WIDTH - WALL;
   
   if ((keymap.fire1 || keymap.fire2) && tonguelength == 0)
   {
      if (shotsavailable > 0)
      {
         addkomishot();
         shotsavailable--;
         keymap.fire1 = 0;
         keymap.fire2 = 0;
      } else {
         tonguespeed = TONGUE_SPEED;
      }
   }

   if (tonguespeed > 0 && keymap.fire1 == 0 && keymap.fire2 == 0)
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
   
   for (n = 0; n < MAX_COINS; n++)
   {
      if (caughttype == COIN && caughtnumber == n)
      {
         coin[n].x = caughtoffsetx + komix;
         coin[n].y = caughtoffsety + komiy - tonguelength;
         if (coin[n].y > komiy - 10 || tonguelength == 0)
         {
            coin[n].exists = 0;
            addscore(COIN_SCORE);
            caughttype = 0;
            caughtnumber = -1;
            tonguelength = 0;
            tonguespeed = 0;
            playsound(eat_sound);
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
            addscore(DIAMOND_SCORE);
            caughttype = 0;
            caughtnumber = -1;
            tonguelength = 0;
            tonguespeed = 0;
            playsound(eat_sound);
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
               playsound(destructorkill_sound);
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
   
   if (goodie.exists == 0 && tick < POWERUPCUTOFFTIME)
   {
      for (n = 0; n < POWERUPTYPES; n++)
      {
         if (rnd() < levelinfo.powerup_prob[n])
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
            playsound(powerup_sound);
            break;
         }
      }
   }
   
   if (freeze == 0)       // Stuff that can be frozen by powerup follows...
   {
   
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
            playsound(lightningwarning_sound);
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
               case EYEBALL :
                  moveeyeball(n);
                  break;
               case WRAPBALL :
                  movewrapball(n);
                  break;
               case BOMBER :
                  movebomber(n);
                  break;
               case SNIPERLEFT :
                  movesniper(n);
                  break;
               case SNIPERRIGHT :
                  movesniper(n);
                  break;
            }
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
   enemy[n].speedx += (rnd() - 0.5) / 3;
   enemy[n].speedy += (rnd() - 0.5) / 3;
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
      enemy[n].y = lightningy + enemy[n].intvar;
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
            if (playedshootsound == 0)
            {
               playsound(shoot_sound);
               playedshootsound = 1;
            }
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

void moveeyeball (int n)
{
   if (tonguelength)
   {
      enemy[n].speedx = komix - enemy[n].x;
      enemy[n].speedy = (komiy - tonguelength) - enemy[n].y;
      if (fabs(enemy[n].speedx) > levelinfo.enemyshotbasespeed)
      {
         enemy[n].speedy = enemy[n].speedy / (fabs(enemy[n].speedx) / enemy[n].floatvar);
         enemy[n].speedx = enemy[n].speedx / (fabs(enemy[n].speedx) / enemy[n].floatvar);
      }
      if (fabs(enemy[n].speedy) > levelinfo.enemyshotbasespeed)
      {
         enemy[n].speedx = enemy[n].speedx / (fabs(enemy[n].speedy) / enemy[n].floatvar);
         enemy[n].speedy = enemy[n].speedy / (fabs(enemy[n].speedy) / enemy[n].floatvar);
      }
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
   if (enemy[n].y > HEIGHT - (SPRITE_SIZE / 2))
   {
      enemy[n].y = HEIGHT - (SPRITE_SIZE / 2);
      enemy[n].speedy = fabs(enemy[n].speedy) * -1;
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void movewrapball (int n)
{
   if (tonguelength)
   {
      enemy[n].speedx = komix - enemy[n].x;
      enemy[n].speedy = (komiy - tonguelength) - enemy[n].y;
      if (fabs(enemy[n].speedx) > levelinfo.enemyshotbasespeed)
      {
         enemy[n].speedy = enemy[n].speedy / (fabs(enemy[n].speedx) / enemy[n].floatvar);
         enemy[n].speedx = enemy[n].speedx / (fabs(enemy[n].speedx) / enemy[n].floatvar);
      }
      if (fabs(enemy[n].speedy) > levelinfo.enemyshotbasespeed)
      {
         enemy[n].speedx = enemy[n].speedx / (fabs(enemy[n].speedy) / enemy[n].floatvar);
         enemy[n].speedy = enemy[n].speedy / (fabs(enemy[n].speedy) / enemy[n].floatvar);
      }
   }
   enemy[n].x += enemy[n].speedx;
   enemy[n].y += enemy[n].speedy;
   
   if (enemy[n].x > WIDTH + SPRITE_SIZE / 2)
   {
      enemy[n].x = SPRITE_SIZE / -2;
   }
   if (enemy[n].x < SPRITE_SIZE / -2)
   {
      enemy[n].x = WIDTH + SPRITE_SIZE / 2;
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

void movebomber (int n)
{
   int i;

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
   enemy[n].intvar -= 1;
   if (enemy[n].intvar == 0)
   {
      enemy[n].intvar = enemy[n].intvar2;
      for (i = 0; i < MAX_ENEMYSHOTS; i++)
      {
         if (enemyshot[i].exists == 0)
         {
            enemyshot[i].exists = 1;
            enemyshot[i].x = enemy[n].x;
            enemyshot[i].y = enemy[n].y;
            enemyshot[i].speedx = 0;
            enemyshot[i].speedy = levelinfo.bombspeed;
            if (playedshootsound == 0)
            {
               playsound(shoot_sound);
               playedshootsound = 1;
            }
            break;
         }
      }
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void movesniper (int n)
{
   int i;

   enemy[n].y += enemy[n].speedy;
   if (enemy[n].y < SPRITE_SIZE / 2)
   {
      enemy[n].y = SPRITE_SIZE / 2;
      enemy[n].speedy = fabs(enemy[n].speedy);
   }
   if (enemy[n].y > HEIGHT - (SPRITE_SIZE * 2))
   {
      enemy[n].y = HEIGHT - (SPRITE_SIZE * 2);
      enemy[n].speedy = fabs(enemy[n].speedy) * -1;
   }
   if (tonguelength)
   {
      if (komiy - tonguelength < enemy[n].y)
      {
         if (enemy[n].intvar == 0)
         {
            enemy[n].intvar = 1;
            for (i = 0; i < MAX_ENEMYSHOTS; i++)
            {
               if (enemyshot[i].exists == 0)
               {
                  enemyshot[i].exists = 1;
                  enemyshot[i].x = enemy[n].x;
                  enemyshot[i].y = enemy[n].y;
                  
                  enemyshot[i].speedx = SNIPERSHOTSPEED;
                  if (enemy[n].type == SNIPERRIGHT)
                  {
                     enemyshot[i].speedx *= -1;
                  }
                  
                  enemyshot[i].speedy = 0;
                  
                  if (playedshootsound == 0)
                  {
                     playsound(shoot_sound);
                     playedshootsound = 1;
                  }
                  break;
               }
            }
         }
      }
   } else {                    // Note: This working correctly is dependent on there always being a tick of tonguelength == 0
      enemy[n].intvar = 0;     // even when spacebar is continually held down. Thankfully, this is the case.
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
   
   // Draw pre-laser... (and play the sound if necessary - this seems the best place to play the sound)
   for (n = 0; n < MAX_ENEMIES; n++)
   {
      if (enemy[n].exists && enemy[n].type == LASERGUN && (tick % enemy[n].intvar) >= (enemy[n].intvar - LASERWARNTIME))
      { 
         frect(virtue, 0, enemy[n].y - (LASERWIDTH / 2), WIDTH, enemy[n].y + (LASERWIDTH / 2), WARNLASER_R, WARNLASER_G, WARNLASER_B);
         if (tick % enemy[n].intvar == enemy[n].intvar - LASERWARNTIME)
         {
            playsound(laserpowerup_sound);
         }
      }
   }

   // Draw laser... (and play the sound if necessary - this seems the best place to play the sound)
   for (n = 0; n < MAX_ENEMIES; n++)
   {
      if (enemy[n].exists && enemy[n].type == LASERGUN && tick % enemy[n].intvar < LASERDURATION && tick >= enemy[n].intvar)
      {
         frect(virtue, 0, enemy[n].y - (LASERWIDTH / 2), WIDTH, enemy[n].y + (LASERWIDTH / 2), LASER_R, LASER_G, LASER_B);
         if (tick % enemy[n].intvar == 0)
         {
            playsound(laser_sound);
         }
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
      if (fastretract)
      {
         frect(virtue, (komix - TONGUE_WIDTH / 2), komiy - tonguelength - 1, komix + (TONGUE_WIDTH / 2), komiy, 0, 0, 0);
         frect(virtue, (komix - TONGUE_WIDTH / 2) + 1, komiy - tonguelength, (komix + TONGUE_WIDTH / 2) - 1, komiy, TONGUE_FAST_R, TONGUE_FAST_G, TONGUE_FAST_B);
         drawsprite(&fasttip_sprite, virtue, komix, komiy - tonguelength);
      } else {
         frect(virtue, (komix - TONGUE_WIDTH / 2), komiy - tonguelength - 1, komix + (TONGUE_WIDTH / 2), komiy, 0, 0, 0);
         frect(virtue, (komix - TONGUE_WIDTH / 2) + 1, komiy - tonguelength, (komix + TONGUE_WIDTH / 2) - 1, komiy, TONGUE_R, TONGUE_G, TONGUE_B);
         drawsprite(&tip_sprite, virtue, komix, komiy - tonguelength);
      }
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
            playsound(stick_sound);
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
            playsound(stick_sound);
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
         playsound(stick_sound);
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
   
   // Anything that can be set by choosenumbers() needs to be set to a default value here.
   // Also clear all objects (with the exception of coins + diamonds if this isn't a new level).

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
   
   levelinfo.diverhoverlevel = SPRITE_SIZE + (SPRITE_SIZE / 4);
   levelinfo.divespeedy = DIVE_SPEEDY_DEFAULT;
   levelinfo.divespeedx = DIVE_SPEEDX_DEFAULT;
   
   levelinfo.bomberhoverlevel = (SPRITE_SIZE * 2) + (SPRITE_SIZE / 4);
   levelinfo.bombspeed = BOMB_SPEED_DEFAULT;
   
   levelinfo.roamerminspeed = ROAMER_MIN_SPEED_DEFAULT;
   levelinfo.roamermaxspeed = ROAMER_MAX_SPEED_DEFAULT;

   levelinfo.brownianmaxspeed = BROWNIAN_MAX_SPEED_DEFAULT;
   
   levelinfo.guardianaccels = 0;
   
   levelinfo.gunnershootprob = GUNNER_SHOOT_PROB_DEFAULT;
   levelinfo.enemyshotbasespeed = ENEMY_SHOT_SPEED_DEFAULT;
   
   levelinfo.electrasflag = 0;
   levelinfo.electraoffset = ELECTRA_OFFSET_DEFAULT;
   
   levelinfo.snipersflag = 0;
   
   levelinfo.dropperhoveroffset = SPRITE_SIZE;
   levelinfo.dropperspeedy = DROPPER_SPEEDY_DEFAULT;
   
   for (n = 0; n < POWERUPTYPES; n++)
   {
      levelinfo.powerup_prob[n] = BASEPOWERUPPROB;
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void choosenumbers (void)
{

   int coins;         // Local variables which will only be copied to the globals
   int diamonds;      // if resetmoney is flagged (due to this being a new level).

   int pretendlevel;  // Used for shuffled levels; we pretend we're on this level.
   
   if (shuffle == 0 || level > DEFINEDLEVELS)
   {
      pretendlevel = level;
   } else {
      pretendlevel = shuffledlevels[level - 1];
   }

   // Default values, may be over-ridden in a moment.
   coins = (pretendlevel / 2.5) + 2;
   diamonds = (pretendlevel % 2) + 1;
   
   if (algorithmicenemies == 0)
   {
      switch (pretendlevel)
      {
         case 1 :
            levelinfo.enemycount[BOUNCER] = 1;
            levelinfo.enemycount[SCROLLERLEFT] = 1;
            levelinfo.enemycount[DROPPER] = 2;
            levelinfo.enemycount[BOMBER] = 1;
            levelinfo.roamermaxspeed = 2;
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
            levelinfo.enemycount[BOUNCER] = 1;
            levelinfo.enemycount[EYEBALL] = 1;
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
            levelinfo.enemycount[BROWNIAN] = 3;
            levelinfo.enemycount[BOMBER] = 1;
            break;
         case 7 :
            levelinfo.enemycount[ROAMER] = 7;
            levelinfo.enemycount[DIVER] = 1;
            levelinfo.roamermaxspeed = 3;
               coins = 6;
               diamonds = 2;
            levelinfo.topdiamonds = 0;
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
               coins = 6;
               diamonds = 2;
            break;
         case 10 :
            levelinfo.enemycount[DROPPER] = 8;
            levelinfo.enemycount[GUNNER] = 1;
            levelinfo.enemycount[ROAMER] = 1;
            levelinfo.gunnershootprob = GUNNER_SHOOT_PROB_DEFAULT * 0.6;
               coins = 4;
               diamonds = 2;
            lightningcheck = 60;
            break;
         case 11 :
            levelinfo.enemycount[BOMBER] = 7;
            levelinfo.enemycount[SCROLLERRIGHT] = 2;
            levelinfo.enemycount[ROAMER] = 2;
            levelinfo.roamermaxspeed = 2;        // Mostly for the Bombers, which also use this value.
               coins = 6;
               diamonds = 1;
            break;
         case 12 :
            levelinfo.electrasflag = 1;
            levelinfo.enemycount[SCROLLERRIGHT] = 1;
            levelinfo.enemycount[SCROLLERLEFT] = 1;
            levelinfo.enemycount[DIVER] = 1;
            levelinfo.enemycount[ROAMER] = 1;
            levelinfo.enemycount[BROWNIAN] = 1;
            levelinfo.enemycount[LASERGUN] = 1;
            levelinfo.enemycount[SKULL] = 1;
            levelinfo.enemycount[GUNNER] = 1;
            levelinfo.guardianaccels = 1;
            levelinfo.electraoffset = 0;
            lightningcheck = 80;
               coins = 5;
               diamonds = 1;
            break;
         case 13 :
            levelinfo.enemycount[DIVER] = 9;
            levelinfo.enemycount[BOUNCER] = 1;
            levelinfo.topdiamonds = 0;
               coins = 3;
               diamonds = 2;
            break;
         case 14 :
            levelinfo.enemycount[DROPPER] = 4;
            levelinfo.enemycount[GUNNER] = 3;
            levelinfo.gunnershootprob = 0.005;
               coins = 4;
               diamonds = 2;
            break;
         case 15 :
            levelinfo.enemycount[EYEBALL] = 2;
            levelinfo.enemycount[WRAPBALL] = 1;  
            levelinfo.enemycount[GUNNER] = 1;
               coins = 3;
               diamonds = 4;
            lightningcheck = 100;
            break;
         case 16 :
            levelinfo.enemycount[DIVER] = 1;
            levelinfo.enemycount[SKULL] = 1;
            levelinfo.enemycount[BROWNIAN] = 2;
            levelinfo.enemycount[ROAMER] = 2;
            levelinfo.snipersflag = 1;
               coins = 4;
               diamonds = 3;
            break;
         case 17 :
            levelinfo.enemycount[GUNNER] = 1;
            levelinfo.enemycount[BOMBER] = 1;
            levelinfo.enemycount[ACCELERATOR] = 2;
            levelinfo.enemycount[ROAMER] = 2;
            levelinfo.guardianaccels = 1;
               coins = 7;
               diamonds = 1;
            break;
         case 18 :
            levelinfo.enemycount[SKULL] = 1;
            levelinfo.enemycount[LASERGUN] = 4;
            levelinfo.enemycount[GUNNER] = 2;
            levelinfo.enemycount[ACCELERATOR] = 1;
            levelinfo.guardianaccels = 1;
            levelinfo.gunnershootprob = 0.005;
            break;
         case 19 :
            levelinfo.enemycount[SCROLLERLEFT] = 1;
            levelinfo.enemycount[SCROLLERRIGHT] = 1;
            levelinfo.enemycount[EYEBALL] = 1;
            levelinfo.enemycount[WRAPBALL] = 1;
            levelinfo.snipersflag = 1;
            lightningcheck = 100000000;
               coins = 6;
               diamonds = 2;
            break;
         case 20 :
            levelinfo.enemycount[SCROLLERRIGHT] = 2;
            levelinfo.enemycount[DROPPER] = 3;
            levelinfo.enemycount[BOUNCER] = 1;
            levelinfo.enemycount[ROAMER] = 2;
            levelinfo.enemycount[ACCELERATOR] = 1;
            levelinfo.enemycount[EYEBALL] = 1;
               coins = 6;
               diamonds = 2;
            levelinfo.topdiamonds = 1;
            lightningcheck = 100;
            break;
         case 21 :
            levelinfo.enemycount[DIVER] = 5;
            levelinfo.enemycount[BROWNIAN] = 2;
            levelinfo.enemycount[ROAMER] = 1;
            levelinfo.enemycount[SCROLLERRIGHT] = 3;
            levelinfo.electrasflag = 1;
               coins = 5;
               diamonds = 3;
            levelinfo.topdiamonds = 0;
            break;
         case 22 :
            levelinfo.enemycount[DROPPER] = 5;
            levelinfo.enemycount[EYEBALL] = 1;
            levelinfo.enemycount[WRAPBALL] = 1;
            lightningcheck = 100000000;
               coins = 6;
               diamonds = 3;
            break;
         case 23 :
            levelinfo.enemycount[GUNNER] = 6;
               coins = 0;
               diamonds = 1;
            break;
         case 24 :
            levelinfo.enemycount[BOUNCER] = 3;
            levelinfo.enemycount[ROAMER] = 6;
            levelinfo.enemycount[BROWNIAN] = 6;
            levelinfo.enemycount[DIVER] = 1;
            levelinfo.enemycount[SCROLLERLEFT] = 1;
            levelinfo.powerup_prob[SHOOTPOWER] = 1;
            lightningcheck = 100000000;
               coins = 9;
               diamonds = 2;
            break;
         case 25 :
            levelinfo.enemycount[GUNNER] = 3;
            levelinfo.enemycount[SCROLLERRIGHT] = 2;
            lightningcheck = 100000000;
            levelinfo.topdiamonds = 0;
            levelinfo.roamermaxspeed = 1.5;
            levelinfo.roamerminspeed = 3;
               coins = 3;
               diamonds = 4;
            break;
         case 26 :
            levelinfo.enemycount[SCROLLERLEFT] = 2;
            levelinfo.enemycount[DROPPER] = 4;
            levelinfo.enemycount[ROAMER] = 5;
            levelinfo.enemycount[SKULL] = 1;
            lightningcheck = 100;
               coins = 7;
               diamonds = 3;
            break;
         case 27 :
            levelinfo.enemycount[ACCELERATOR] = 4;
            levelinfo.enemycount[BOUNCER] = 1;
            levelinfo.enemycount[GUNNER] = 1;
               coins = 4;
               diamonds = 2;
            levelinfo.topdiamonds = 0;
            levelinfo.guardianaccels = 1;
            break;
         case 28 :
            levelinfo.enemycount[BOUNCER] = 2;
            levelinfo.enemycount[GUNNER] = 1;
            levelinfo.enemycount[BOMBER] = 1;
            levelinfo.enemycount[SCROLLERLEFT] = 2;
            levelinfo.enemycount[SCROLLERRIGHT] = 2;
            levelinfo.enemycount[ROAMER] = 3;
               coins = 5;
               diamonds = 1;
            break;
         case 29 :
            levelinfo.enemycount[BOMBER] = 12;
            levelinfo.enemycount[BOUNCER] = 4;
            levelinfo.topdiamonds = 0;
               coins = 6;
               diamonds = 4;
            break;
         case 30 :                  // Remember to increase DEFINEDLEVELS in declarations.h if we go past this.
            lightningcheck = 2;
            levelinfo.fastlightningy = START_LIGHTNING_Y + 10;
            levelinfo.fastlightningcheck = 1;
            levelinfo.powerup_prob[EXTRALIFE] = 1;
               coins = 0;
               diamonds = 1;
            break;
         default :
            algorithmicenemynumbers(pretendlevel);
            break;
      }
   } else {
      algorithmicenemynumbers(pretendlevel);
   }
   
   if (resetmoney)
   {
      levelinfo.coins = coins;
      levelinfo.diamonds = diamonds;
   }
   
   return;
}
   
///////////////////////////////////////////////////////////////////////////////////

void algorithmicenemynumbers (int thislevel)   // Set enemy numbers algorithmically. Only called if numbers not predefined above.
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
   if (thislevel % 3 == 1 || thislevel % 7 == 1) levelinfo.enemycount[EYEBALL] = 1;
   if (thislevel % 8 == 3) levelinfo.enemycount[DROPPER] = 3;
   
   if (thislevel > 10) levelinfo.topdiamonds = 0;
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void makelevel (void)    // Set the various objects into existence.

/*
The various enemies use the 2 intvar variables and the floatvar
variable for different things. Here's the list...

DROPPER:
Uses intvar as a flag for whether it's allowed to fall. Set to zero so
it can't fall if Komi is underneath it at level start. Gets set to one
as soon as Komi is not underneath it.

DIVER:
Uses intvar as its countdown to diving, and resets it to the value in
intvar2 when the dive is complete.

BOUNCER:
Uses floatvar, which stores its maximum vertical speed.

ACCELERATOR:
Uses floatvar for its maximum speed.
Uses intvar for its offset from lightning, if in Guardian mode.

GUNNER:
Uses intvar to store tick (frame) it last fired in. Used so that there's
a delay between shots.

ELECTRA:
Uses intvar to store the enemy-number of its target Electra.

LASERGUN:
Uses intvar to store what tick it should fire its laser.

EYEBALL / WRAPBALL:
Uses floatvar to store it's maximum speed.

BOMBER:
Uses intvar for countdown to shot, and intvar2 for what to reset count to afterwards.

SNIPER:
Uses intvar for whether it has already fired a shot since the tongue started its trip.

*/

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
   // The second for loop in each case is just an easy (but rather inefficient) way to locate the first vacant enemy structure.
   
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

   for (n = 0; n < levelinfo.enemycount[BOMBER]; n++)
   {
      for (t = 0; t < MAX_ENEMIES; t++)
      {
         if (enemy[t].exists == 0)
         {
            enemy[t].exists = 1;
            enemy[t].type = BOMBER;
            enemy[t].x = (rnd() * (WIDTH - (SPRITE_SIZE * 2))) + SPRITE_SIZE;
            enemy[t].y = levelinfo.bomberhoverlevel;
            enemy[t].speedx = (rnd() * (levelinfo.roamermaxspeed - levelinfo.roamerminspeed)) + levelinfo.roamerminspeed;
            if (rnd() < 0.5) enemy[t].speedx *= -1;
            enemy[t].speedy = 0;
            enemy[t].intvar = 80 + (n * 40);
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
               enemy[t].intvar = lightningy - enemy[t].y;    // Offset from lightning.
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
   
   if (levelinfo.snipersflag)
   {
      for (t = 0; t < MAX_ENEMIES - 1; t++)
      {
         if (enemy[t].exists == 0)
         {
            enemy[t].exists = 1;
            enemy[t].type = SNIPERLEFT;
            enemy[t].x = SPRITE_SIZE / 2;
            enemy[t].y = HEIGHT - (SPRITE_SIZE * 2);
            enemy[t].speedx = 0;
            enemy[t].speedy = -2;
            enemy[t].intvar = 0;
            
            assert(enemy[t + 1].exists == 0);
            
            enemy[t + 1].exists = 1;
            enemy[t + 1].type = SNIPERRIGHT;
            enemy[t + 1].x = WIDTH - (SPRITE_SIZE / 2);
            enemy[t + 1].y = HEIGHT - (SPRITE_SIZE * 2);
            enemy[t + 1].speedx = 0;
            enemy[t + 1].speedy = -2;
            enemy[t + 1].intvar = 0;
            
            levelinfo.enemycount[SNIPERLEFT] = 1;
            levelinfo.enemycount[SNIPERRIGHT] = 1;
            
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
   
   for (n = 0; n < levelinfo.enemycount[WRAPBALL]; n++)
   {
      for (t = 0; t < MAX_ENEMIES; t++)
      {
         if (enemy[t].exists == 0)
         {
            enemy[t].exists = 1;
            enemy[t].type = WRAPBALL;
            enemy[t].x = (rnd() * (WIDTH - (SPRITE_SIZE * 2))) + SPRITE_SIZE;
            enemy[t].y = (rnd() * (lightningy - SPRITE_SIZE)) + (SPRITE_SIZE / 2);
            enemy[t].speedx = 0;
            enemy[t].speedy = 0;
            enemy[t].floatvar = 2.5 + ((float)n / 2);
            break;
         }
      }
   }

   for (n = 0; n < levelinfo.enemycount[EYEBALL]; n++)
   {
      for (t = 0; t < MAX_ENEMIES; t++)
      {
         if (enemy[t].exists == 0)
         {
            enemy[t].exists = 1;
            enemy[t].type = EYEBALL;
            enemy[t].x = (rnd() * (WIDTH - (SPRITE_SIZE * 2))) + SPRITE_SIZE;
            enemy[t].y = (rnd() * (lightningy - SPRITE_SIZE)) + (SPRITE_SIZE / 2);
            enemy[t].speedx = 0;
            enemy[t].speedy = 0;
            enemy[t].floatvar = 2.5 + ((float)(n + levelinfo.enemycount[WRAPBALL]) / 2);
            break;
         }
      }
   }
            
   return;
}

///////////////////////////////////////////////////////////////////////////////////

int playerdeath (void)      // Return 1 if player is dead.
{
   int komideadflag = 0;
   int n;

   if (lightningdeath())
   {
      komideadflag = 1;
      playsound(electricdeath_sound);
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
               playsound(electricdeath_sound);
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
      playsound(contactdeath_sound);
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

void cleanexit (int exitstatus)    // Exit cleanly, shutting down SDL.
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
// Load the sound if it exists.

void loadsound(Mix_Chunk ** thesound, char * directory, char * filename)
{
   char fullpath[TEXTBUFFERSIZE];
   
   FILE * testopen;

   if (strlen(directory) + strlen(filename) >= sizeof(fullpath))    // Check for buffer overflow on fullpath
   {
      fprintf(stderr, "Fatal error while loading %s:\n", filename);
      fprintf(stderr, "Size of directory name (%d chars) plus size of file name (%d chars)\n", strlen(directory), strlen(filename));
      fprintf(stderr, "is too long (over %d chars), and would cause a buffer overflow...\n", sizeof(fullpath) - 1);
      cleanexit(1);
   }
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

void playsound (Mix_Chunk * thesound)   // Pass sound pointer to Mix_PlayChannel() assuming sound is on.
{
   if (nosound == 0)
   {
      Mix_PlayChannel(-1, thesound, 0);
   }
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void drawmenu (int highlight_start, int highlight_quit)    // Draw all the graphical stuff on the title screen. Locations predefined.
                                                           // Highlight the buttons if so flagged.
{
   rects = 0;   // In case drawing all this would push rects over the max.

   cls(virtue, 0, 0, 0);
   drawsprite(&maintitle_title, virtue, MAINTITLE_X, MAINTITLE_Y);
   drawsprite(&gpl_title, virtue, GPL_X, GPL_Y);

   if (highlight_start == 0)
   {
      drawsprite(&start_title, virtue, STARTBUTTON_X, STARTBUTTON_Y);
   } else {
      drawsprite(&start2_title, virtue, STARTBUTTON_X, STARTBUTTON_Y);
   }
   
   if (highlight_quit == 0)
   {
      drawsprite(&quit_title, virtue, QUITBUTTON_X, QUITBUTTON_Y);
   } else {
      drawsprite(&quit2_title, virtue, QUITBUTTON_X, QUITBUTTON_Y);
   }
   
   drawsprite(&bolts_title, virtue, BOLTSTITLE_X, BOLTSTITLE_Y);
   drawsprite(&diamond_sprite, virtue, DIAMONDTITLE_X, DIAMONDTITLE_Y);
   drawsprite(&komi_sprite, virtue, KOMITITLE_X, KOMITITLE_Y);
   drawsprite(&diver_sprite, virtue, DIVERTITLE_X, DIVERTITLE_Y);
   
   drawsprite(&generator_sprite, virtue, (BOLTSTITLE_X - (bolts_title.pixelmap->w / 2)) - ((SPRITE_SIZE / 2) - (SPRITE_SIZE - LIGHTNINGADJUST)), BOLTSTITLE_Y);
   drawsprite(&generator_sprite, virtue, (BOLTSTITLE_X + (bolts_title.pixelmap->w / 2)) + ((SPRITE_SIZE / 2) - (SPRITE_SIZE - LIGHTNINGADJUST)), BOLTSTITLE_Y);
   
   drawsprite(&speed_title, virtue, SPEEDTITLE_X, SPEEDTITLE_Y);
   drawspeedrect();
   
   SDL_UpdateRect(virtue, 0, 0, 0, 0);
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void drawspeedrect (void)   // The rect showing the speed at the bottom left of the menu.
{
   rects = 0;   // In case drawing this would push rects over the max.

   int fillx1, fillx2, filly1, filly2;
   
   // Clear it first...
   frect(virtue, SPEEDRECTLEFT_X, SPEEDRECTTOP_Y, SPEEDRECTLEFT_X + SPEEDRECTWIDTH, SPEEDRECTTOP_Y + SPEEDRECTHEIGHT, 150, 150, 150);
   frect(virtue, SPEEDRECTLEFT_X + 1, SPEEDRECTTOP_Y + 1, SPEEDRECTLEFT_X + SPEEDRECTWIDTH - 1, SPEEDRECTTOP_Y + SPEEDRECTHEIGHT - 1, 0, 0, 0);
   if (delay < LONGESTDELAY)
   {
      fillx1 = SPEEDRECTLEFT_X;
      filly1 = SPEEDRECTTOP_Y;
      fillx2 = SPEEDRECTLEFT_X + ((LONGESTDELAY - delay) * ((float)SPEEDRECTWIDTH / LONGESTDELAY));
      filly2 = filly1 + SPEEDRECTHEIGHT;
   
      frect(virtue, fillx1, filly1, fillx2, filly2, 0, 100, 150);
   }
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void checkspeedadjust (void)
{
   int leftx, rightx, topy, bottomy;
   
   leftx = SPEEDRECTLEFT_X;
   topy = SPEEDRECTTOP_Y;
   rightx = SPEEDRECTLEFT_X + SPEEDRECTWIDTH;
   bottomy = SPEEDRECTTOP_Y + SPEEDRECTHEIGHT;
   
   if (mousemap.clickx > leftx - 20 && mousemap.clickx < rightx + 20 && mousemap.clicky > topy && mousemap.clicky < bottomy)
   {
      delay = LONGESTDELAY - ((mousemap.clickx - leftx) / ((float)SPEEDRECTWIDTH / LONGESTDELAY));
      if (delay < 0) delay = 0;
      if (delay > LONGESTDELAY) delay = LONGESTDELAY;
      saveprefs(filepath, "prefs");
      drawspeedrect();
      SDL_UpdateRect(virtue, 0, 0, 0, 0);
   }
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
      if (event.key.keysym.sym == LeftKey3)
         keymap.left3 = 1;
      if (event.key.keysym.sym == RightKey1)
         keymap.right1 = 1;
      if (event.key.keysym.sym == RightKey2)
         keymap.right2 = 1;
      if (event.key.keysym.sym == RightKey3)
         keymap.right3 = 1;
      if (event.key.keysym.sym == FireKey1)
         keymap.fire1 = 1;
      if (event.key.keysym.sym == FireKey2)
         keymap.fire2 = 1;
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
      if (event.key.keysym.sym == LeftKey3)
         keymap.left3 = 0;
      if (event.key.keysym.sym == RightKey1)
         keymap.right1 = 0;
      if (event.key.keysym.sym == RightKey2)
         keymap.right2 = 0;
      if (event.key.keysym.sym == RightKey3)
         keymap.right3 = 0;
      if (event.key.keysym.sym == FireKey1)
         keymap.fire1 = 0;
      if (event.key.keysym.sym == FireKey2)
         keymap.fire2 = 0;
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

void updatemousemap (void)   // Keeps track of whether the mouse button is down, and what position it clicked on.
{
   if (event.type == SDL_MOUSEBUTTONDOWN)
   {
      mousemap.button = 1;
      mousemap.clickx = event.button.x;
      mousemap.clicky = event.button.y;
   } else if (event.type == SDL_MOUSEBUTTONUP) {
      mousemap.button = 0;
   } else if (event.type == SDL_MOUSEMOTION) {
      mousemap.currentx = event.motion.x;
      mousemap.currenty = event.motion.y;
   }
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void manageevents (void)   // Manage relevant keyboard, mouse, and quit events.
{
   while (SDL_PollEvent(&event))
   {
      if (event.type == SDL_QUIT)
      {
         cleanexit(0);
      } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
         updatekeymap();
      } else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEMOTION) {
         updatemousemap();
      }
   }
   return;
}

///////////////////////////////////////////////////////////////////////////////////

int leveldone (void)     // Return 1 if there are no coins or diamonds on the level any more.
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
   
   if (SDL_MUSTLOCK(virtue)) SDL_LockSurface(virtue);
   
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
   
   if (SDL_MUSTLOCK(virtue)) SDL_UnlockSurface(virtue);
   
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

void doelectricity (void)  // Draw lightning between left and right sides, at vertical position lightningy.
{
   int n;
   int currentx, currenty;
   int nextx, nexty;
   int finishedthisline;
   int brightness;
   int de_lightningdeath_flag = 0;
   int brokenlightningflag = 0;
   
   if (SDL_MUSTLOCK(virtue)) SDL_LockSurface(virtue);
   
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
            line(virtue, currentx, currenty, nextx, nexty, brightness, brightness, 255);
            currentx = nextx;
            currenty = nexty;
            if (finishedthisline)
            {
               break;
            }
         }
      }
   }
   
   if (SDL_MUSTLOCK(virtue)) SDL_UnlockSurface(virtue);
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

int sign (float value)    // Returns sign of value, either -1 for negative, 1 for positive, or 0 for zero.
{
   if (value < 0) return -1;
   if (value > 0) return 1;
   return 0;
}

///////////////////////////////////////////////////////////////////////////////////

float rnd (void)    // Random number between 0 and 1.
{
   return (float)rand() / RAND_MAX;
}

///////////////////////////////////////////////////////////////////////////////////

                       // Called when Komi dies or level completed.
void fadeout (void)    // Not so much a fade as a series of black lines...

// Note that this function calls manageevents() repeatedly, and returns immediately if keymap.escape is set.
// The calling function can check keymap.escape to see if this is why it has returned.

{
   int n;
   
   if (SDL_MUSTLOCK(virtue)) SDL_LockSurface(virtue);    // Use of line() requires this.
   
   for (n = 0; n < HEIGHT; n += 2)
   {
      line(virtue, 0, n, WIDTH - 1, n, 0, 0, 0);
      if (n % 10 == 0)
      {
         if (SDL_MUSTLOCK(virtue)) SDL_UnlockSurface(virtue);
         SDL_UpdateRect(virtue, 0, 0, 0, 0);
         SDL_Delay(5);
         manageevents();
         if (keymap.escape)
         {
            return;
         }
         if (SDL_MUSTLOCK(virtue)) SDL_LockSurface(virtue);
      }
   }

   for (n = HEIGHT - 1; n > 0; n -= 2)
   {
      line(virtue, 0, n, WIDTH - 1, n, 0, 0, 0);
      if ((n + 1) % 10 == 0)
      {
         if (SDL_MUSTLOCK(virtue)) SDL_UnlockSurface(virtue);
         SDL_UpdateRect(virtue, 0, 0, 0, 0);
         SDL_Delay(5);
         manageevents();
         if (keymap.escape)
         {
            return;
         }
         if (SDL_MUSTLOCK(virtue)) SDL_LockSurface(virtue);
      }
   }
   
   if (SDL_MUSTLOCK(virtue)) SDL_UnlockSurface(virtue);
   
   SDL_UpdateRect(virtue, 0, 0, 0, 0);
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void updatetitlebar (void)
{
   char thestring[TEXTBUFFERSIZE];
   
   sprintf(thestring, "Komi  -  Level: %d  -  Score: %d  -  Lives: %d", level, score, lives);
   SDL_WM_SetCaption(thestring, NULL);
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void screenshot (SDL_Surface * surface, char * directory, char * filename)
{
   char fullpath[TEXTBUFFERSIZE];

   if (strlen(directory) + strlen(filename) >= sizeof(fullpath))    // Check for buffer overflow on fullpath
   {
      fprintf(stderr, "Error while saving %s:\n", filename);
      fprintf(stderr, "Size of directory name (%d chars) plus size of file name (%d chars)\n", strlen(directory), strlen(filename));
      fprintf(stderr, "is too long (over %d chars), and would cause a buffer overflow...\n", sizeof(fullpath) - 1);
      return;
   }
   strcpy(fullpath, directory);
   strcat(fullpath, filename);
   
   if (SDL_SaveBMP(surface, fullpath) == 0)
   {
      fprintf(stdout, "Saved screenshot to %s\n", fullpath);
      playsound(screenshot_sound);
   } else {
      fprintf(stdout, "Failed to save screenshot to %s\n", fullpath);
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void doelectrabolts (int x1, int y, int x2)   // Draw the Electra enemy's lightning between (x1, y) and (x2, y)
{
   int n;
   int currentx, currenty;
   int nextx, nexty;
   int finishedthisline;
   int brightness;
   int temp;

   if (SDL_MUSTLOCK(virtue)) SDL_LockSurface(virtue);

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
         line(virtue, currentx, currenty, nextx, nexty, 255, brightness, brightness);
         currentx = nextx;
         currenty = nexty;
         if (finishedthisline)
         {
            break;
         }
      }
   }
   
   if (SDL_MUSTLOCK(virtue)) SDL_UnlockSurface(virtue);
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void dobrokenlightning (int leftelectrax, int rightelectrax)   // Draw lightning between left and right sides
                                                               // But draw nothing between x and y.
                                                               // Called when Electras exist and are level with lightning.
{

   // Note that virtue should be locked before calling this.

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
         line(virtue, currentx, currenty, nextx, nexty, brightness, brightness, 255);
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

void drawskullpull (int clearflag, int n)   // Draw the circles from the Skull to Komi.
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

                               // Do whatever is done by the powerup Komi has just captured.
void goodieaction (int type)   // This function should play some sound, be it the standard eat sound or something else.
{
   switch (type)
   {
      case EXTRALIFE :
         lives++;
         updatetitlebar();
         playsound(oneup_sound);
         break;
      case SHOOTPOWER :
         shotsavailable += 3;
         playsound(eat_sound);
         break;
      case DESTRUCTOR :
         addcornershots();
         if (playedshootsound == 0)
         {
            playsound(shoot_sound);
            playedshootsound = 1;
         }
         break;
      case GAMEMOD :
         switch (intrnd(1))
         {
            case 0 :
               freeze = FREEZE_TIME;
               playsound(freeze_sound);
               break;
            case 1 :
               if (fastretract == 0)
               {
                  fastretract = 1;
                  playsound(eat_sound);
               } else {
                  freeze = FREEZE_TIME;
                  playsound(freeze_sound);
               }
               break;
            default :
               fprintf(stderr, "Warning: Got bad number from intrnd()\n");
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

void addcornershots (void)   // Add 4 inbound friendly shots - one at each corner. Used if we get the "destructor" powerup.
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

void addkomishot (void)    // Add an upward travelling friendly shot fired from Komi's position.
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
         friendlyshot[n].speedy = KOMI_SHOT_SPEED * -1;
         if (playedshootsound == 0)
         {
            playsound(shoot_sound);
            playedshootsound = 1;
         }
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

///////////////////////////////////////////////////////////////////////////////////

                           // Add num to score and update the titlebar.
void addscore (int num)    // Also checks if we're over the points required to get a new life.
{
   int initialscore;
   
   initialscore = score;
   score += num;
   if (initialscore / EXTRALIFEPOINTS < score / EXTRALIFEPOINTS)
   {
      lives++;
      playsound(oneup_sound);
   }
   updatetitlebar();
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void saveprefs (char * directory, char * filename)
{
   char fullpath[TEXTBUFFERSIZE];

   FILE * outfile;
   
   if (strlen(directory) + strlen(filename) >= sizeof(fullpath))    // Check for buffer overflow on fullpath
   {
      fprintf(stderr, "Error while saving %s:\n", filename);
      fprintf(stderr, "Size of directory name (%d chars) plus size of file name (%d chars)\n", strlen(directory), strlen(filename));
      fprintf(stderr, "is too long (over %d chars), and would cause a buffer overflow...\n", sizeof(fullpath) - 1);
      return;
   }
   strcpy(fullpath, directory);
   strcat(fullpath, filename);
   
   if ((outfile = fopen(fullpath, "wb")) == NULL)
   {
      fprintf(stderr, "Failed to open prefs file %s for save.\n", fullpath);
      return;
   } else {
      putc(PREFSVERSION, outfile);                    // Save preferences version (for check at load)
      putc(delay, outfile);                           // Save delay.
      putc(highscore & 0x000000FF, outfile);          // Save highscore...
      putc((highscore & 0x0000FF00) >> 8, outfile);
      putc((highscore & 0x00FF0000) >> 16, outfile);
      putc((highscore & 0xFF000000) >> 24, outfile);
      fclose(outfile);
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void loadprefs (char * directory, char * filename)
{
   char fullpath[TEXTBUFFERSIZE];

   FILE * infile;
   int nextchar;

   if (strlen(directory) + strlen(filename) >= sizeof(fullpath))    // Check for buffer overflow on fullpath
   {
      fprintf(stderr, "Error while loading %s:\n", filename);
      fprintf(stderr, "Size of directory name (%d chars) plus size of file name (%d chars)\n", strlen(directory), strlen(filename));
      fprintf(stderr, "is too long (over %d chars), and would cause a buffer overflow...\n", sizeof(fullpath) - 1);
      return;
   }
   strcpy(fullpath, directory);
   strcat(fullpath, filename);
   
   if ((infile = fopen(fullpath, "rb")) == NULL)
   {
      fprintf(stderr, "Failed to open prefs file %s for load.\n", fullpath);
      fprintf(stderr, "Attempting to write defaults to it...\n");
      saveprefs(directory, filename);                                     // If we fail, try to save / create the file instead.
      return;
   } else {
   
      if ((nextchar = getc(infile)) != EOF)
      {
         if (nextchar != PREFSVERSION)                 // Check that preferences are of the current type.
         {
            fprintf(stderr, "Preferences file appears to be in wrong format,\n");
            fprintf(stderr, "so attempting to write defaults to it...\n");
            fclose(infile);
            saveprefs(directory, filename);
            return;
         }
      }
      
      if ((nextchar = getc(infile)) != EOF)
      {
         if (nextchar >= 0 && nextchar <= 50 && gotdelayarg == 0)         // Check for sane delay values and gotdelayarg flag.
         {
            delay = nextchar;
         }
      }
      
      if ((nextchar = getc(infile)) != EOF)                // Read highscore in byte by byte.
         highscore = nextchar;
      if ((nextchar = getc(infile)) != EOF)
         highscore += nextchar << 8;
      if ((nextchar = getc(infile)) != EOF)
         highscore += nextchar << 16;
      if ((nextchar = getc(infile)) != EOF)
         highscore += nextchar << 24;

      fclose(infile);
   }
   
   return;
}

///////////////////////////////////////////////////////////////////////////////////

void setmaintitlebar (void)
{
   char thestring[TEXTBUFFERSIZE];
   
   sprintf(thestring, "Komi %s  -  Highscore: %d", VERSION, highscore);
   SDL_WM_SetCaption(thestring, NULL);
   return;
}

///////////////////////////////////////////////////////////////////////////////////

int playmusic (char * directory, char * filename, int loops)
{
   char fullpath[TEXTBUFFERSIZE];
   FILE * testopen;

   if (strlen(directory) + strlen(filename) >= sizeof(fullpath))    // Check for buffer overflow on fullpath
   {
      fprintf(stderr, "Error while loading %s:\n", filename);
      fprintf(stderr, "Size of directory name (%d chars) plus size of file name (%d chars)\n", strlen(directory), strlen(filename));
      fprintf(stderr, "is too long (over %d chars), and would cause a buffer overflow...\n", sizeof(fullpath) - 1);
      return 1;
   }
   strcpy(fullpath, directory);
   strcat(fullpath, filename);
   
   if (music != NULL)
   {
      fprintf(stderr, "Error: music != NULL.\n");
      fprintf(stderr, "Taking no action...\n");
      return 1;
   }

   if ((testopen = fopen(fullpath, "rb")) == NULL)
   {
      fprintf(stderr, "Failed to open %s\n", fullpath);
      return 1;
   } else {
      fclose(testopen);
   }
   
   music = Mix_LoadMUS(fullpath);
   if (music == NULL)
   {
      fprintf(stderr, "Error loading %s. %s\n", fullpath, Mix_GetError());
      return 1;
   }
   
   if (Mix_PlayMusic(music, loops))     // Try to play the music...
   {
      fprintf(stderr, "Mix_PlayMusic() returned an error.");
      music = NULL;
      return 1;
   }
   
   return 0;
}

///////////////////////////////////////////////////////////////////////////////////

void shufflelevels (void)
{
   int n;
   int first, second;
   int temp;
   
   for (n = 0; n < DEFINEDLEVELS; n++)
   {
      shuffledlevels[n] = n + 1;
   }
   
   for (n = 0; n < 1000; n++)
   {
      first = intrnd(DEFINEDLEVELS - 1);
      second = intrnd(DEFINEDLEVELS - 1);
      temp = shuffledlevels[first];
      shuffledlevels[first] = shuffledlevels[second];
      shuffledlevels[second] = temp;
   }
   
   return;
}

