#ifndef CONNECTION_H
#define CONNECTION_H

#include <node.h>
#include <v8.h>
#include <node_buffer.h>
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
	
private:
	
	redisContext *connection_;
	
	Connection();
	~Connection();
	
	static Handle<Value> New(const v8::Arguments& args);
	static Handle<Value> Connect(const Arguments& args);
	static Handle<Value> Disconnect(const Arguments& args);
	
	static Handle<Value> Get(const Arguments& args);
	static Handle<Value> Set(const Arguments& args);
	static Handle<Value> Del(const Arguments& args);
	static Handle<Value> Expire(const Arguments& args);
	
	static char* MallocCString(Handle<Value> v8String);
	
};

#endif


