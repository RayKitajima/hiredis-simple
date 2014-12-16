#ifndef CONNECTION_H
#define CONNECTION_H

#include <nan.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "hiredis/hiredis.h"

#define LOG(msg) printf("%s\n",msg);
#define THROW(msg) return ThrowException(Exception::Error(String::New(msg)));

using namespace v8;
using namespace node;

class Connection : public node::ObjectWrap
{

public:
	
	static void Init(Handle<Object> target);
	bool Connect(const char* host, const int port);
	void Disconnect();
	
protected:
	
	redisContext *connection_;
	
	Connection();
	~Connection();
	
	static NAN_METHOD(New);
	static NAN_METHOD(Connect);
	static NAN_METHOD(Disconnect);
	
	static NAN_METHOD(Get);
	static NAN_METHOD(Set);
	static NAN_METHOD(Del);
	static NAN_METHOD(Lpush);
	static NAN_METHOD(Rpush);
	static NAN_METHOD(Lrange);
	static NAN_METHOD(Lpop);
	static NAN_METHOD(Rpop);
	static NAN_METHOD(Exists);
	static NAN_METHOD(Expire);
	static NAN_METHOD(Incr);
	static NAN_METHOD(Incrby);
	static NAN_METHOD(Decr);
	static NAN_METHOD(Decrby);
	
	static char* MallocCString(Handle<Value> v8String);
	
};

#endif


