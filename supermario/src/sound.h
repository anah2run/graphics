#pragma once
#pragma comment(lib, "irrKlang.lib")
#include "irrKlang\irrKlang.h"
using namespace irrklang;
ISoundEngine* engine = createIrrKlangDevice();
ISoundSource* mp3_src_bgm;
ISoundSource* mp3_src_gun;
ISoundSource* mp3_src_jump;
ISoundSource* mp3_src_step;
ISoundSource* mp3_src_enmhit;
ISoundSource* mp3_src_enmboom;
ISoundSource* mp3_src_blockhit;
ISoundSource* mp3_src_scream;
ISoundSource* mp3_src_heart;
ISoundSource* mp3_src_burned;
//
static const char* mp3_path_bgm = "../bin/sounds/breakout.mp3";
static const char* mp3_path_gun = "../bin/sounds/gun.mp3";
static const char* mp3_path_jump = "../bin/sounds/jump.mp3";
static const char* mp3_path_step = "../bin/sounds/step.ogg";
static const char* mp3_path_enmhit = "../bin/sounds/hit.wav";
static const char* mp3_path_enmboom = "../bin/sounds/boom.wav";
static const char* mp3_path_blockhit = "../bin/sounds/solid.wav";
static const char* mp3_path_scream = "../bin/sounds/scream.wav";
static const char* mp3_path_heart = "../bin/sounds/heart.wav";
static const char* mp3_path_burned = "../bin/sounds/burned.wav";
//
void init_sound() {
	mp3_src_bgm = engine->addSoundSourceFromFile(mp3_path_bgm);
	mp3_src_bgm->setDefaultVolume(0.5f);

	mp3_src_gun = engine->addSoundSourceFromFile(mp3_path_gun);
	mp3_src_gun->setDefaultVolume(0.5f);

	mp3_src_jump = engine->addSoundSourceFromFile(mp3_path_jump);
	mp3_src_jump->setDefaultVolume(0.5f);

	mp3_src_step = engine->addSoundSourceFromFile(mp3_path_step);
	mp3_src_step->setDefaultVolume(0.5f);

	mp3_src_enmhit = engine->addSoundSourceFromFile(mp3_path_enmhit);
	mp3_src_enmhit->setDefaultVolume(0.4f);

	mp3_src_enmboom = engine->addSoundSourceFromFile(mp3_path_enmboom);
	mp3_src_enmboom->setDefaultVolume(0.3f);

	mp3_src_blockhit = engine->addSoundSourceFromFile(mp3_path_blockhit);
	mp3_src_blockhit->setDefaultVolume(0.5f);

	mp3_src_scream = engine->addSoundSourceFromFile(mp3_path_scream);
	mp3_src_scream->setDefaultVolume(0.7f);

	mp3_src_burned = engine->addSoundSourceFromFile(mp3_path_burned);
	mp3_src_burned->setDefaultVolume(0.3f);

	mp3_src_heart = engine->addSoundSourceFromFile(mp3_path_heart);
	mp3_src_heart->setDefaultVolume(0.4f);
}
