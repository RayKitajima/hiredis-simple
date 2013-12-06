
// 
// make sure you are running redis with this configuration
// host:127.0.0.1,port:6379
// 

var redis = require('../build/Release/hiredissimple');
console.log("hiredis-simple:"+redis);

var con = new redis.Connection();
console.log("connection:"+con);

console.log("---");
console.log("making connection");
con.connect("127.0.0.1",6379);
console.log("connected");

console.log("---");
console.log("set:");
var res1 = con.set("key1","value1");
console.log(res1);
var res2 = con.set("key2","value2");
console.log(res2);

console.log("---");
console.log("get:");
var val1 = con.get("key1");
console.log(val1);
var val2 = con.get("key2");
console.log(val2);

console.log("expire key2:3sec");
con.expire("key2",3);

console.log("wait 4sec");

setTimeout(
	function(){
		console.log("now key2 should be undefined:");
		var val3 = con.get("key2");
		console.log(val3);
		
		console.log("---");
		con.disconnect();
		console.log("then disconnected");
	},
	4000
);


