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
	
	Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);
	
	tpl->SetClassName(NanNew<String>("Connection"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("Connection"),
		NanNew<FunctionTemplate>(New)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("connect"),
		NanNew<FunctionTemplate>(Connect)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("select"),
		NanNew<FunctionTemplate>(Select)->GetFunction()
	);

	tpl->PrototypeTemplate()->Set(
		NanNew<String>("get"),
		NanNew<FunctionTemplate>(Get)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("set"),
		NanNew<FunctionTemplate>(Set)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("del"),
		NanNew<FunctionTemplate>(Del)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("lpush"),
		NanNew<FunctionTemplate>(Lpush)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("rpush"),
		NanNew<FunctionTemplate>(Rpush)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("lrange"),
		NanNew<FunctionTemplate>(Lrange)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("lpop"),
		NanNew<FunctionTemplate>(Lpop)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("rpop"),
		NanNew<FunctionTemplate>(Rpop)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("expire"),
		NanNew<FunctionTemplate>(Expire)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("exists"),
		NanNew<FunctionTemplate>(Exists)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("incr"),
		NanNew<FunctionTemplate>(Incr)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("incrby"),
		NanNew<FunctionTemplate>(Incrby)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("decr"),
		NanNew<FunctionTemplate>(Decr)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("decrby"),
		NanNew<FunctionTemplate>(Decrby)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("sadd"),
		NanNew<FunctionTemplate>(Sadd)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("smembers"),
		NanNew<FunctionTemplate>(Smembers)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("randomkey"),
		NanNew<FunctionTemplate>(Randomkey)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("srandmember"),
		NanNew<FunctionTemplate>(Srandmember)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("flushall"),
		NanNew<FunctionTemplate>(Flushall)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("flushdb"),
		NanNew<FunctionTemplate>(Flushdb)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("disconnect"),
		NanNew<FunctionTemplate>(Disconnect)->GetFunction()
	);
	
	target->Set(NanNew<String>("Connection"), tpl->GetFunction());
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

//v8 entry point into Connection#select
NAN_METHOD(Connection::Select)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());

	if(args.Length() == 0 || !args[0]->IsNumber()) {
		NanThrowError("Invalid select request, you should specify index");
	}

	char* index = MallocCString(args[0]);

	// call hiredis
	redisReply *reply;
	redisAppendCommand(self->connection_,"SELECT %s",index);
	int res = redisGetReply(self->connection_, (void**)&reply);

	if(res == REDIS_ERR) {
		LOG("Can not select database");
		free(index);
		freeReplyObject(reply);
		NanReturnUndefined();
	}

	free(index);
	freeReplyObject(reply);
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
	if( reply->type == REDIS_REPLY_INTEGER ){
		response = NanNew<Number>(reply->integer);
	}else if( reply->type == REDIS_REPLY_STRING ){
		response = NanNew<String>(reply->str);
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
	
	if( !args[0]->IsString() ){
		NanThrowError("key should be string");
	}
	if( args[1]->IsUndefined() ){
		NanThrowError("value is not defined");
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

// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 

//v8 entry point into Connection#lpush
NAN_METHOD(Connection::Lpush)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() || !args[1]->IsString() ){
		NanThrowError("key should be string");
	}
	if( args[1]->IsUndefined() ){
		NanThrowError("value is not defined");
	}
	
	char* key = MallocCString(args[0]);
	char* val = MallocCString(args[1]);
	
	// call hiredis
	redisReply *reply;
	redisAppendCommand(self->connection_,"LPUSH %s %s",key,val);
	int res = redisGetReply(self->connection_, (void**)&reply);
	
	if( res == REDIS_ERR ){
		LOG("LPUSH command failed");
		free(key);
		freeReplyObject(reply);
		NanReturnUndefined();
	}
	
	free(key);
	free(val);
	freeReplyObject(reply);
	
	NanReturnUndefined();
}

//v8 entry point into Connection#rpush
NAN_METHOD(Connection::Rpush)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() || !args[1]->IsString() ){
		NanThrowError("key should be string");
	}
	if( args[1]->IsUndefined() ){
		NanThrowError("value is not defined");
	}
	
	char* key = MallocCString(args[0]);
	char* val = MallocCString(args[1]);
	
	// call hiredis
	redisReply *reply;
	redisAppendCommand(self->connection_,"RPUSH %s %s",key,val);
	int res = redisGetReply(self->connection_, (void**)&reply);
	
	if( res == REDIS_ERR ){
		LOG("RPUSH command failed");
		free(key);
		freeReplyObject(reply);
		NanReturnUndefined();
	}
	
	free(key);
	free(val);
	freeReplyObject(reply);
	
	NanReturnUndefined();
}

//v8 entry point into Connection#lrange
NAN_METHOD(Connection::Lrange)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() || !args[1]->IsNumber() || !args[2]->IsNumber() ){
		NanThrowError("key and range (from to) should be defiend");
	}
	
	char* key = MallocCString(args[0]);
	int from  = args[1]->IntegerValue();
	int to    = args[2]->IntegerValue();
	
	// call hiredis
	redisReply *reply;
	redisAppendCommand(self->connection_,"LRANGE %s %d %d",key,from,to);
	int res = redisGetReply(self->connection_, (void**)&reply);
	
	if( res == REDIS_ERR ){
		LOG("LRANGE command failed");
		free(key);
		freeReplyObject(reply);
		NanReturnUndefined();
	}
	
	// make response
	Local<Array> response = NanNew<v8::Array>();
	unsigned int i = 0;
	if( reply->type == REDIS_REPLY_ARRAY ){
		unsigned int j;
		for( j=0; j<reply->elements; j++ ){
			response->Set(i++,NanNew<String>(reply->element[j]->str));
		}
	}
	
	free(key);
	freeReplyObject(reply);
	
	NanReturnValue(response);
}

//v8 entry point into Connection#lpop
NAN_METHOD(Connection::Lpop)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() ){
		NanThrowError("No key specified");
	}
	
	char* key = MallocCString(args[0]);
	
	// call hiredis
	redisReply *reply;
	redisAppendCommand(self->connection_,"LPOP %s",key);
	int res = redisGetReply(self->connection_, (void**)&reply);
	
	if( res == REDIS_ERR ){
		LOG("LPOP command failed");
		free(key);
		freeReplyObject(reply);
		NanReturnUndefined();
	}
	
	// make response
	Local<Value> response;
	
	if( reply->type == REDIS_REPLY_STRING ){
		response = NanNew<String>(reply->str);
	}else if( reply->type == REDIS_REPLY_INTEGER ){
		response = NanNew<Number>(reply->integer);
	}
	
	free(key);
	freeReplyObject(reply);
	
	NanReturnValue(response);
}

//v8 entry point into Connection#rpop
NAN_METHOD(Connection::Rpop)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() ){
		NanThrowError("No key specified");
	}
	
	char* key = MallocCString(args[0]);
	
	// call hiredis
	redisReply *reply;
	redisAppendCommand(self->connection_,"RPOP %s",key);
	int res = redisGetReply(self->connection_, (void**)&reply);
	
	if( res == REDIS_ERR ){
		LOG("RPOP command failed");
		free(key);
		freeReplyObject(reply);
		NanReturnUndefined();
	}
	
	// make response
	Local<Value> response;
	if( reply->type == REDIS_REPLY_STRING ){
		response = NanNew<String>(reply->str);
	}else if( reply->type == REDIS_REPLY_INTEGER ){
		response = NanNew<Number>(reply->integer);
	}
	
	free(key);
	freeReplyObject(reply);
	
	NanReturnValue(response);
}

// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 

//v8 entry point into Connection#exists
NAN_METHOD(Connection::Exists)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() ){
		NanThrowError("No key specified");
	}
	
	char* key = MallocCString(args[0]);
	
	// call hiredis
	redisReply *reply;
	redisAppendCommand(self->connection_,"EXISTS %s",key);
	int res = redisGetReply(self->connection_, (void**)&reply);
	
	if( res == REDIS_ERR ){
		LOG("EXISTS command failed");
		free(key);
		freeReplyObject(reply);
		NanReturnUndefined();
	}
	
	// make response
	Local<Value> response = NanNew<Number>(reply->integer); // 0|1
	
	free(key);
	freeReplyObject(reply);
	
	NanReturnValue(response);
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

//v8 entry point into Connection#incr
NAN_METHOD(Connection::Incr)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() ){
		NanThrowError("No key specified");
	}
	
	char* key = MallocCString(args[0]);
	
	// call hiredis
	redisReply *reply;
	redisAppendCommand(self->connection_,"INCR %s",key);
	int res = redisGetReply(self->connection_, (void**)&reply);
	
	if( res == REDIS_ERR ){
		LOG("INCR command failed");
		free(key);
		freeReplyObject(reply);
		NanReturnUndefined();
	}
	
	// make response
	Local<Value> response = NanNew<Number>(reply->integer);
	
	free(key);
	freeReplyObject(reply);
	
	NanReturnValue(response);
}

//v8 entry point into Connection#incrby
NAN_METHOD(Connection::Incrby)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() || !args[1]->IsNumber() ){
		NanThrowError("key and num should be specified");
	}
	
	char* key = MallocCString(args[0]);
	int num = args[1]->IntegerValue();
	
	// call hiredis
	redisReply *reply;
	redisAppendCommand(self->connection_,"INCRBY %s %d",key,num);
	int res = redisGetReply(self->connection_, (void**)&reply);
	
	if( res == REDIS_ERR ){
		LOG("INCRBY command failed");
		free(key);
		freeReplyObject(reply);
		NanReturnUndefined();
	}
	
	// make response
	Local<Value> response = NanNew<Number>(reply->integer);
	
	free(key);
	freeReplyObject(reply);
	
	NanReturnValue(response);
}

//v8 entry point into Connection#decr
NAN_METHOD(Connection::Decr)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() ){
		NanThrowError("No key specified");
	}
	
	char* key = MallocCString(args[0]);
	
	// call hiredis
	redisReply *reply;
	redisAppendCommand(self->connection_,"DECR %s",key);
	int res = redisGetReply(self->connection_, (void**)&reply);
	
	if( res == REDIS_ERR ){
		LOG("DECR command failed");
		free(key);
		freeReplyObject(reply);
		NanReturnUndefined();
	}
	
	// make response
	Local<Value> response = NanNew<Number>(reply->integer);
	
	free(key);
	freeReplyObject(reply);
	
	NanReturnValue(response);
}

//v8 entry point into Connection#decrby
NAN_METHOD(Connection::Decrby)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() || !args[1]->IsNumber() ){
		NanThrowError("key and num should be specified");
	}
	
	char* key = MallocCString(args[0]);
	int num = args[1]->IntegerValue();
	
	// call hiredis
	redisReply *reply;
	redisAppendCommand(self->connection_,"DECRBY %s %d",key,num);
	int res = redisGetReply(self->connection_, (void**)&reply);
	
	if( res == REDIS_ERR ){
		LOG("DECRBY command failed");
		free(key);
		freeReplyObject(reply);
		NanReturnUndefined();
	}
	
	// make response
	Local<Value> response = NanNew<Number>(reply->integer);
	
	free(key);
	freeReplyObject(reply);
	
	NanReturnValue(response);
}

// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 

//v8 entry point into Connection#sadd
NAN_METHOD(Connection::Sadd)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() ){
		NanThrowError("name should be string");
	}
	if( args[1]->IsUndefined() ){
		NanThrowError("no valued defined");
	}
	
	char* key = MallocCString(args[0]);
	char* val = MallocCString(args[1]);
	
	// call hiredis
	redisReply *reply;
	redisAppendCommand(self->connection_,"SADD %s %s",key,val);
	int res = redisGetReply(self->connection_, (void**)&reply);
	
	if( res == REDIS_ERR ){
		LOG("SADD command failed");
		free(key);
		free(val);
		freeReplyObject(reply);
		NanReturnUndefined();
	}
	
	// make response
	Local<Value> response = NanNew<Number>(reply->integer);

	free(key);
	free(val);
	freeReplyObject(reply);
	
	NanReturnValue(response);
}

//v8 entry point into Connection#smembers
NAN_METHOD(Connection::Smembers)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() ){
		NanThrowError("name of set should be string");
	}
	
	char* key = MallocCString(args[0]);
	
	// call hiredis
	redisReply *reply;
	redisAppendCommand(self->connection_,"SMEMBERS %s",key);
	int res = redisGetReply(self->connection_, (void**)&reply);
	
	if( res == REDIS_ERR ){
		LOG("SMEMBERS command failed");
		free(key);
		freeReplyObject(reply);
		NanReturnUndefined();
	}
	
	// make response
	Local<Array> response = NanNew<v8::Array>();
	unsigned int i = 0;
	if( reply->type == REDIS_REPLY_ARRAY ){
		unsigned int j;
		for( j=0; j<reply->elements; j++ ){
			if( reply->element[j]->type == REDIS_REPLY_STRING ){
				response->Set(i++,NanNew<String>(reply->element[j]->str));
			}
		}
	}
	
	free(key);
	freeReplyObject(reply);
	
	NanReturnValue(response);
}

//v8 entry point into Connection#randomkey
NAN_METHOD(Connection::Randomkey)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	// call hiredis
	redisReply *reply;
	redisAppendCommand(self->connection_,"RANDOMKEY");
	int res = redisGetReply(self->connection_, (void**)&reply);
	
	if( res == REDIS_ERR ){
		LOG("RANDOMKEY command failed");
		freeReplyObject(reply);
		NanReturnUndefined();
	}
	
	// make response
	Local<Value> response;
	if( reply->type == REDIS_REPLY_STRING ){
		response = NanNew<String>(reply->str);
	}
	
	freeReplyObject(reply);
	
	NanReturnValue(response);
}

//v8 entry point into Connection#srandmember
NAN_METHOD(Connection::Srandmember)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() ){
		NanThrowError("set name should be specified");
	}
	
	char* key = MallocCString(args[0]);
	
	int num = 1;
	if( args[1]->IsNumber() ){
		num = args[1]->IntegerValue();
	}
	
	// call hiredis
	redisReply *reply;
	redisAppendCommand(self->connection_,"SRANDMEMBER %s %d",key,num);
	int res = redisGetReply(self->connection_, (void**)&reply);
	
	if( res == REDIS_ERR ){
		LOG("SRANDMEMBER command failed");
		free(key);
		freeReplyObject(reply);
		NanReturnUndefined();
	}
	
	// make response
	Local<Array> response = NanNew<v8::Array>();
	unsigned int i = 0;
	if( reply->type == REDIS_REPLY_ARRAY ){
		unsigned int j;
		for( j=0; j<reply->elements; j++ ){
			if( reply->element[j]->type == REDIS_REPLY_STRING ){
				response->Set(i++,NanNew<String>(reply->element[j]->str));
			}
		}
	}
	
	free(key);
	freeReplyObject(reply);
	
	NanReturnValue(response);
}

// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 

//v8 entry point into Connection#flushall
NAN_METHOD(Connection::Flushall)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	// call hiredis (never fails)
	redisCommand(self->connection_,"FLUSHALL");
	
	NanReturnUndefined();
}

//v8 entry point into Connection#flushdb
NAN_METHOD(Connection::Flushdb)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	// call hiredis (never fails)
	redisCommand(self->connection_,"FLUSDB");
	
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




