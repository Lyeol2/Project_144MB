# source/core/CMakeLists.txt

file(GLOB_RECURSE CORE_SOURCES
    "private/*.cpp"
)

# Remove Launch.cpp from core library sources as it will be used in the executable
list(REMOVE_ITEM CORE_SOURCES "${CMAKE_CURRENT_LIST_DIR}/private/Launch.cpp")

add_library(core STATIC ${CORE_SOURCES})

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/public
)
