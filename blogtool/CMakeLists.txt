set(tgt_name blogtool)

file(GLOB_RECURSE headers CONFIGURE_DEPENDS include/*.h include/*.hpp)
file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.c src/*.cpp src/*.cc src/*.cxx)

add_executable(${tgt_name})

target_sources(${tgt_name} PUBLIC ${headers})
target_sources(${tgt_name} PRIVATE ${sources})

target_include_directories(${tgt_name} PUBLIC include)

target_link_libraries(${tgt_name} PRIVATE 
    CLI11::CLI11
    fmt::fmt
    spdlog::spdlog
)