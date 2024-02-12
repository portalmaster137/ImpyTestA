#pragma once

#include <signal.h>
#include <spdlog/spdlog.h>
#include <dpp/dpp.h>
#include "SQLConnector.h"
#include "dotenv.h"

namespace Util {

	inline int calculateXPFromMessage(dpp::message msg) {
		std::string text = msg.content;
		int xp = text.length() / 3;
		if (xp > 150) {
			xp = 150;
		}
		int atthXp = msg.attachments.size() * 50;
		if (atthXp > 150) {
			atthXp = 150;
		}
		xp += atthXp;
		return xp;
	}

	inline int getLevelFromXP(int xp) {
		double level = 0.1 * sqrt(xp);
		return static_cast<int>(level);
	}

	struct EnvConfig {
		std::string host;
		std::string user;
		std::string password;
		std::string database;
		std::string token;
		unsigned int port;
	};

	inline EnvConfig AttemptGetEnv() {
		std::string DATABASE_HOST = dotenv::getenv("DATABASE_HOST");
		std::string DATABASE_USERNAME = dotenv::getenv("DATABASE_USERNAME");
		std::string DATABASE_PASSWORD = dotenv::getenv("DATABASE_PASSWORD");
		std::string DATABASE_NAME = dotenv::getenv("DATABASE_NAME");
		std::string TOKEN = dotenv::getenv("TOKEN");
		//port by default is 3306
		//so either use dotenv::getenv("PORT") or 3306 if PORT is not set
		std::string PORT = dotenv::getenv("PORT");
		if (PORT.empty()) {
			PORT = "3306";
		}
		unsigned int PORT_INT = std::stoi(PORT);

		if (DATABASE_HOST.empty() || DATABASE_USERNAME.empty() || DATABASE_PASSWORD.empty() || DATABASE_NAME.empty() || TOKEN.empty()) {
			spdlog::error("One or more environment variables are not set");
			throw std::runtime_error("One or more environment variables are not set");
		}

		return { DATABASE_HOST, DATABASE_USERNAME, DATABASE_PASSWORD, DATABASE_NAME, TOKEN, PORT_INT };
	}

}