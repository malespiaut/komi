/* loaders.h -- functions which load stuff */


void loadsprites (void)
{

   loadsprite(&komi_sprite, filepath, "sprites_komi.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   loadsprite(&tip_sprite, filepath, "sprites_tip.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   loadsprite(&fasttip_sprite, filepath, "sprites_fasttip.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   
   loadsprite(&tonguemask_sprite, filepath, "sprites_tonguemask.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   loadsprite(&pickupmask_sprite, filepath, "sprites_pickupmask.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   
   // To make things slightly easier, the coins/diamonds don't get black set as a no-collide colour here...
   loadsprite(&coin_sprite, filepath, "sprites_coin.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 255, 255, 255);
   loadsprite(&diamond_sprite, filepath, "sprites_diamond.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 255, 255, 255);
   
   loadsprite(&generator_sprite, filepath, "sprites_generator.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   
   loadsprite(&scrollerleft_sprite, filepath, "sprites_scrollerleft.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   spritemap[SCROLLERLEFT] = &scrollerleft_sprite;
   loadsprite(&scrollerright_sprite, filepath, "sprites_scrollerright.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   spritemap[SCROLLERRIGHT] = &scrollerright_sprite;
   loadsprite(&electra_sprite, filepath, "sprites_electra.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   spritemap[ELECTRA] = &electra_sprite;
   loadsprite(&roamer_sprite, filepath, "sprites_roamer.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   spritemap[ROAMER] = &roamer_sprite;
   loadsprite(&brownian_sprite, filepath, "sprites_brownian.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   spritemap[BROWNIAN] = &brownian_sprite;
   loadsprite(&lasergun_sprite, filepath, "sprites_lasergun.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   spritemap[LASERGUN] = &lasergun_sprite;
   loadsprite(&diver_sprite, filepath, "sprites_diver.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   spritemap[DIVER] = &diver_sprite;
   loadsprite(&skull_sprite, filepath, "sprites_skull.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   spritemap[SKULL] = &skull_sprite;
   loadsprite(&bouncer_sprite, filepath, "sprites_bouncer.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   spritemap[BOUNCER] = &bouncer_sprite;
   loadsprite(&gunner_sprite, filepath, "sprites_gunner.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   spritemap[GUNNER] = &gunner_sprite;
   loadsprite(&accelerator_sprite, filepath, "sprites_accelerator.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   spritemap[ACCELERATOR] = &accelerator_sprite;
   loadsprite(&dropper_sprite, filepath, "sprites_dropper.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   spritemap[DROPPER] = &dropper_sprite;
   loadsprite(&eyeball_sprite, filepath, "sprites_eyeball.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   spritemap[EYEBALL] = &eyeball_sprite;
   loadsprite(&wrapball_sprite, filepath, "sprites_wrapball.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   spritemap[WRAPBALL] = &wrapball_sprite;
   loadsprite(&bomber_sprite, filepath, "sprites_bomber.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   spritemap[BOMBER] = &bomber_sprite;
   loadsprite(&sniperleft_sprite, filepath, "sprites_sniperleft.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   spritemap[SNIPERLEFT] = &sniperleft_sprite;
   loadsprite(&sniperright_sprite, filepath, "sprites_sniperright.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   spritemap[SNIPERRIGHT] = &sniperright_sprite;

   loadsprite(&powerup_sprite, filepath, "sprites_powerup.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   powerupspritemap[GAMEMOD] = &powerup_sprite;
   
   loadsprite(&destructor_sprite, filepath, "sprites_destructor.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   powerupspritemap[DESTRUCTOR] = &destructor_sprite;
   
   loadsprite(&shootpower_sprite, filepath, "sprites_shootpower.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   powerupspritemap[SHOOTPOWER] = &shootpower_sprite;
   
   loadsprite(&life_sprite, filepath, "sprites_life.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   powerupspritemap[EXTRALIFE] = &life_sprite;

   loadsprite(&skullpull_sprite, filepath, "sprites_skullpull.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);

   loadsprite(&friendlyshot_sprite, filepath, "sprites_friendlyshot.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   loadsprite(&enemyshot_sprite, filepath, "sprites_enemyshot.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   
   loadsprite(&shooterkomi_sprite, filepath, "sprites_shooterkomi.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   loadsprite(&electrickomi_sprite, filepath, "sprites_electrickomi.bmp", SPRITE_SIZE, SPRITE_SIZE, 255, 255, 255, 0, 0, 0);
   
   loadsprite(&maintitle_title, filepath, "titles_title.bmp", 288, 80, 0, 0, 0, 0, 0, 0);
   loadsprite(&bolts_title, filepath, "titles_bolts.bmp", 152, 24, 0, 0, 0, 0, 0, 0);
   loadsprite(&start_title, filepath, "titles_start.bmp", 224, 48, 0, 0, 0, 0, 0, 0);
   loadsprite(&quit_title, filepath, "titles_quit.bmp", 96, 48, 0, 0, 0, 0, 0, 0);
   loadsprite(&start2_title, filepath, "titles_start2.bmp", 224, 48, 0, 0, 0, 0, 0, 0);
   loadsprite(&quit2_title, filepath, "titles_quit2.bmp", 96, 48, 0, 0, 0, 0, 0, 0);
   loadsprite(&gpl_title, filepath, "titles_gpl.bmp", 480, 112, 0, 0, 0, 0, 0, 0);
   loadsprite(&speed_title, filepath, "titles_speed.bmp", 136, 48, 0, 0, 0, 0, 0, 0);
   loadsprite(&shuffleoff_title, filepath, "titles_shuffleoff.bmp", 224, 48, 0, 0, 0, 0, 0, 0);
   loadsprite(&shuffleoff2_title, filepath, "titles_shuffleoff2.bmp", 224, 48, 0, 0, 0, 0, 0, 0);
   loadsprite(&shuffleon_title, filepath, "titles_shuffleon.bmp", 224, 48, 0, 0, 0, 0, 0, 0);
   loadsprite(&shuffleon2_title, filepath, "titles_shuffleon2.bmp", 224, 48, 0, 0, 0, 0, 0, 0);
   
   return;
}


///////////////////////////////////////////////////////////////////////////////////

void loadsounds (void)
{

   loadsound(&oneup_sound, filepath, "sounds_oneup.wav");
   loadsound(&contactdeath_sound, filepath, "sounds_contactdeath.wav");
   loadsound(&destructorkill_sound, filepath, "sounds_destructorkill.wav");
   loadsound(&eat_sound, filepath, "sounds_eat.wav");
   loadsound(&electricdeath_sound, filepath, "sounds_electricdeath.wav");
   loadsound(&freeze_sound, filepath, "sounds_freeze.wav");
   loadsound(&gameover_sound, filepath, "sounds_gameover.wav");
   loadsound(&laser_sound, filepath, "sounds_laser.wav");
   loadsound(&laserentry_sound, filepath, "sounds_laserentry.wav");
   loadsound(&laserpowerup_sound, filepath, "sounds_laserpowerup.wav");
   loadsound(&lastlife_sound, filepath, "sounds_lastlife.wav");
   loadsound(&lightningwarning_sound, filepath, "sounds_lightningwarning.wav");
   loadsound(&pause_sound, filepath, "sounds_pause.wav");
   loadsound(&powerup_sound, filepath, "sounds_powerup.wav");
   loadsound(&screenshot_sound, filepath, "sounds_screenshot.wav");
   loadsound(&shoot_sound, filepath, "sounds_shoot.wav");
   loadsound(&stick_sound, filepath, "sounds_stick.wav");
   loadsound(&unfreezewarning_sound, filepath, "sounds_unfreezewarning.wav");

   return;
}
