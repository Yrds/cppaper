#include "script/functions/getConfigFromEntity.hpp"
#include "components/FileComponent.hpp"
#include "components/Config.hpp"
#include "systems/config.hpp"
#include "components/NoOutput.hpp"

namespace cppaper::script::functions {

  void getConfigFromEntity(entt::registry& registry, sol::state& lua)
  {
    lua.set_function("get_configs_from_entity",[&registry,&lua](sol::variadic_args va){
      if(va.size() > 1)
      {
        throw std::invalid_argument("The function get_configs_from_entity accepts only 1 argument");
      }

      auto configView = registry.view<const ConfigComponent>(entt::exclude<NoOutputComponent>);

      for(auto [entity, config] : configView.each()  )
      {
        if(static_cast<int>(entity) == static_cast<int>(va[0]) )
        {
          sol::table table = lua.create_table();
          for( auto c : config.map)
          {
            table[c.first] = c.second;
          }
          return sol::make_object(lua, table );
        }
      }
      return sol::make_object(lua,sol::lua_nil);
    } );
  }

}