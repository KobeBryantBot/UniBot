#include "NativePluginEngine.hpp"
#include "CommandManager.hpp"
#include "EventBusImpl.hpp"
#include "KobeBryant.hpp"
#include "PluginManager.hpp"
#include "ScheduleManager.hpp"
#include "ServiceManager.hpp"
#include "api/utils/StringUtils.hpp"
#include "core/Global.hpp"

namespace fs = std::filesystem;

std::string NativePluginEngine::getPluginType() const { return "native"; }

#ifdef _WIN32

std::string getErrorReason(unsigned long errorCode) {
    if (errorCode == 126) {
        return tr("bot.errorCode.126");
    } else if (errorCode == 127) {
        return tr("bot.errorCode.127");
    }
    return {};
}

bool NativePluginEngine::loadPlugin(const std::string& name, const std::filesystem::path& entry) {
    try {
        auto& logger = KobeBryant::getInstance().getLogger();
        logger.info("bot.nativePlugin.loading", {name});
        if (HMODULE hModule = LoadLibrary(entry.wstring().c_str())) {
            mPluginsMap1[name]    = hModule;
            mPluginsMap2[hModule] = name;
            logger.info("bot.nativePlugin.loaded", {name});
            return true;
        } else {
            DWORD errorCode = GetLastError();
            auto  reason    = getErrorReason(errorCode);
            logger.error("bot.nativePlugin.load.fail", {name, S(errorCode), reason});
        }
    }
    CATCH
    return false;
}

bool NativePluginEngine::unloadPlugin(const std::string& name) {
    try {
        if (mPluginsMap1.contains(name)) {
            auto hModule = mPluginsMap1[name];
            return unloadPlugin(hModule);
        }
    }
    CATCH
    return false;
}

bool NativePluginEngine::unloadPlugin(HMODULE hModule) {
    try {
        if (mPluginsMap2.contains(hModule)) {
            auto name = mPluginsMap2[hModule];
            PluginManager::getInstance().tryRemovePluginEngine(name);
            KobeBryant::getInstance().getLogger().info("bot.nativePlugin.unloading", {name});
            if (FreeLibrary(hModule)) {
                mPluginsMap1.erase(name);
                mPluginsMap2.erase(hModule);
                KobeBryant::getInstance().getLogger().info("bot.nativePlugin.unloaded", {name});
                return true;
            }
        }
    }
    CATCH
    return false;
}

#else

#endif