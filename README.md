
# Hiredis-Simple

Synchronous, blocking redis binding for node.js.
This modules requires hiredis C library on your system.

## Preparation

	git clone https://github.com/redis/hiredis
	cd hiredis
	make && make install

## Installation

    npm install hiredis-simple

## Supported commnads

GET/SET/DEL/LPUSH/RPUSH/LRANGE/LPOP/RPOP/EXIST/INCR/INCRBY/DECR/DECRBY

SADD/SMEMBERS/SRANDMEMBER/RANDOMKEY

FLUSHDB/FLUSHALL


## Examples

```javascript
var HiredisSimple = require('hiredis-simple');
var redis = new HiredisSimple.Client();

redis.connect("127.0.0.1",6379);

redis.set("key1","value1");
var value1 = redis.get("key1");

redis.expire("key1",86400);

redis.lpush("list1","value1"); // can push one-by-one
redis.rpush("list1","value2");
var array = redis.lrange("list1",0,-1);

var value1 = redis.lpop("list1");
var value2 = redis.rpop("list1");

redis.disconnect();
```

## Note

Node 0.8, 0.10, 0.11+ supported.

## License

Copyright (c) 2013 Rei Kitajima (rei.kitajima@gmail.com)

MIT License

