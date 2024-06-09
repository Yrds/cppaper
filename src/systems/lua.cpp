#include "systems/lua.hpp"

#include <filesystem>
#include <map>

#include "components/PathComponent.hpp"
#include "components/SystemConfigComponent.hpp"
#include "components/Script.hpp"

//Script functions
#include "script/functions/getPagesWithConfig.hpp"
#include "script/functions/getConfigFromPage.hpp"

namespace cppaper {

inline static std::map<std::string, sol::lib> librariesMap {
    {"base", sol::lib::base},
    {"package", sol::lib::package},
    {"coroutine", sol::lib::coroutine},
    {"string", sol::lib::string},
    {"os", sol::lib::os},
    {"math", sol::lib::math},
    {"table", sol::lib::table},
    {"debug", sol::lib::debug},
    {"bit32", sol::lib::bit32},
    {"io", sol::lib::io},
    {"ffi", sol::lib::ffi},
    {"jit", sol::lib::jit}
};

void scanScriptFiles(entt::registry &registry) {
  std::filesystem::path scriptPath{"scripts"};

  if(!std::filesystem::is_directory(scriptPath)) {
    return;
  }

  std::map<std::string, std::filesystem::path> sortedScriptFiles;

  for(auto& file: std::filesystem::directory_iterator(scriptPath)) {
    std::cout << "Script found" << std::endl;
    if(std::filesystem::is_regular_file(file) && file.path().extension() == ".lua" ) {
      sortedScriptFiles[file.path().string()] = file;
    }
  }

  for(auto& file: sortedScriptFiles) {
    auto scriptId = registry.create();
    registry.emplace<OriginPathComponent>(scriptId, file.second);
    registry.emplace<ScriptComponent>(scriptId);
  }
}

void loadScriptsLibraries(SystemConfigComponent& systemConfigComponent, sol::state& lua) {
    for(const auto &library : systemConfigComponent.luaLibraries) {
      std::cout << "loading library lua: " << library << std::endl;
      lua.open_libraries(librariesMap[library]);
    }
}

void loadScriptCppaperFunctions(entt::registry& registry, sol::state& lua) {
      std::cout << "loading cppaper function into lua..." << std::endl;

      script::functions::getPagesWithConfig(registry, lua);
      script::functions::getConfigFromPage(registry, lua);
}

void initScriptSystem(entt::registry &registry) {
  const auto scriptsView = registry.view<const OriginPathComponent, ScriptComponent>().use<ScriptComponent>();

  auto systemEntity = registry.view<SystemConfigComponent>().front();

  SystemConfigComponent& systemConfig =
      registry.get<SystemConfigComponent>(systemEntity);

  loadScriptCppaperFunctions(registry, systemConfig.lua);
  loadScriptsLibraries(systemConfig, systemConfig.lua);

  scriptsView.each([&](const auto &originPathComponent, auto &scriptComponent){
    loadScriptsLibraries(systemConfig, scriptComponent.lua);
    loadScriptCppaperFunctions(registry, scriptComponent.lua);

    scriptComponent.lua.script_file(originPathComponent.path);
  });

}

void luaBeforeTemplate(entt::registry &registry) {
  const auto scriptsView = registry.view<ScriptComponent>();

  for(auto it = scriptsView.rbegin(), last = scriptsView.rend(); it != last; ++it) {
    scriptsView.get<ScriptComponent>(*it).lua.script("if before_template then before_template() end");
  }
}

void luaBeforeOutput(entt::registry &registry) {
  const auto scriptsView = registry.view<ScriptComponent>();

  for(auto it = scriptsView.rbegin(), last = scriptsView.rend(); it != last; ++it) {
    scriptsView.get<ScriptComponent>(*it).lua.script("if before_output then before_output() end");
  }
}
}
