/* Komi definitions, structures, and global declarations... */

/* -------------------------------------------------------------------------- */
// Definitions...

#define VERSION "1.04"
#define PREFSVERSION 1    // Increment this if prefs format changes.
#define PREFSNAME ".komirc"

// Window size.
#define WIDTH 640
#define HEIGHT 480

// Number of predefined levels.
#define DEFINEDLEVELS 30

// Delay between frames (default)
#define DEFAULT_DELAY 10

// Different enemy types. Should probably use an enum.
#define SCROLLERLEFT 0
#define SCROLLERRIGHT 1
#define ELECTRA 2
#define ACCELERATOR 3
#define ROAMER 4
#define BROWNIAN 5
#define LASERGUN 6
#define DIVER 7
#define SKULL 8
#define BOUNCER 9
#define GUNNER 10
#define DROPPER 11
#define EYEBALL 12
#define WRAPBALL 13
#define BOMBER 14
#define SNIPERLEFT 15
#define SNIPERRIGHT 16
#define ENEMYTYPES 17

// Starfield constants.
#define MAXSTARS 350
#define MAX_STARSPEED 10
#define YOUNG_STAR 45      // How long after birth do we reduce star brightness?
#define SLOWEST_STAR 0.3
#define DIM_STAR_BOUNDARY 0.5

// Starting stats.
#define START_LIVES 3
#define KOMI_SPEED 4
#define TONGUE_SPEED 6

// Tongue colour.
#define TONGUE_R 79
#define TONGUE_G 106
#define TONGUE_B 226

#define TONGUE_FAST_R 255
#define TONGUE_FAST_G 0
#define TONGUE_FAST_B 0

#define TONGUE_WIDTH 6

#define KOMI_SHOT_SPEED 3

// Size of all sprites. Mostly used for bouncing enemies off the sides.
#define SPRITE_SIZE 32

// How close to the edges Komi can go.
#define WALL 16

// Constants for the lightning.
#define FORKSTODRAW 1
#define LIGHTNINGADJUST 26             // Actual distance of start/finish from the left/right edges.
#define BOLTMAXLENGTH 30
#define LIGHTNINGVARIANCE 8            // Vertical randomness.
#define START_LIGHTNING_Y 240          // What vertical position the lightning starts at.
#define GEN_X 16                       // Location to draw the generator sprite.
#define LIGHTNINGCHECK_DEFAULT 30      // Default for how often the lightning gets moved down a pixel. Once every 30 frames.
#define FASTLIGHTNINGCHECK_DEFAULT 15  // Once it reaches the "fast" level (below) it (by default) goes once every 15 frames.
#define DEFAULT_FASTLIGHTNINGY 320

// How many arrays of structures to make (which hold stuff like location, speed, etc).
#define MAX_ENEMIES 30
#define MAX_COINS 10
#define MAX_DIAMONDS 4
#define MAX_ENEMYSHOTS 10
#define MAX_FRIENDLYSHOTS 10

#define GUNNERRELOADTIME 5           // Minimum time between shots for the Gunner.

#define BASEPOWERUPPROB 0.00005      // Default chance of a powerup appearing per frame.
#define POWERUPCUTOFFTIME 10000      // No powerups after this many ticks.

#define FREEZE_TIME 600              // How many ticks a freeze lasts.
#define FREEZE_WARN_INTERVAL 60      // Intervals between freeze warning sounds.
#define FREEZE_WARNINGS 3            // How many warnings.

// Constants for the Lasergun enemy.
#define LASERDURATION 20        // Frames the laser lasts.
#define LASERWARNTIME 60        // Give warning this many frames before.
#define LASERWIDTH 2
#define LASER_R 255             // Colours for laser.
#define LASER_G 50
#define LASER_B 0
#define WARNLASER_R 120         // Colours for warning pre-laser.
#define WARNLASER_G 50
#define WARNLASER_B 0

#define SKULLY 352              // Vertical position for Skull enemy.

// Enemy default speeds etc (can be over-ruled when levelinfo is made)
#define BROWNIAN_MAX_SPEED_DEFAULT 1.5
#define ROAMER_MIN_SPEED_DEFAULT 0.5
#define ROAMER_MAX_SPEED_DEFAULT 1
#define DROPPER_SPEEDY_DEFAULT 5
#define ELECTRA_OFFSET_DEFAULT -32
#define GUNNER_SHOOT_PROB_DEFAULT 0.01
#define DIVE_SPEEDY_DEFAULT 4
#define DIVE_SPEEDX_DEFAULT 2
#define ENEMY_SHOT_SPEED_DEFAULT 3
#define BOMB_SPEED_DEFAULT 5

#define SNIPERSHOTSPEED 3

// Points...
#define COIN_SCORE 10           // Points for a coin.
#define DIAMOND_SCORE 50        // Points for a diamond.
#define EXTRALIFEPOINTS 1000    // New life every x points.

// Locations of the various images on the title screen.
#define MAINTITLE_X 320
#define MAINTITLE_Y 60
#define GPL_X 320
#define GPL_Y 175
#define BOLTSTITLE_X 370
#define BOLTSTITLE_Y 325
#define DIAMONDTITLE_X 410
#define DIAMONDTITLE_Y 295
#define KOMITITLE_X 340
#define KOMITITLE_Y 370
#define DIVERTITLE_X 320
#define DIVERTITLE_Y 280
#define QUITBUTTON_X 585
#define QUITBUTTON_Y 385
#define STARTBUTTON_X 520
#define STARTBUTTON_Y 445
#define SPEEDTITLE_X 85
#define SPEEDTITLE_Y 445
#define SHUFFLETITLE_X 129
#define SHUFFLETITLE_Y 385

#define SPEEDRECTLEFT_X 170
#define SPEEDRECTTOP_Y 435
#define SPEEDRECTWIDTH 150
#define SPEEDRECTHEIGHT 20

#define LONGESTDELAY 20      // Delay of this or slower makes the speed rect at the title screen all black.
                             // Can still get longer delays with command line arguments.

// Return values for playlevel()
#define LEVEL_COMPLETE 1
#define DEATH 2
#define QUIT 3

// Values to differentiate between different objects caught by the tongue.
#define COIN 1
#define DIAMOND 2
#define POWERUP 3

// Different powerup types.
#define EXTRALIFE 0
#define SHOOTPOWER 1
#define GAMEMOD 2
#define DESTRUCTOR 3
#define POWERUPTYPES 4

// How many rectanges to keep track of if doing fast drawing, where only
// updated rectanges are redrawn.
#define MAXRECTS 200

#define TEXTBUFFERSIZE 1024

#define YES 1
#define NO 0

/* -------------------------------------------------------------------------- */
// Structure definitions...

struct star_struct {
   float x; float y; int brightness; int age; float speed;};

struct sprite_struct {
   SDL_Surface * pixelmap; bitmask_t * collisionmask;};

struct baddie_struct {
   int exists; int type; float x; float y; float speedx; float speedy; float floatvar; int intvar; int intvar2;};

// Note: the different enemies use the float variable ("floatvar") and the 2 int variables
// ("intvar") for different things. See comments to makelevel() in komi.c

struct coin_struct {
   int exists; float x; float y; float speedx; float speedy;};

struct diamond_struct {
   int exists; float x; float y; float speedx; float speedy;};

// The powerups.
struct goodie_struct {
   int exists; int type; float x; float y; float speedx; float speedy;};
   
struct enemyshot_struct {
   int exists; float x; float y; float speedx; float speedy;};

struct friendlyshot_struct {
   int exists; float x; float y; float speedx; float speedy;};

// Holds flags for whether these keys are down or up.
struct keymap_struct {
   int left1; int left2; int left3; int right1; int right2; int right3; int fire1; int fire2; int escape; int levelskip; int pause; int screenshot;};

// Holds flag for whether the mouse is down, and what location it clicked on.
struct mousemap_struct {
   int button; int clickx; int clicky; int currentx; int currenty;};

// Settings that may change for each level.
struct level_struct {
   int coins;                     // How many coins...
   int diamonds;                  //          diamonds...
   int enemycount[ENEMYTYPES];    //          enemies of the various types...
   
   int topdiamonds;               // Flag for whether diamonds move only along the top.
   
   int fastlightningy;            // At what position does the lightning speed up?
   int fastlightningcheck;        // Speed up to moving down a pixel once every x frames.
   
   float scrollerspeedx;          // Speed for Scroller enemy type (the rockets)
   int scrolleroffset;            // Their offset from the lightning.
   
   int diverhoverlevel;           // Where the Divers hover when not diving.
   float divespeedy;              // Diver speed during a dive.
   float divespeedx;
   
   int bomberhoverlevel;          // Y location for the Bomber enemy type.
   int bombspeed;                 // Speed its shots travel.
   
   int snipersflag;               // Are there snipers in the level?
   
   float roamerminspeed;          // Min speed for Roamers (also used for some other enemies)
   float roamermaxspeed;          // And the max speed.

   float brownianmaxspeed;        // Max speed for the Brownian motion enemy.
   
   int guardianaccels;            // Flag for whether Accelerator enemy sticks near the lightning.
   
   float gunnershootprob;         // Chance per frame of gunner shooting.
   float enemyshotbasespeed;      // Speed of shot.
   
   int electrasflag;              // Whether there are Electras in the level.
   int electraoffset;             // Their offset from the lightning.
   
   int dropperhoveroffset;        // Offset of Dropper enemies from the lightning.
   int dropperspeedy;             // Their diving speed.
   
   float powerup_prob[POWERUPTYPES];   // Probability of given powerup type appearing per frame.
   };

/* -------------------------------------------------------------------------- */
// Globals...

SDL_Surface * virtue;            // The SDL surface that we draw to.
SDL_Event event;                 // For getting the last event.

SDL_Rect updaterect[MAXRECTS];   // Array of rectanges used if --fastdraw option is on. What areas to redraw this frame.
int rects;                       // Number of rectanges to redraw this frame. Passed to SDL_UpdateRects().

int level;                       // Current level.
int lives;                       // Current lives.

int hog = 0;                     // Flag for whether to use own delaying function which doesn't give up CPU.
int delay = DEFAULT_DELAY;       // Delay between frames.

// Various options that the user can set with command line arguments.
int nostarsflag = 0;
int nosound = 0;
int nomusic = 0;
int cheats = 0;
int invincible = 0;
int shuffle = 0;
int algorithmicenemies = 0;
int fullscreen = 0;
int fastdraw = 0;
int gfxdetails = 0;

int gotdelayarg = 0;    // Flag raised if "--delay" is used; prevents delay being read from prefs file.

int shuffledlevels[DEFINEDLEVELS];

// Position of Komi.
int komix;
int komiy;

// Tongue.
int tonguelength;       // Current tongue length (0 if tongue fully retracted).
int tonguespeed;        // Speed the tongue extends/falls at.

int caughttype;         // What type of object (if any) caught by the tongue.
int caughtnumber;       // Caught object's number (ie it could be coin 3).
int caughtoffsetx;      // Its x-distance from the tongue-tip when caught.
int caughtoffsety;      // And its y-distance.

int shotsavailable;     // How many shots Komi can shoot. Usually zero.
int fastretract;        // Not currently used. Would be flag for faster tongue retraction.
int freeze;             // Freeze enemy movement.

int playedshootsound;   // Gets set to 0 every tick, then to 1 when a shot is fired. Prevents multiple playback in a single tick.

int lightningy;         // Position of lightning.
int lightningcheck;     // Should merge this into levelinfo structure. Move lightning down one pixel every x frames.

int tick;               // Current frame number.
int score;              // Current score.
int highscore = 0;      // Highest score achieved.
int lastscore = 0;      // Last score this session.

int resetmoney;         // Flag for whether to remake money at level start.
                        // Will be Yes if this is a new level, No if we're replaying one we died in.

int givelastlifewarning = 0;    // Whether to give a warning upon next start of level due to last life (if in fullscreen mode)

int havecheated;        // Whether player has cheated this game. Affects whether highscore is saved.

// Now make all the relevant structures declared above.

struct star_struct star[MAXSTARS];

struct level_struct levelinfo;

struct baddie_struct enemy[MAX_ENEMIES];
struct coin_struct coin[MAX_COINS];
struct diamond_struct diamond[MAX_DIAMONDS];
struct enemyshot_struct enemyshot[MAX_ENEMYSHOTS];
struct friendlyshot_struct friendlyshot[MAX_FRIENDLYSHOTS];

struct goodie_struct goodie;

struct keymap_struct keymap = {0,0,0,0,0,0,0,0,0,0,0,0};
struct mousemap_struct mousemap = {0,0,0,0,0};

// All the sprite types get their own structure.
struct sprite_struct komi_sprite;
struct sprite_struct tip_sprite;
struct sprite_struct fasttip_sprite;
struct sprite_struct coin_sprite;
struct sprite_struct diamond_sprite;
struct sprite_struct generator_sprite;
struct sprite_struct scrollerleft_sprite;
struct sprite_struct scrollerright_sprite;
struct sprite_struct electra_sprite;
struct sprite_struct roamer_sprite;
struct sprite_struct brownian_sprite;
struct sprite_struct lasergun_sprite;
struct sprite_struct diver_sprite;
struct sprite_struct skull_sprite;
struct sprite_struct bouncer_sprite;
struct sprite_struct gunner_sprite;
struct sprite_struct dropper_sprite;
struct sprite_struct accelerator_sprite;
struct sprite_struct eyeball_sprite;
struct sprite_struct wrapball_sprite;
struct sprite_struct bomber_sprite;
struct sprite_struct sniperleft_sprite;
struct sprite_struct sniperright_sprite;
struct sprite_struct powerup_sprite;
struct sprite_struct destructor_sprite;
struct sprite_struct shootpower_sprite;
struct sprite_struct life_sprite;
struct sprite_struct shooterkomi_sprite;
struct sprite_struct friendlyshot_sprite;
struct sprite_struct enemyshot_sprite;
struct sprite_struct electrickomi_sprite;

struct sprite_struct skullpull_sprite;

struct sprite_struct tonguemask_sprite;
struct sprite_struct pickupmask_sprite;

// Will hold pointers to the different sprites so we can access each type by number.
struct sprite_struct * spritemap[ENEMYTYPES];
struct sprite_struct * powerupspritemap[POWERUPTYPES];

struct sprite_struct maintitle_title;
struct sprite_struct bolts_title;
struct sprite_struct start_title;
struct sprite_struct quit_title;
struct sprite_struct start2_title;
struct sprite_struct quit2_title;
struct sprite_struct gpl_title;
struct sprite_struct speed_title;
struct sprite_struct shuffleoff_title;
struct sprite_struct shuffleoff2_title;
struct sprite_struct shuffleon_title;
struct sprite_struct shuffleon2_title;

// All the sounds...
Mix_Chunk * contactdeath_sound = NULL;
Mix_Chunk * destructorkill_sound = NULL;
Mix_Chunk * eat_sound = NULL;
Mix_Chunk * electricdeath_sound = NULL;
Mix_Chunk * freeze_sound = NULL;
Mix_Chunk * gameover_sound = NULL;
Mix_Chunk * laser_sound = NULL;
Mix_Chunk * laserentry_sound = NULL;
Mix_Chunk * laserpowerup_sound = NULL;
Mix_Chunk * lastlife_sound = NULL;
Mix_Chunk * lightningwarning_sound = NULL;
Mix_Chunk * oneup_sound = NULL;
Mix_Chunk * pause_sound = NULL;
Mix_Chunk * powerup_sound = NULL;
Mix_Chunk * screenshot_sound = NULL;
Mix_Chunk * shoot_sound = NULL;
Mix_Chunk * stick_sound = NULL;
Mix_Chunk * unfreezewarning_sound = NULL;

// Music...
Mix_Music * music = NULL;

// Path to the data (sound files, graphics, etc). 
#ifdef DATAPATH
char filepath[TEXTBUFFERSIZE] = DATAPATH;
char prefsdir[TEXTBUFFERSIZE] = DATAPATH;
#else
char filepath[TEXTBUFFERSIZE] = "komidata/";
char prefsdir[TEXTBUFFERSIZE] = "komidata/";
#endif
