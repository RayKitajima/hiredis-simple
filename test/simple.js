
// 
// make sure you are running redis with this configuration
// host:127.0.0.1,port:6379
// 

var HiredisSimple = require('hiredis-simple');
console.log("hiredis-simple:"+HiredisSimple);

var redis = new HiredisSimple.Client();
console.log("connection:"+redis);

console.log("---");
console.log("making connection");
redis.connect("127.0.0.1",6379);

console.log("---");
console.log("set:key1,key2");
var res1 = redis.set("key1","value1");
console.log(res1);
var res2 = redis.set("key2","value2");
console.log(res2);

console.log("---");
console.log("get with un-exist key -> undefined");
var val0 = redis.get("key0");
console.log(val0);

console.log("---");
console.log("get:key1,key2 -> value1,value2");
var val1 = redis.get("key1");
console.log(val1);
var val2 = redis.get("key2");
console.log(val2);

console.log("---");
console.log("del:key1, value1->undefined");
redis.get("key1");
val1 = redis.get("key1");
console.log(val1);
console.log("then del key1");
redis.del("key1");
val1 = redis.get("key1");
console.log(val1);

console.log("---");
console.log("key2 has:");
console.log(val2);
console.log("expire key2 after 3sec");
redis.expire("key2",3);
console.log("wait 4sec");
setTimeout(
	function(){
		console.log("now key2 should be undefined:");
		var val3 = redis.get("key2");
		console.log(val3);
		
		console.log("---");
		redis.disconnect();
		console.log("disconnected");
	},
	4000
);

