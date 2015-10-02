#ifndef _GTSERVER_H_
#define _GTSERVER_H_

#define enum_type(t) typedef enum t t
#define struct_type(t) typedef struct t t

enum
{
  GTHttp_UrlSize = 256,
  GTHttpRequest_BodySize = 2048
};

enum GTHttpVersion
{
  GTHttpVersion_None,
  GTHttpVersion_1_0,
  GTHttpVersion_1_1,
  GTHttpVersion_Size
};
enum_type(GTHttpVersion);

enum GTHttpMethod
{
  GTHttpMethod_None,
  GTHttpMethod_Get,
  GTHttpMethod_Head,
  GTHttpMethod_Post,
  GTHttpMethod_Size
}
enum_type(GTHttpMethod);

enum GTHttpHeader
{
  GTHttpHeader_None,
  // general header
  GTHttpHeader_Connection,
  // entity header
  GTHttpHeader_ContentEncoding,
  GTHttpHeader_ContentType,
  // request header
  GTHttpHeader_Host,
  GTHttpHeader_Size
};  
enum_type(GTHttpHeader);

struct GTHttpRequestLine
{
  GTHttpMethod method;
  GTHttpVersion version;
  char url[GTHttp_UrlSize];
};
struct_type(GTHttpRequestLine);

struct GTHttpRequest
{
  GTHttpRequestLine line;
  char body[GTHttpRequest_BodySize];
};
struct_type(GTHttpRequest);

enum GTHttpStatusCode
{
  GTHttpStatusCode_None,
  GTHttpStatusCode_Ok = 200,
  GTHttpStatusCode_BadRequest = 400,
  GTHttpStatusCode_ServerError = 500
}
enum_type(GTHttpStatusCode);

struct GTHttpStatusLine
{
  GTHttpVersion version;
  GTHttpStatusCode status;
};
struct_type(GTHttpStatusLine);

struct GTHttpServer
{

};
struct_type(GTHttpServer);

struct GTHttpClient
{

};
struct_type(GTHttpClient);

#endif
