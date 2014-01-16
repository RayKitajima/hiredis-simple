{
	"targets": [
		{
			"target_name": "hiredissimple",
			"sources": [ 
				"src/hiredissimple.cc",
				"src/connection.cc"
			],
			"include_dirs": [
				'<!@(pg_config --includedir)',
				'<!(node -e "require(\'nan\')")'
			],
			"libraries" : ['-lhiredis']
		}
	]
}
