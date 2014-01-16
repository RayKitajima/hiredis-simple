#include <nan.h>
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
	NanScope();
	
	Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
	
	tpl->SetClassName(NanSymbol("Connection"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	
	tpl->PrototypeTemplate()->Set(
		NanSymbol("Connection"),
		FunctionTemplate::New(New)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanSymbol("connect"),
		FunctionTemplate::New(Connect)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanSymbol("get"),
		FunctionTemplate::New(Get)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanSymbol("set"),
		FunctionTemplate::New(Set)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanSymbol("del"),
		FunctionTemplate::New(Del)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanSymbol("expire"),
		FunctionTemplate::New(Expire)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanSymbol("disconnect"),
		FunctionTemplate::New(Disconnect)->GetFunction()
	);
	
	target->Set(NanSymbol("Connection"), tpl->GetFunction());
}

//v8 entry point to constructor
NAN_METHOD(Connection::New)
{
	NanScope();
	
	Connection *connection = new Connection();
	connection->Wrap(args.This());
	
	NanReturnValue(args.This());
}

//v8 entry point into Connection#connect
NAN_METHOD(Connection::Connect)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if(args.Length() == 0 || !args[0]->IsString() || !args[1]->IsNumber()) {
		NanThrowError("Invalid connection request, you should specify host and port");
	}
	
	String::Utf8Value host(args[0]->ToString());
	bool success = self->Connect(*host,args[1]->NumberValue());
	if(!success) {
		NanThrowError("Can not make connection");
		self->Disconnect();
	}
	
	NanReturnUndefined();
}

//v8 entry point into Connection#disconnect
NAN_METHOD(Connection::Disconnect)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	self->Disconnect();
	
	NanReturnUndefined();
}


// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 


//v8 entry point into Connection#get
NAN_METHOD(Connection::Get)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() ){
		NanThrowError("No key specified");
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
		NanReturnUndefined();
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
	
	NanReturnValue(response);
}

//v8 entry point into Connection#set
NAN_METHOD(Connection::Set)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() || !args[1]->IsString() ){
		NanThrowError("key and value should be string");
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
		NanReturnUndefined();
	}
	
	free(key);
	free(val);
	freeReplyObject(reply);
	
	NanReturnUndefined();
}

//v8 entry point into Connection#del
NAN_METHOD(Connection::Del)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() ){
		NanThrowError("No key specified");
	}
	
	char* key = MallocCString(args[0]);
	
	// call hiredis
	redisCommand(self->connection_,"DEL %s",key);
	
	free(key);
	
	NanReturnUndefined();
}

//v8 entry point into Connection#expire
NAN_METHOD(Connection::Expire)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() || !args[1]->IsNumber() ){
		NanThrowError("key should be string, or ttl should be number");
	}
	
	char* key = MallocCString(args[0]);
	int ttl = args[1]->NumberValue();
	
	// call hiredis
	redisCommand(self->connection_,"EXPIRE %s %d",key,ttl);
	
	free(key);
	
	NanReturnUndefined();
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




