
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
console.log("del key-name list1");
redis.del("list1");

console.log("---");
console.log("making list: c b a 1 2 3");
redis.lpush("list1","a");
redis.lpush("list1","b");
redis.lpush("list1","c");
redis.rpush("list1","1");
redis.rpush("list1","2");
redis.rpush("list1","3");
var array = redis.lrange("list1",0,-1);
console.log("list1:"+array[0]);

console.log("pop, list1->b a 1 2");
redis.lpop("list1");
redis.rpop("list1");
var array = redis.lrange("list1",0,-1);
console.log("list1:"+array);

console.log("---");
console.log("exist list1?");
if( redis.exists("list1") ){
	console.log("list1 is exist");
}

console.log("---");
console.log("getting random key, should be key2, list1 or myset");
var rkey = redis.randomkey();
console.log(rkey);
console.log("value of LIST is string, call parseInt if your data should be number");

console.log("---");
redis.del("myset");
console.log("make set");
redis.sadd("myset","sval1");
redis.sadd("myset","sval2");
redis.sadd("myset",3);
redis.sadd("myset",4);
var myset_vals = redis.smembers("myset");
console.log("myset now has:");
console.log(myset_vals);
console.log("pickup random one, should be sva11, sval2, 3 or 4");
myset_vals = redis.srandmember("myset");
console.log(myset_vals);
console.log("value of SET is string, call parseInt if your data should be number");

console.log("---");
redis.del("counter");
console.log("count up/down, 1->2->0");
redis.set("counter",0);
var count;
count = redis.incr("counter");
console.log("counter(+1) : "+count);
count = redis.incr("counter");
console.log("counter(+1) : "+count);
count = redis.decrby("counter",2);
console.log("counter(-2) : "+count);

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

