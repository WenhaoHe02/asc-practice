add_rules("mode.debug", "mode.release")

toolchain("aocc")
set_kind("standalone")
set_sdkdir(" /opt/AMD/aocc-compiler-4.2.0")
set_toolset("cc", "clang")
set_toolset("cxx", "clang", "clang++")
set_toolset("ld", "clang++", "clang")
set_toolset("sh", "clang++", "clang")
set_toolset("ar", "ar")
set_toolset("ex", "ar")
set_toolset("strip", "strip")
set_toolset("mm", "clang")
set_toolset("mxx", "clang", "clang++")
set_toolset("as", "clang")
on_check(function(toolchain)
    return import("lib.detect.find_tool")("clang")
end)
on_load(function(toolchain)
    -- get march
    local march = is_arch("x86_64", "x64") and "-march=znver4" or "-m32"

    -- init flags for c/c++
    toolchain:add("cxflags", march)
    toolchain:add("ldflags", march)
    toolchain:add("shflags", march)
    local includedir = toolchain
    if os.isdir(includedir) then
        toolchain:add("includedirs", includedir)
    end
    -- init flags for objc/c++  (with ldflags and shflags)
    toolchain:add("mxflags", march)

    -- init flags for asm
    toolchain:add("asflags", march)
end)
toolchain_end()


add_requires("openmp", { system = true })

target("dgemm")
set_kind("binary")
set_toolchains("aocc")
add_files("src/*.c")
add_linkdirs("./../amd-blis/lib/ILP64")
add_rpathdirs("./../amd-blis/lib/ILP64")
add_links("m", "blis", "pthread")
add_includedirs("./../amd-blis/include/ILP64")
add_packages("openmp")

--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--
