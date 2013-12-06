{
	"targets": [
		{
			"target_name": "hiredissimple",
			"sources": [ 
				"src/hiredissimple.cc",
				"src/connection.cc"
			],
			'libraries' : ['-lhiredis']
		}
	]
}
