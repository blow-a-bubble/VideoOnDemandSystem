#include "odb.h"

namespace bubble
{
    std::shared_ptr<odb::database> ODBFactory::create(const ODBSettings &settings)
    {
        std::unique_ptr<odb::mysql::connection_pool_factory> pool_factory = std::make_unique<odb::mysql::connection_pool_factory>(settings.connection_pool_size);
        return std::make_shared<odb::mysql::database>(
            settings.user.c_str(),
            settings.passwd.c_str(),
            settings.db.c_str(),
            settings.host.c_str(),
            settings.port,
            nullptr,
            settings.cset.c_str(),
            0,
            std::move(pool_factory)
        );
    }
}