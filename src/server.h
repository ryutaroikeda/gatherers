#ifndef _GTSERVER_H_
#define _GTSERVER_H_

#include "board.h"

#define enum_type(t) typedef enum t t
#define struct_type(t) typedef struct t t

struct GTStream;

enum
{
  GTServer_UrlSize = 255,
  GTServer_HostSize = 255,
  GTServer_BodySize = 8192,
  GTServer_ClientSize = 1
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
  GTHttpStatus_ServerError = 500
};
enum_type(GTHttpStatus);

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
  char body[GTServer_BodySize];
};
struct_type(GTResponse);

int GTResponse_Init(GTResponse* res);

struct GTConnection
{
  int conn;
  GTRequest* req;
  GTResponse* res;
};
struct_type(GTConnection);

int GTConnection_Init(GTConnection* c, GTRequest* req, GTResponse* res);

int GTConnection_ParseRequest(GTConnection* c, struct GTStream* s);

struct GTClient
{
  int id;
};
struct_type(GTClient);

struct GTSession
{
  GTPlayer client;
};

struct GTServer
{
  int listenSize;
  int clientSize;
  int sock;
  // GTHttpClient clients[GTHttpServer_ClientSize];
};
struct_type(GTServer);

int GTServer_Init(GTServer* svr);

int GTServer_Filter(GTServer* svr, char* s, const char* f);

int GTServer_CleanUrl(GTServer* svr, char* url);

int GTServer_Run(GTServer* svr);

#undef enum_type
#undef struct_type 
#endif
