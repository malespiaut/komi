// From Komi.c

void menu (void);
void game (void);
int playlevel (void);
void dostars (void);
void resetstar (int n);
void drawsprites (void);
void clearsprites (void);
void movesprites (void);
void doelectricity (void);
float rnd (void);
void blanklevel (void);
void makelevel (void);
int sign (float value);
int playerdeath (void);
int lightningdeath (void);
void cleanexit (int exitstatus);
void loadsound(Mix_Chunk ** thesound, char * directory, char * filename);
void playsound (Mix_Chunk * thesound);
void drawmenu (void);
void updatekeymap (void);
void updatemousemap (void);
void manageevents (void);
void checktonguepickup (void);
int leveldone (void);
void choosenumbers (void);
void fadeout (void);
void updatetitlebar (void);
void screenshot (SDL_Surface * surface, char * directory, char * filename);
void doelectrabolts (int x1, int y, int x2);
void dobrokenlightning (int leftelectrax, int rightelectrax);
void drawskullpull (int clearflag, int n);
void algorithmicenemynumbers (int thislevel);
void goodieaction (int type);
void addcornershots (void);
void addkomishot (void);
int intrnd (int max);
void addscore (int num);
void drawspeedrect (void);
void checkspeedadjust (void);
void loadprefs(char * directory, char * filename);
void saveprefs(char * directory, char * filename);
void setmaintitlebar (void);

void movescroller (int n);
void movediver (int n);
void movebouncer (int n);
void moveroamer (int n);
void movebrownian (int n);
void moveaccelerator (int n);
void movegunner (int n);
void moveelectra (int n);
void movelasergun (int n);
void moveskull (int n);
void movedropper (int n);
void moveeyeball (int n);
void movewrapball (int n);

// From sprites.h

int sprite_collision (struct sprite_struct * sprite1, int x1, int y1, struct sprite_struct * sprite2, int x2, int y2);
void init_spriteimagemap(struct sprite_struct * thesprite, int width, int height);
void drawsprite (struct sprite_struct * thesprite, SDL_Surface * screen, int centrex, int centrey);
void clearsprite (struct sprite_struct * thesprite, SDL_Surface * screen, int centrex, int centrey);
void loadsprite (struct sprite_struct * thesprite, char * directory, char * filename, int width, int height, int hider, int hideb, int hideg, int nocollider, int nocollideg, int nocollideb);

// From loaders.h

void loadsprites (void);
void loadsounds (void);

// From info.h

void printcopyinfo (void);
void printhelp (void);
void printversion (void);

// From gfx.h

void putpixel(SDL_Surface * surface, int x, int y, Uint32 pixel);
void line (SDL_Surface * bitmapstruct, int x1, int y1, int x2, int y2, int red, int green, int blue);
void rect (SDL_Surface * bitmapstruct, int x1, int y1, int x2, int y2, int red, int green, int blue);
void frect (SDL_Surface * bitmapstruct, int x1, int y1, int x2, int y2, int red, int green, int blue);
void cls (SDL_Surface * bitmapstruct, int red, int green, int blue);
void updaterectsarray (int leftx, int topy, int width, int height);
