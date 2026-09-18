cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

project(MtrsUtils)
set(LIB_NAME "mtrsutils")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(TARGET mtrsutils)
    return()
endif()

add_library(${LIB_NAME} STATIC
    
src/fun/fs/get_folder.cpp
src/fun/fs/load_external_lib.cpp

src/fun/math/hash.cpp

src/fun/msg/mtrs_message.cpp

src/fun/str/string_optimizer.cpp
src/fun/str/utf8_to_utf32.cpp
src/fun/str/preprocess_json.cpp

src/type/prs/MtrsFileManager.cpp
src/type/prs/DeferredData.cpp

)

target_compile_definitions(${LIB_NAME} PRIVATE
    $<$<CONFIG:Release>:FLAG_RELEASE>
)

target_include_directories(${LIB_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

