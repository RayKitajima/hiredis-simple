
// usage:
//     
//     var hiredis = require('hiredis-simple');
//     var client = new hiredis.Client();
//     client.connect("127.0.0.1",6379);
//     
//     client.set("key1","value1");
//     
//     var value = client.get("key1");
//     
//     client.expire("key1",86400);
//     
//     client.lpush("list1","value1");
//     client.rpush("list1","value2");
//     client.lrange("list1",0,-1);
//     
//     var value1 = client.lpop("key1");
//     var value2 = client.rpop("key2");
//     
//     if( redis.exist("key1") ){ "true"; }
//     
//     client.disconnect();
//     
//     

var binding = require(__dirname + '/../build/Release/hiredissimple.node');


// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 

// constructor

function ClientConstructor(){
	// instance variable
	this.connection = new binding.Connection();
};

module.exports = {
	Client : ClientConstructor
};

var Client = ClientConstructor.prototype;


// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 

// public APIs

Client.connect = function(host,port){
	this.connection.connect(host,port);
};

Client.select = function(index){
	return this.connection.select(index);
};

Client.get = function(key){
	return this.connection.get(key);
};

Client.set = function(key,value){
	return this.connection.set(key,value);
};

Client.del = function(key){
	return this.connection.del(key);
};

Client.lpush = function(key,val){
	return this.connection.lpush(key,val);
}

Client.rpush = function(key,val){
	return this.connection.rpush(key,val);
}

Client.lrange= function(key,from,to){
	return this.connection.lrange(key,from,to);
}

Client.lpop = function(key){
	return this.connection.lpop(key);
}

Client.rpop = function(key){
	return this.connection.rpop(key);
}

Client.expire = function(key,ttl){
	return this.connection.expire(key,ttl);
};

Client.exists = function(key){
	return this.connection.exists(key);
}

Client.incr = function(key){
	return this.connection.incr(key);
}

Client.incrby = function(key,num){
	return this.connection.incr(key,num);
}

Client.decr = function(key){
	return this.connection.decr(key);
}

Client.decrby = function(key,num){
	return this.connection.decrby(key,num);
}

Client.sadd = function(name,val){
	return this.connection.sadd(name,val);
}

Client.smembers = function(name){
	return this.connection.smembers(name);
}

Client.randomkey = function(){
	return this.connection.randomkey();
}

Client.srandmember = function(set,num){
	return this.connection.srandmember(set,num);
}

Client.flashall = function(){
	return this.connection.flashall();
}

Client.flashdb = function(){
	return this.connection.flashdb();
}

Client.disconnect = function(){
	this.connection.disconnect();
};



