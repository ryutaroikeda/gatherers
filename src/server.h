#ifndef _GTSERVER_H_
#define _GTSERVER_H_

#include <netinet/in.h>
#define enum_type(t) typedef enum t t
#define struct_type(t) typedef struct t t

struct GTAI;
struct GTBoard;
struct GTGame;
struct GTStream;

enum
{
  GTServer_UrlSize = 255,
  GTServer_HostSize = 255,
  GTServer_BodySize = 8192,
  // GTServer_ClientSize = 1,
  GTServer_TimeToLive = 60
};

enum GTHttpMethod
{
  GTHttpMethod_None,
  GTHttpMethod_Get,
  GTHttpMethod_Head,
  GTHttpMethod_Post,
  GTHttpMethod_Size
};
enum_type(GTHttpMethod);

enum GTHttpStatus
{
  GTHttpStatus_None,
  GTHttpStatus_Ok = 200,
  GTHttpStatus_BadRequest = 400,
  GTHttpStatus_NotFound = 404,
  GTHttpStatus_ServerError = 500
};
enum_type(GTHttpStatus);

enum GTHttpConnection {
  GTHttpConnection_None,
  GTHttpConnection_KeepAlive,
  GTHttpConnection_Close,
  GTHttpConnection_Size
};
enum_type(GTHttpConnection);

enum GTHttpHeader
{
  GTHttpHeader_None,
  // general header
  GTHttpHeader_Connection,
  // entity header
  // GTHttpHeader_ContentEncoding,
  // GTHttpHeader_ContentType,
  GTHttpHeader_ContentLength,
  // request header
  GTHttpHeader_Host,
  GTHttpHeader_Size
};  
enum_type(GTHttpHeader);

struct GTRequest
{
  GTHttpMethod method;
  long int contentLength;
  char url[GTServer_UrlSize];
  char host[GTServer_HostSize];
  char body[GTServer_BodySize];
};
struct_type(GTRequest);

int GTRequest_Init(GTRequest* req);

struct GTResponse
{
  GTHttpStatus status;
  long int contentLength;
  GTHttpConnection conn;
  char body[GTServer_BodySize];
};
struct_type(GTResponse);

int GTResponse_Init(GTResponse* res);

struct GTConnection
{
  // int conn;
  GTRequest* req;
  GTResponse* res;
};
struct_type(GTConnection);

int GTConnection_Init(GTConnection* c, GTRequest* req, GTResponse* res);

int GTConnection_ParseRequest(GTConnection* c, struct GTStream* s);

enum GTSessionError {
  GTSessionError_None,
  GTSessionError_Size
};
enum_type(GTSessionError);

struct GTSession {
  int conn;
  int isDone;
  // long ttl;
  struct GTBoard* b;
  struct GTGame* g;
  struct GTAI* ai;
  GTSessionError err;
};
struct_type(GTSession);

int GTSession_Init(GTSession* s);

struct GTServer {
  in_port_t port;
  int listenSize;
  int sock;
};
struct_type(GTServer);

int GTServer_Init(GTServer* svr, in_port_t port);

int GTServer_Filter(char* s, const char* f);

int GTServer_CleanUrl(char* url);

int GTServer_Run(GTServer* svr);

#undef enum_type
#undef struct_type 
#endif
