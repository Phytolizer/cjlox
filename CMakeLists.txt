cmake_minimum_required(VERSION 3.12...3.23)

project(
    cjlox
    LANGUAGES C
    VERSION 0.1.0
    DESCRIPTION "JLox in C"
    HOMEPAGE_URL "https://github.com/Phytolizer/cjlox"
)

function(declare_module NAME)
    cmake_parse_arguments(
        PARSE_ARGV 0 "DM" "INTERNAL_INCLUDE" "KIND;TARGET_NAME;OUTPUT_NAME"
        "SOURCES;ABSOLUTE_SOURCES;DEPENDS;INCLUDES"
    )
    if(NOT DM_KIND)
        message(FATAL_ERROR "Missing module kind")
    endif()
    if(DM_KIND STREQUAL "library")
        set(DM_PUBLICITY PUBLIC)
    elseif(DM_KIND STREQUAL "executable")
        set(DM_PUBLICITY PRIVATE)
    elseif(DM_KIND STREQUAL "interface")
        set(DM_KIND "library")
        set(DM_INTERFACEKW INTERFACE)
        set(DM_PUBLICITY INTERFACE)
    else()
        message(FATAL_ERROR "Unknown module kind")
    endif()
    if(DM_TARGET_NAME)
        set(DM_TARGET_NAME ${DM_TARGET_NAME})
    else()
        set(DM_TARGET_NAME ${NAME})
    endif()
    list(TRANSFORM DM_SOURCES PREPEND "modules/${NAME}/source/")
    cmake_language(
        CALL "add_${DM_KIND}" ${DM_TARGET_NAME} ${DM_INTERFACEKW} ${DM_SOURCES}
        ${DM_ABSOLUTE_SOURCES}
    )
    target_link_libraries(${DM_TARGET_NAME} ${DM_PUBLICITY} ${DM_DEPENDS})
    target_include_directories(
        ${DM_TARGET_NAME} ${DM_PUBLICITY} ${DM_INCLUDES}
        "modules/${NAME}/include"
    )
    target_compile_options(
        ${DM_TARGET_NAME}
        ${DM_PUBLICITY}
        -Wall
        -Wextra
        -Werror=implicit-function-declaration
        -Werror=return-type
        -Werror=unused-function
        -Werror=unused-parameter
        -Werror=unused-variable
        -fsanitize=address,undefined
    )
    target_link_options(
        ${DM_TARGET_NAME} ${DM_PUBLICITY} -fsanitize=address,undefined
    )
    if(DM_INTERNAL_INCLUDE)
        target_include_directories(
            ${DM_TARGET_NAME} PRIVATE "modules/${NAME}/internal_include"
        )
    endif()
    if(DM_OUTPUT_NAME)
        set_target_properties(
            ${DM_TARGET_NAME} PROPERTIES OUTPUT_NAME ${DM_OUTPUT_NAME}
        )
    endif()
endfunction()

declare_module(sysexits KIND interface)
declare_module(phyto_strong_types KIND interface)
declare_module(
    phyto_vec
    KIND library
    SOURCES vec.c
)
declare_module(ansi_esc KIND interface)
declare_module(
    nonstd
    KIND library
    SOURCES asprintf.c ctype.c strtok.c strspn.c strpbrk.c strdup.c
)
declare_module(
    phyto_test
    KIND interface
    DEPENDS ansi_esc nonstd
)
declare_module(
    phyto_vec_test
    KIND executable
    SOURCES main.c
    DEPENDS phyto_vec phyto_test
)
declare_module(phyto_span KIND interface)
declare_module(
    phyto_string_view
    KIND library
    SOURCES string_view.c
    DEPENDS phyto_span
)
declare_module(
    phyto_string
    KIND library
    SOURCES string.c
    DEPENDS phyto_string_view phyto_vec nonstd
)
declare_module(
    phyto_string_test
    KIND executable
    SOURCES append_fill.c append_view.c main.c own.c
    DEPENDS phyto_string phyto_test
)

declare_module(
    lox
    KIND library
    SOURCES lox.c
    DEPENDS sysexits
)
declare_module(
    cjlox
    KIND executable
    SOURCES main.c
    DEPENDS lox sysexits
)

if(PROJECT_IS_TOP_LEVEL)
    enable_testing()
    add_test(NAME phyto_vec_test COMMAND phyto_vec_test)
    add_test(NAME phyto_string_test COMMAND phyto_string_test)
endif()
