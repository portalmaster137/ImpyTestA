#pragma once
#include <mysql/mysql.h>
#include <spdlog/spdlog.h>

class SQLConnector {
private:
    MYSQL* conn;

public:
    inline SQLConnector(const char* server, const char* user, const char* password, const char* database, unsigned int port) {
        conn = mysql_init(NULL);

        if (!mysql_real_connect(conn, server, user, password, database, port, NULL, 0)) {
            spdlog::critical("Error connecting to database: {}", mysql_error(conn));
            throw std::runtime_error("Error connecting to database");
        }
        spdlog::debug("Successfully connected to database");
    }

    inline void closeConnection() {
		mysql_close(conn);
	}

    inline bool connected() {
        return mysql_ping(conn) == 0;
	}
	

    inline void ExecuteNoReturnQuery(const char* query, bool log = false) {
        if (mysql_query(conn, query)) {
			spdlog::critical("Error executing query: {}", mysql_error(conn));
			throw std::runtime_error("Error executing query");
		}
        if (log)
            spdlog::debug("Executed transaction: {}", query);
        MYSQL_RES* res;
        res = mysql_store_result(conn);
        mysql_free_result(res);
	}

    inline MYSQL_RES* ExecuteReturnQuery(const char* query) {
        if (mysql_query(conn, query)) {
			spdlog::critical("Error executing query: {}", mysql_error(conn));
			throw std::runtime_error("Error executing query");
		}
		MYSQL_RES* res;
		res = mysql_store_result(conn);
		return res;
	}

    inline ~SQLConnector() {
        mysql_close(conn);
    }

    inline MYSQL* getConnection() {
        return conn;
    }
};