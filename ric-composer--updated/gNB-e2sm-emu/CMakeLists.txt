cmake_minimum_required(VERSION 2.8)
project(GNB-E2SM-EMU C)

include_directories(proto)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -g -O0")

set(PROTOBUFC_COMPILE_FLAGS "-I/usr/local/include -L/usr/local/lib -lprotobuf-c")
set(PROTOBUF_LINK_FLAG "-L/usr/local/lib -lprotobuf-c")

set(CMAKE_CXX_FLAGS "${PROTOBUFC_COMPILE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${PROTOBUF_LINK_FLAG}")


add_executable(gnb_e2server_emu
        gnb_e2server_emu.c
        oai-oran-protolib/builds/ran_messages.pb-c.h
        oai-oran-protolib/builds/ran_messages.pb-c.c
        gnb_message_handlers.c
        gnb_message_handlers.h)


target_link_libraries(gnb_e2server_emu protobuf-c)
#target_link_libraries(e2_term_emu protobuf-c)

macro(print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "print_all_variables------------------------------------------}")
endmacro()

#print_all_variables()