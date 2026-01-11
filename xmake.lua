-- set minimum xmake version
set_xmakever("2.8.2")

-- includes
includes("lib/commonlibsse")
includes("extern/styyx-utils")

-- set project
set_project("styyx-thievery")
set_version("2.0.0")
set_license("GPL-3.0")

-- set defaults
set_languages("c++23")
set_warnings("allextra")

-- add rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

-- set policies
set_policy("build.optimization.lto", true)
set_policy("package.requires_lock", true)


-- set configs
set_config("skyrim_ae", true)
set_config("commonlib_toml", true)
set_config("commonlib_json", true)

add_extrafiles("release/**.toml")
add_extrafiles("release/**.json")

target("styyx-thievery")
    -- add dependencies to target
    add_deps("commonlibsse")
    set_policy("build.c++.modules", true)
    add_deps("styyx-util")
    -- add commonlibsse plugin
    add_rules("commonlibsse.plugin", {
        name = "styyx-thievery",
        author = "styyx",
        description = "overhaul for skyrim's thief skills"
    })

    -- add src files
    add_files("src/**.cpp", "src/*.ixx")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/pch.h")
    add_includedirs("extern/clibutil/include", { public = true })
