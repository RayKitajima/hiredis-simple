
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

Client.get = function(key){
	return this.connection.get(key);
};

Client.set = function(key,value){
	return this.connection.set(key,value);
};

Client.del = function(key){
	return this.connection.del(key);
};

Client.expire = function(key,ttl){
	return this.connection.expire(key,ttl);
};

Client.disconnect = function(){
	this.connection.disconnect();
};



