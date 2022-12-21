#ifndef INDEX_FILE_COMPONENT_INCLUDED
#define INDEX_FILE_COMPONENT_INCLUDED

#include <filesystem>

#include "entt/entt.hpp"

namespace  {
  struct IndexFileComponent { 
    std::filesystem::path indexFilePath;
  };
}

#endif //IndexFile_INCLUDED
