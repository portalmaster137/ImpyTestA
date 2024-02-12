#include "Bot.h"
#include "Utils.h"
#include "SlashCommandHandler.h"

Bot::Bot(SQLConnector& SQL_conn, std::string token, dpp::intents intents) : SQL_conn(SQL_conn), bot(token, intents) {
	
	bot.on_log([this](const dpp::log_t& event) {
		onLog(event);
	});

	bot.on_message_create([this](const dpp::message_create_t& event) {
		onMessageCreate(event);
	});

	bot.on_ready([this](const dpp::ready_t& event) {
		onReady(event);
	});

	bot.on_slashcommand([this](const dpp::slashcommand_t& event) {
		onSlashCommand(event);
	});

}

void Bot::onLog(const dpp::log_t& event) {
	switch (event.severity) {
		case dpp::ll_debug:
			spdlog::debug(event.message);
			break;
		case dpp::ll_info:
			spdlog::info(event.message);
			break;
		case dpp::ll_warning:
			spdlog::warn(event.message);
			break;
		case dpp::ll_error:
			spdlog::error(event.message);
			break;
		default:
			spdlog::critical(event.message);
			break;

	}
}

void Bot::createNewUser(dpp::snowflake userid, std::string username) {
	std::string query = "INSERT INTO User (idUser, Username) VALUES (" + std::to_string(userid) + ", '" + username + "')";
	SQL_conn.ExecuteNoReturnQuery(query.c_str(), true);
}

void Bot::onMessageCreate(const dpp::message_create_t& event) {
	
	if (event.msg.author.is_bot()) return;
	std::string query = "SELECT * FROM User WHERE idUser = " + std::to_string(event.msg.author.id);
	try {
		MYSQL* conn = SQL_conn.getConnection();
		MYSQL_RES* res;
		MYSQL_ROW row;

		try {
		if (mysql_query(conn, query.c_str())) {
			spdlog::debug("Error: {}", mysql_error(conn));
			return;
		}
		}
		catch (std::exception& e) {
			spdlog::critical("Error: {}", e.what());
			//EXIT PANIC OH GOD FUCK FUCK FUCK WHAT DO I DO???
			exit(1);
		}
		res = mysql_store_result(conn);
		if (res) {
			row = mysql_fetch_row(res);
			if (row) {
				int xp = Util::calculateXPFromMessage(event.msg);
				if (xp == 0) return;
				std::string updateQuery = "UPDATE User SET UserXP = UserXP + " + std::to_string(xp) + " WHERE idUser = " + std::to_string(event.msg.author.id);
				SQL_conn.ExecuteNoReturnQuery(updateQuery.c_str(), false);
				return;
			}
			else {
				spdlog::debug("User not found in database");
				mysql_free_result(res);
				createNewUser(event.msg.author.id, event.msg.author.username);
				return;
			}
		}
		mysql_free_result(res);

	}
	catch (std::exception& e) {
		spdlog::error("Error: {}", e.what());
	}
	
}

void Bot::onReady(const dpp::ready_t& event) {
	if (dpp::run_once<struct register_bot_commands>()) {

		//delete any global commands

		dpp::slashcommand ping("ping", "Ping pong!", bot.me.id);
		//dpp::slashcommand checkdb("checkdb", "Check the Database Connection", bot.me.id);
		dpp::slashcommand exp("experience", "Check how much XP you got", bot.me.id);
		dpp::slashcommand lead("leaderboard", "mmmmm sweet sweet leaderboard", bot.me.id);

		dpp::slashcommand givecmd("give_xp", "Give a User some XP", bot.me.id);
		givecmd.add_option(dpp::command_option(dpp::command_option_type::co_user, "user", "The User to give XP to", true));
		givecmd.add_option(dpp::command_option(dpp::command_option_type::co_integer, "amount", "The Amount of XP", true));

		bot.guild_bulk_command_create_sync({ ping, exp, lead, givecmd }, 1171622536937754654);

		spdlog::debug("Slash Commands Registered");
	}
	spdlog::info("Bot is ready");
	bot.set_presence(dpp::presence(dpp::presence_status::ps_online, dpp::activity_type::at_watching, "the Database"));
}

void Bot::onSlashCommand(const dpp::slashcommand_t& event) {
	SlashCommandHandler::Handle(event, &bot, &SQL_conn);
}

void Bot::start() {
	bot.start(true);
}

void Bot::start_noreturn() {
	bot.start(false);
}

void Bot::stop() {
	bot.shutdown();
}