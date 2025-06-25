-- set minimum xmake version
set_xmakever("2.8.2")

-- includes
includes("lib/commonlibsse")

-- set project
set_project("styyx-thievery")
set_version("1.0.0")
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

-- targets
-- ImGui static lib target
target("imgui")
    set_kind("static")
    add_files("extern/ImGUI/*.cpp")
    add_includedirs("extern/ImGUI", {public = true})

target("styyx-thievery")
    -- add dependencies to target
    add_deps("commonlibsse")
    add_deps("imgui")
    set_policy("build.c++.modules", true)
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
    add_includedirs("extern/ImGUI")
    add_includedirs("extern/ImGUI/backends")
    add_files("extern/ImGUI/backends/imgui_impl_dx11.cpp")
    add_files("extern/ImGUI/backends/imgui_impl_win32.cpp")
    add_includedirs("extern/clibutil/include", { public = true })

after_build(function(target)
    local copy = function(env, ext)
        for _, env in pairs(env:split(";")) do
            if os.exists(env) then
                local plugins = path.join(env, ext, "SKSE/Plugins")
                os.mkdir(plugins)
                os.trycp(target:targetfile(), plugins)
                os.trycp(target:symbolfile(), plugins)
                -- Copy config files or other extras
                os.trycp("$(projectdir)/release/**", plugins)
            end
        end
    end
    if os.getenv("XSE_TES5_MODS_PATH") then
        copy(os.getenv("XSE_TES5_MODS_PATH"), target:name())
    elseif os.getenv("XSE_TES5_GAME_PATH") then
        copy(os.getenv("XSE_TES5_GAME_PATH"), "Data")
    end    
end)
