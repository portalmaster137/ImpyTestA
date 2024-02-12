#pragma once

#include <dpp/dpp.h>
#include <spdlog/spdlog.h>

#include "SQLConnector.h"
#include "Utils.h"

namespace SlashCommandHandler {

	inline void Handle(const dpp::slashcommand_t& event, dpp::cluster* bot, SQLConnector* SQL_conn) {
		if (event.command.get_command_name() == "ping") {
			event.reply("Pong!");
			return;
		}

		if (event.command.get_command_name() == "checkdb") {
			if (SQL_conn->connected()) {
				event.reply("Database is connected");
			}
			else {
				event.reply("Database is not connected");
			}
			return;
		}

		if (event.command.get_command_name() == "experience") {
			event.thinking();
			std::string query = "SELECT UserXP FROM User WHERE idUser = " + std::to_string(event.command.member.user_id);
			MYSQL_ROW row;
			MYSQL_RES* res = SQL_conn->ExecuteReturnQuery(query.c_str());
			if (res) {
				row = mysql_fetch_row(res);
				if (row) {
					event.edit_response("You have " + std::string(row[0]) + " XP, being level " + std::to_string(Util::getLevelFromXP(atoi(row[0]))));
				}
				else {
					event.edit_response("You have 0 XP");
				}
				mysql_free_result(res);
			}
		}

		if (event.command.get_command_name() == "leaderboard") {
			event.thinking();
			std::string query = "SELECT * FROM User ORDER BY UserXP DESC LIMIT 10";
			MYSQL_RES* res = SQL_conn->ExecuteReturnQuery(query.c_str());
			MYSQL_ROW row;
			if (res) {
				std::string response = "```";
				while ((row = mysql_fetch_row(res))) {
					response += std::string(row[1]) + " - " + std::string(row[2]) + " XP | Level " + std::to_string(Util::getLevelFromXP(atoi(row[2]))) + " \n";
				}
				response += "```";
				event.edit_response(response);
				mysql_free_result(res);
			}
		}

		if (event.command.get_command_name() == "give_xp") {
			event.thinking();
			auto executor = event.command.member;
			auto target = std::get<dpp::snowflake>(event.get_parameter("user"));
			auto amount = std::get<int64_t>(event.get_parameter("amount"));
			//get the target from the snowflake
			auto tuser = bot->user_get_cached_sync(target);
			if (tuser.id.empty()) {
				event.edit_response("User not found");
				return;
			}
			if (executor.user_id == tuser.id) {
				event.edit_response("You can't give yourself XP");
				return;
			}
			if (amount < 1) {
				event.edit_response("You can't give less than 1 XP");
				return;
			}

			//check for target in database

			std::string q0 = "SELECT * FROM User WHERE idUser = " + std::to_string(tuser.id);
			MYSQL_RES* res0;
			MYSQL_ROW row0;
			if (mysql_query(SQL_conn->getConnection(), q0.c_str())) {
				event.edit_response("Error: " + std::string(mysql_error(SQL_conn->getConnection())));
			}
			else {
				res0 = mysql_store_result(SQL_conn->getConnection());
				if (!res0) {
					event.edit_response("Target not found in database");
					return;
				}
				mysql_free_result(res0);
			}

			std::string q1 = "SELECT UserXP FROM User WHERE idUser = " + std::to_string(executor.user_id);
			MYSQL_RES* res;
			MYSQL_ROW row;
			if (mysql_query(SQL_conn->getConnection(), q1.c_str())) {
				event.edit_response("Error: " + std::string(mysql_error(SQL_conn->getConnection())));
			}
			else {
				res = mysql_store_result(SQL_conn->getConnection());
				if (res) {
					row = mysql_fetch_row(res);
					auto executorXP = atoi(row[0]);
					mysql_free_result(res);
					if (executorXP < amount) {
						event.edit_response("You don't have enough XP to give, dummy!");
						return;
					}
					std::string q2 = "UPDATE User SET UserXP = UserXP - " + std::to_string(amount) + " WHERE idUser = " + std::to_string(executor.user_id);
					std::string q3 = "UPDATE User SET UserXP = UserXP + " + std::to_string(amount) + " WHERE idUser = " + std::to_string(tuser.id);
					SQL_conn->ExecuteNoReturnQuery(q2.c_str(), true);
					SQL_conn->ExecuteNoReturnQuery(q3.c_str(), true);

					event.edit_response("Gave " + std::to_string(amount) + " XP to " + tuser.username);
				}
			}
		}
	}

}