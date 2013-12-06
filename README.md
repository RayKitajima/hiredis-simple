
# Hiredis-Simple

Synchronous, blocking redis binding for node.js.
This modules requires hiredis C library on your system.

No support for transaction, simply get, set and expire.

## Preparation

	git clone https://github.com/redis/hiredis
	cd hiredis
	make && make install

## Installation

    npm install hiredis-simple

## Examples

```javascript
var HiredisSimple = require('hiredis-simple');
var redis = HiredisSimple.Client();

redis.connect("127.0.0.1",6379);

redis.set("key1","value1");

redis.expire("key1",86400);

var value = redis.get("key1");

redis.disconnect();
```

## Note

Still only for node 0.8.*.

## License

Copyright (c) 2013 Rei Kitajima (rei.kitajima@gmail.com)

MIT License

