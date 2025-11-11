#include "script/functions/getConfigFromEntity.hpp"
#include "components/Config.hpp"
#include "components/NoOutput.hpp"

namespace cppaper::script::functions {

  void getConfigFromEntity(entt::registry& registry, sol::state& lua)
  {
    lua.set_function("get_configs_from_entity", [&](sol::variadic_args va){
      if(va.size() > 1) {
        throw std::invalid_argument("The function get_configs_from_entity accepts only 1 argument");
      }

      auto config = registry.try_get<const ConfigComponent>(va[0]);
      if (!config) return sol::make_object(lua, sol::lua_nil);
      
      sol::table table = lua.create_table();

      for(auto c : config->map)
      {
        table[c.first] = c.second;
      }
      return sol::make_object(lua, table);

    } );
  }

}
