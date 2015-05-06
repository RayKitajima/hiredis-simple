{
	"targets": [
		{
			"target_name": "hiredissimple",
			"sources": [ 
				"src/hiredissimple.cc",
				"src/connection.cc"
			],
			"include_dirs": [
				'<!(node -e "require(\'nan\')")'
			],
			"libraries" : ['-lhiredis']
		}
	]
}
