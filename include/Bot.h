#pragma once
#include <signal.h>
#include <spdlog/spdlog.h>
#include <dpp/dpp.h>
#include "SQLConnector.h"



class Bot
{
private:
	SQLConnector& SQL_conn;
	dpp::cluster bot;
public:
	Bot(SQLConnector& SQL_conn, std::string token, dpp::intents intents);

	void createNewUser(dpp::snowflake userid, std::string username);
	void onMessageCreate(const dpp::message_create_t& event);
	void onReady(const dpp::ready_t& event);
	void onSlashCommand(const dpp::slashcommand_t& event);
	void onLog(const dpp::log_t& event);

	void start();
	void start_noreturn();

	void stop();
};

