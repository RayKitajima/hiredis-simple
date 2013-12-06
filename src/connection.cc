
#include <node.h>
#include <node_buffer.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "connection.h"

using namespace v8;
using namespace node;

Connection::Connection() {};
Connection::~Connection() {};

//v8 object initializer
void Connection::Init(Handle<Object> target)
{
	Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
	
	tpl->SetClassName(String::NewSymbol("Connection"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	
	tpl->PrototypeTemplate()->Set(
		String::NewSymbol("Connection"),
		FunctionTemplate::New(New)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		String::NewSymbol("connect"),
		FunctionTemplate::New(Connect)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		String::NewSymbol("get"),
		FunctionTemplate::New(Get)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		String::NewSymbol("set"),
		FunctionTemplate::New(Set)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		String::NewSymbol("del"),
		FunctionTemplate::New(Del)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		String::NewSymbol("expire"),
		FunctionTemplate::New(Expire)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		String::NewSymbol("disconnect"),
		FunctionTemplate::New(Disconnect)->GetFunction()
	);
	
	Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
	target->Set(String::NewSymbol("Connection"), constructor);
}

//v8 entry point to constructor
Handle<Value> Connection::New(const Arguments& args)
{
	HandleScope scope;
	
	Connection *connection = new Connection();
	connection->Wrap(args.This());
	
	return args.This();
}

//v8 entry point into Connection#connect
Handle<Value> Connection::Connect(const Arguments& args)
{
	HandleScope scope;
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if(args.Length() == 0 || !args[0]->IsString() || !args[1]->IsNumber()) {
		THROW("Invalid connection request, you should specify host and port");
	}
	
	String::Utf8Value host(args[0]->ToString());
	bool success = self->Connect(*host,args[1]->NumberValue());
	if(!success) {
		THROW("Can not make connection");
		self->Disconnect();
	}
	
	return Undefined();
}

//v8 entry point into Connection#disconnect
Handle<Value> Connection::Disconnect(const Arguments& args)
{
	HandleScope scope;
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	self->Disconnect();
	
	return Undefined();
}


// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 


//v8 entry point into Connection#get
Handle<Value> Connection::Get(const Arguments& args)
{
	HandleScope scope;
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() ){
		THROW("No key specified");
	}
	
	char* key = MallocCString(args[0]);
	
	// call hiredis
	redisReply *reply;
	redisAppendCommand(self->connection_,"GET %s",key);
	int res = redisGetReply(self->connection_, (void**)&reply);
	
	if( res == REDIS_ERR ){
		LOG("GET command failed");
		free(key);
		freeReplyObject(reply);
		return scope.Close(Undefined());
	}
	
	// make response
	Local<Value> response;
	
	// currently supports integer and string
	if( reply->type == REDIS_REPLY_STRING ){
		response = String::New(reply->str);
	}else if( reply->type == REDIS_REPLY_INTEGER ){
		response = Integer::New(reply->integer);
	}
	
	free(key);
	freeReplyObject(reply);
	
	return scope.Close(response);
}

//v8 entry point into Connection#set
Handle<Value> Connection::Set(const Arguments& args)
{
	HandleScope scope;
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() || !args[1]->IsString() ){
		THROW("key and value should be string");
	}
	
	char* key = MallocCString(args[0]);
	char* val = MallocCString(args[1]);
	
	// call hiredis
	redisReply *reply;
	redisAppendCommand(self->connection_,"SET %s %s",key,val);
	int res = redisGetReply(self->connection_, (void**)&reply);
	
	if( res == REDIS_ERR ){
		LOG("SET command failed");
		free(key);
		freeReplyObject(reply);
		return scope.Close(Undefined());
	}
	
	free(key);
	free(val);
	freeReplyObject(reply);
	
	return scope.Close(Undefined());
}

//v8 entry point into Connection#del
Handle<Value> Connection::Del(const Arguments& args)
{
	HandleScope scope;
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() ){
		THROW("No key specified");
	}
	
	char* key = MallocCString(args[0]);
	
	// call hiredis
	redisCommand(self->connection_,"DEL %s",key);
	
	free(key);
	
	return scope.Close(Undefined());
}

//v8 entry point into Connection#expire
Handle<Value> Connection::Expire(const Arguments& args)
{
	HandleScope scope;
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() || !args[1]->IsNumber() ){
		THROW("key should be string, or ttl should be number");
	}
	
	char* key = MallocCString(args[0]);
	int ttl = args[1]->NumberValue();
	
	// call hiredis
	redisCommand(self->connection_,"EXPIRE %s %d",key,ttl);
	
	free(key);
	
	return scope.Close(Undefined());
}


// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 


//initialize redis context
bool Connection::Connect(const char* host, const int port)
{
	connection_ = redisConnect(host, port);
	
	if( connection_->err ){
		LOG("Unable to set connection");
		redisFree(connection_);
		return false;
	}
	
	return true;
}

//safely destroys the redis context
void Connection::Disconnect()
{
	if(connection_ != NULL) {
		redisFree(connection_);
		connection_ = NULL;
	}
}


// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 


//helper function to malloc new string from v8string
char* Connection::MallocCString(v8::Handle<Value> v8String)
{
	String::Utf8Value utf8String(v8String->ToString());
	char *cString = (char *) malloc(strlen(*utf8String) + 1);
	if(!cString) {
		return cString;
	}
	strcpy(cString, *utf8String);
	return cString;
}




