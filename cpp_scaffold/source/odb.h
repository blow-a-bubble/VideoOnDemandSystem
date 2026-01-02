#pragma once
#include "logger.h"
#include <odb/database.hxx>
#include <odb/mysql/transaction.hxx>
#include <odb/mysql/database.hxx>

namespace bubble
{
    struct ODBSettings
    {
        std::string host;
        std::string user = "root";
        std::string passwd;
        std::string db;
        std::string cset = "utf8";
        unsigned int port = 3306;
        unsigned int connection_pool_size = 3;
    };

    class ODBFactory
    {
    public:
        static std::shared_ptr<odb::database> create(const ODBSettings &settings);
    };
}