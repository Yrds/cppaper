#include "systems/lua.hpp"

#include <filesystem>
#include <map>

#include "components/FileComponent.hpp"
#include "components/PathComponent.hpp"
#include "components/Script.hpp"


namespace cppaper {

void scanScriptFiles(entt::registry &registry) {
  std::filesystem::path scriptPath{"scripts"};

  if(!std::filesystem::is_directory(scriptPath)) {
    return;
  }

  std::map<std::string, std::filesystem::path> sortedScriptFiles;

  for(auto& file: std::filesystem::directory_iterator(scriptPath)) {
    if(std::filesystem::is_regular_file(file) && file.path().extension() == "lua" ) {
      sortedScriptFiles[file.path().string()] = file;
    }
  }

  for(auto& file: sortedScriptFiles) {
    auto scriptId = registry.create();
    registry.emplace<OriginPathComponent>(scriptId, file.second);
    registry.emplace<ScriptComponent>(scriptId);
  }

}

void initScriptSystem(entt::registry &registry) {
  const auto scriptsView = registry.view<const OriginPathComponent, ScriptComponent>().use<ScriptComponent>();

  scriptsView.each([](const auto &originPathComponent, auto &scriptComponent){
    scriptComponent.lua.open_libraries(sol::lib::base);
    scriptComponent.lua.script_file(originPathComponent.path);
  });

}

void luaBeforeTemplate(entt::registry &registry) {
  const auto scriptsView = registry.view<ScriptComponent>();

  for(auto it = scriptsView.rbegin(), last = scriptsView.rend(); it != last; ++it) {
    scriptsView.get<ScriptComponent>(*it).lua.script("before_template()");
  }
}

void luaBeforeOutput(entt::registry &registry) {
  const auto scriptsView = registry.view<ScriptComponent>();

  for(auto it = scriptsView.rbegin(), last = scriptsView.rend(); it != last; ++it) {
    scriptsView.get<ScriptComponent>(*it).lua.script("before_output()");
  }
}
}
