/* Komi definitions, structures, and global declarations... */

/* -------------------------------------------------------------------------- */
// Definitions...

#define VERSION "0.5a"

#define WIDTH 640
#define HEIGHT 480

#define ENEMYTYPES 13

#define SCROLLERLEFT 0
#define SCROLLERRIGHT 1
#define ELECTRA 2
#define FOLLOWER 3
#define ROAMER 4
#define BROWNIAN 5
#define LASERGUN 6
#define DIVER 7
#define SKULL 8
#define BOUNCER 9
#define GUNNER 10
#define ACCELERATOR 11
#define DROPPER 12

#define MAXSTARS 350
#define MAX_STARSPEED 10
#define YOUNG_STAR 45      // How long after birth do we reduce star brightness?
#define SLOWEST_STAR 0.3
#define DIM_STAR_BOUNDARY 0.5

#define START_LIVES 3
#define KOMI_SPEED 4

#define TONGUE_SPEED 6
#define TONGUE_WIDTH 6

#define TONGUE_R 79
#define TONGUE_G 106
#define TONGUE_B 226

#define SPRITE_SIZE 32
#define WALL 16

#define FORKSTODRAW 1
#define LIGHTNINGADJUST 26
#define BOLTMAXLENGTH 30
#define LIGHTNINGVARIANCE 8     // Vertical randomness...
#define START_LIGHTNING_Y 240
#define GEN_X 16
#define LIGHTNINGCHECK_DEFAULT 30
#define FASTLIGHTNINGCHECK_DEFAULT 15
#define DEFAULT_FASTLIGHTNINGY 320

#define MAX_ENEMIES 30
#define MAX_COINS 10
#define MAX_DIAMONDS 4
#define MAX_ENEMYSHOTS 5

#define LASERDURATION 20
#define LASERWARNTIME 60
#define LASERWIDTH 2
#define LASER_R 0
#define LASER_G 255
#define LASER_B 50
#define WARNLASER_R 0
#define WARNLASER_G 100
#define WARNLASER_B 0

#define SKULLY 352

#define MAINTITLE_X 320
#define MAINTITLE_Y 60

#define GPL_X 320
#define GPL_Y 145

#define BOLTSTITLE_X 320
#define BOLTSTITLE_Y 250

#define DIAMONDTITLE_X 360
#define DIAMONDTITLE_Y 220

#define KOMITITLE_X 290
#define KOMITITLE_Y 295

#define DIVERTITLE_X 270
#define DIVERTITLE_Y 205

#define STARTBUTTON_X 320
#define STARTBUTTON_Y 360

#define QUITBUTTON_X 320
#define QUITBUTTON_Y 420

#define COIN_SCORE 10
#define DIAMOND_SCORE 50

// Return values for playlevel()

#define LEVEL_COMPLETE 1
#define DEATH 2
#define QUIT 3

#define COIN 1
#define DIAMOND 2
#define POWERUP 3

/* -------------------------------------------------------------------------- */
// Structure definitions...

struct star_struct {
   float x; float y; int brightness; int age; float speed;};

struct sprite_struct {
   int width; int height; Uint8 * pixels; Uint8 * visible; bitmask * collisionmask;};

struct baddie_struct {
   int exists; int type; float x; float y; float speedx; float speedy; float floatvar; float floatvar2; int intvar; int intvar2;};

struct coin_struct {
   int exists; float x; float y; float speedx; float speedy;};

struct diamond_struct {
   int exists; float x; float y; float speedx; float speedy;};

struct goodie_struct {
   int exists; int type; float x; float y; float speedx; float speedy;};
   
struct enemyshot_struct {
   int exists; float x; float y; float speedx; float speedy;};

struct keymap_struct {
   int left1; int left2; int right1; int right2; int fire; int escape; int levelskip; int pause; int screenshot;};

struct mousemap_struct {
   int button; int clickx; int clicky;};

struct level_struct {
   int coins;
   int diamonds;
   int enemycount[ENEMYTYPES];
   
   int topdiamonds;
   
   int fastlightningy;
   int fastlightningcheck;
   
   float scrollerspeedx;
   int scrolleroffset;
   
   int diverhoverlevel;
   float divespeedy;
   float divespeedx;
   
   float roamerminspeed;
   float roamermaxspeed;

   float brownianmaxspeed;
   
   int guardianaccels;
   
   float gunnershootprob;
   float enemyshotbasespeed;
   
   int electrasflag;
   int electraoffset;
   
   int dropperhoveroffset;
   int dropperspeedy;
   };

/* -------------------------------------------------------------------------- */
// Globals...

SDL_Surface * virtue;
SDL_Event event;

int level;
int lives;

int fullspeedflag = 0;
int delay = 10;

int nostarsflag = 0;
int nosound = 0;
int cheats = 0;
int algorithmicenemies = 0;
int fullscreen = 0;

int komix;
int komiy;
int komispeedx;

int tonguelength;
int tonguespeed;
int fastretract;        // Not currently used.

int caughttype;
int caughtnumber;
int caughtoffsetx;
int caughtoffsety;

int lightningy;
int lightningcheck;     // Should merge this into levelinfo structure.

int tick;
int score;

int gameoverflag;
int resetmoney;

struct star_struct star[MAXSTARS];

struct level_struct levelinfo;

struct baddie_struct enemy[MAX_ENEMIES];
struct coin_struct coin[MAX_COINS];
struct diamond_struct diamond[MAX_DIAMONDS];
struct enemyshot_struct enemyshot[MAX_ENEMYSHOTS];

struct keymap_struct keymap = {0,0,0,0,0,0,0};
struct mousemap_struct mousemap = {0,0,0};

struct sprite_struct komi_sprite;
struct sprite_struct tip_sprite;
struct sprite_struct coin_sprite;
struct sprite_struct diamond_sprite;
struct sprite_struct generator_sprite;
struct sprite_struct scrollerleft_sprite;
struct sprite_struct scrollerright_sprite;
struct sprite_struct electra_sprite;
struct sprite_struct follower_sprite;
struct sprite_struct roamer_sprite;
struct sprite_struct brownian_sprite;
struct sprite_struct lasergun_sprite;
struct sprite_struct diver_sprite;
struct sprite_struct skull_sprite;
struct sprite_struct bouncer_sprite;
struct sprite_struct gunner_sprite;
struct sprite_struct dropper_sprite;
struct sprite_struct accelerator_sprite;
struct sprite_struct powerup_sprite;
struct sprite_struct destructor_sprite;
struct sprite_struct shootpower_sprite;
struct sprite_struct life_sprite;
struct sprite_struct shooterkomi_sprite;
struct sprite_struct shot_sprite;
struct sprite_struct enemyshot_sprite;
struct sprite_struct electrickomi_sprite;

struct sprite_struct skullpull_sprite;

struct sprite_struct tonguemask_sprite;
struct sprite_struct pickupmask_sprite;

struct sprite_struct * spritemap[ENEMYTYPES];

struct sprite_struct maintitle_title;
struct sprite_struct bolts_title;
struct sprite_struct start_title;
struct sprite_struct quit_title;
struct sprite_struct gpl_title;

Mix_Chunk * bounce_sound = NULL;
Mix_Chunk * contactdeath_sound = NULL;
Mix_Chunk * destructorkill_sound = NULL;
Mix_Chunk * eat_sound = NULL;
Mix_Chunk * electricdeath_sound = NULL;
Mix_Chunk * freezesound_sound = NULL;
Mix_Chunk * gameover_sound = NULL;
Mix_Chunk * laser_sound = NULL;
Mix_Chunk * laserentry_sound = NULL;
Mix_Chunk * laserpowerup_sound = NULL;
Mix_Chunk * lightningwarning_sound = NULL;
Mix_Chunk * oneup_sound = NULL;
Mix_Chunk * pause_sound = NULL;
Mix_Chunk * powerup_sound = NULL;
Mix_Chunk * shoot_sound = NULL;
Mix_Chunk * stick_sound = NULL;
Mix_Chunk * unfreezewarning_sound = NULL;

#ifdef DATAPATH
char filepath[256] = DATAPATH;
#else
char filepath[256] = "komidata/"
#endif


