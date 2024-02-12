#include <iostream>
#include <spdlog/spdlog.h>
#include <cmath>

#include "dotenv.h"
#include "SQLConnector.h"
#include "Bot.h"
#include "Utils.h"

Bot* bot_ptr = nullptr;

void sigHandle(int signum) {
	spdlog::warn("[SIGTERM RECV]");

	if (bot_ptr) {
		bot_ptr->stop();
	}

	spdlog::warn("Gracefull shutdown complete");
	exit(signum);
}

int main()
{
#if _DEBUG
	dotenv::init("C:\\Users\\porta\\source\\repos\\ImpyTestA\\.env");
#else
	dotenv::init();
#endif

	spdlog::set_level(spdlog::level::debug);

	Util::EnvConfig config = Util::AttemptGetEnv();

	spdlog::debug("Successfully retrieved environment variables");

	SQLConnector SQL_conn(config.host.c_str(), config.user.c_str(), config.password.c_str(), config.database.c_str(), config.port);


	Bot bot(SQL_conn, config.token, dpp::intents::i_all_intents);
	bot_ptr = &bot;
	
	std::signal(SIGINT, sigHandle);

	bot.start_noreturn();

	return 0;

}
