project "mn"
	language "C++"
	kind "SharedLib"

	files
	{
		"include/**.h",
		"include/**.hpp",
		"src/**.cpp"
	}

	includedirs
	{
		"include/"
	}

	--language configuration
	warnings "Extra"
	cppdialect "c++17"
	systemversion "latest"

	--linux configuration
	filter "system:linux"
		defines { "OS_LINUX" }
		linkoptions {"-pthread", "-ldl"}

	filter { "system:linux", "configurations:debug" }
		linkoptions {"-rdynamic"}


	--windows configuration
	filter "system:windows"
		defines { "OS_WINDOWS" }

	filter { "system:windows", "configurations:debug" }
		links {"dbghelp"}

	filter { "system:windows", "action:vs*"}
		files {"tools/vs/mn.natvis"}

	--os agnostic configuration
	filter "configurations:debug"
		targetsuffix "d"
		defines
		{
			"DEBUG",
			"MN_DLL"
		}
		symbols "On"

	filter "configurations:release"
		defines
		{
			"NDEBUG",
			"MN_DLL"
		}
		optimize "On"