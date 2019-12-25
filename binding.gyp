{
	"targets": [
		{
			"target_name": "bubble_scanner",
			"sources": [
				"src/bubble_scanner.cc",
				"src/scan.cc",
				"src/scan_worker.cc"
			],

			"libraries": [
				"<!(pkg-config --libs opencv)"
			],
			"include_dirs": [
				"<!(pkg-config --cflags opencv)",
				"<!(node -e \"require('nan')\")"
			],

			"cflags!" : [ "-fno-exceptions"],
			"cflags_cc!": [ "-fno-rtti",  "-fno-exceptions"],

			"conditions": [
				[ "OS==\"linux\" or OS==\"freebsd\" or OS==\"openbsd\" or OS==\"solaris\" or OS==\"aix\"", {
					"cflags": [
						"<!(pkg-config --cflags opencv)",
						"-Wall"
					]
				}],
				["OS==\"mac\"", {
					"xcode_settings": {
						"OTHER_CFLAGS": [
							"-mmacosx-version-min=10.7",
							"-std=c++11",
							"-stdlib=libc++",
							"<!(pkg-config --cflags opencv)",
						],
						"GCC_ENABLE_CPP_RTTI": "YES",
						"GCC_ENABLE_CPP_EXCEPTIONS": "YES"
					}
				}]
			]
		}
	]
}
