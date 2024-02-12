#pragma once
#include <dpp/dpp.h>
#include <iomanip>
#include <sstream>

class AudioPlayer
{
private:
	dpp::cluster* bot_ptr;
	bool isPlaying = false;

public:
	AudioPlayer(dpp::cluster* bot) {
		bot_ptr = bot;
	}

	void AttemptPlay(const dpp::slashcommand_t& event) {
		if (isPlaying) {
			bot_ptr->log(dpp::ll_info, "Already playing audio");
			event.reply("Already playing audio");
			return;
		}
		isPlaying = true;

		bot_ptr->log(dpp::ll_info, "Attempting to play audio");
		uint8_t* sound = nullptr;
		size_t sound_size = 0;
		std::ifstream input("audio.pcm", std::ios::in | std::ios::binary | std::ios::ate);
		if (input.is_open()) {
			sound_size = input.tellg();
			sound = new uint8_t[sound_size];
			input.seekg(0, std::ios::beg);
			input.read((char*)sound, sound_size);
			input.close();
		}
		else {
			bot_ptr->log(dpp::ll_error, "Error opening audio file");
			isPlaying = false;
			return;
		}

		dpp::guild g = bot_ptr->guild_get_sync(1171622536937754654);
		if (!g.connect_member_voice(253066493514874881)) {
			bot_ptr->log(dpp::ll_error, "Error connecting to voice channel");
			event.reply("Error connecting to voice channel");
			isPlaying = false;
			return;
		}

		dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);
		if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
			bot_ptr->log(dpp::ll_error, "Error getting voice connection");
			event.reply("Error getting voice connection");
			isPlaying = false;
			return;
		}

		v->voiceclient->send_audio_raw((uint16_t*)sound, sound_size);

		isPlaying = false;
		bot_ptr->log(dpp::ll_info, "Audio sent");
		delete[] sound;
	}
};