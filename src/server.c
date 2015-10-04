#include "dbg.h"
#include "lexer.h"
#include "server.h"
#include "stream.h"
#include "writer.h"
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

// static const char* httpMethod[GTHttpMethod_Size] =
// {
//   "", "GET", "HEAD", "POST"
// };

// static const char* httpHeader[GTHttpHeader_Size] =
// {
//   "",
//   "Connection",
//   "Content-Length",
//   "Host",
// };

int GTRequest_Init(GTRequest* req)
{
  req->method = GTHttpMethod_None;
  req->contentLength = 0;
  memset(req->url, 0, GTServer_UrlSize);
  memset(req->host, 0, GTServer_HostSize);
  memset(req->body, 0, GTServer_BodySize);
  return 0;
}

int GTResponse_Init(GTResponse* res)
{
  res->status = GTHttpStatus_None;
  memset(res->body, 0, GTServer_BodySize);
  return 0;
}

int GTConnection_Init(GTConnection* c, GTRequest* req, GTResponse* res)
{
  c->conn = -1;
  c->req = req;
  c->res = res;
  return 0;
}

int GTConnection_Close(GTConnection* c)
{
  return close(c->conn);
}

int GTServer_Init(GTServer* svr)
{
  svr->listenSize = 10;
  svr->clientSize = 0;
  svr->sock = -1;
  return 0;
}

int GTServer_Socket(GTServer* svr)
{
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    log_err("could not create socket");
    return -1;
  }
  svr->sock = sock;
  return 0;
}

int GTServer_Bind(GTServer* svr, in_port_t port, struct in_addr addr)
{
  int err;
  struct sockaddr_in sockaddr = {
    .sin_family = AF_INET,
    .sin_port = port,
    .sin_addr = addr
  };
  err = bind(svr->sock, (struct sockaddr*)&sockaddr,
   sizeof(struct sockaddr_in));
  if (err == -1) {
    log_err("could not bind socket");
    return -1;
  }
  return 0;
}

int GTServer_Listen(GTServer* svr)
{
  int err;
  err = listen(svr->sock, svr->listenSize);
  if (err == -1) {
    log_err("listen failed");
    return -1;
  }
  return 0;
}

int GTServer_Accept(GTServer* svr, GTConnection* c) 
{
  int conn;
  log_info("awaiting client...");
  conn = accept(svr->sock, NULL, NULL);
  if (conn == -1) {
    log_err("accept failed");
    return -1;
  }
  c->conn = conn;
  return 0;
}

int GTConnection_ParseRequestLine(GTConnection* c, GTStream* s)
{
  enum { bufSize = 1024 };
  char buf[bufSize];
  c->res->status = GTHttpStatus_Ok;
  GTStream_GetToken(s, buf, bufSize, " ");
  debug("request method: %s", buf);
  if (strcmp(buf, "GET") == 0) {
    c->req->method = GTHttpMethod_Get;
  } else {
    c->res->status = GTHttpStatus_BadRequest;
    log_err("bad request");
    return -1;
  }
  GTStream_GetToken(s, c->req->url, GTServer_UrlSize, " ");
  GTStream_GetToken(s, buf, bufSize, "\r");
  if (strcmp(buf, "HTTP/1.1") != 0) {
    c->res->status = GTHttpStatus_BadRequest;
    log_err("bad version");
    return -1;
  }
  GTStream_GetToken(s, buf, bufSize, "\n");
  return 0;
}

int GTConnection_ParseHeader(GTConnection* c, GTStream* s)
{
  enum { tokSize = 256 };
  char tok[tokSize];
  GTStream_GetToken(s, tok, tokSize, ":");
  if (strcmp(tok, "Content-Length") == 0) {
    GTStream_Skip(s, " ");
    GTStream_GetToken(s, tok, tokSize, " \t\r\n");
    c->req->contentLength = strtol(tok, NULL, 10);
  } else if (strcmp(tok, "Host") == 0) {
    GTStream_Skip(s, " ");
    GTStream_GetToken(s, c->req->host, GTServer_HostSize, " \t\r\n");
  }
  return 0;
}

int GTConnection_ParseHeaders(GTConnection* c, GTStream* s)
{
  enum {
   lineSize = 1024
  };
  char line[lineSize];
  while (1) {
    GTStream_GetToken(s, line, lineSize, "\r");
    debug("%s", line);
    if (!strlen(line)) { break; }
    GTStream t;
    GTStream_InitString(&t, line);
    GTConnection_ParseHeader(c, &t);
    // end of current header
    GTStream_GetToken(s, line, lineSize, "\n");
  }
  return 0;
}

int GTConnection_ParseRequest(GTConnection* c, GTStream* s)
{
  int err;
  err = GTConnection_ParseRequestLine(c, s);
  if (err == -1) {
    return -1;
  }
  debug("parsing headers");
  err = GTConnection_ParseHeaders(c, s);
  if (err == -1) {
    return -1;
  }
  #define Min(x, y) ((x) < (y) ? (x) : (y))
  debug("body is %ld bytes", Min(c->req->contentLength, GTServer_BodySize));
  GTStream_Read(s, c->req->body, Min(c->req->contentLength, GTServer_BodySize));
  #undef Min
  return 0;
}

int GTConnection_GetRequest(GTConnection* c)
{
  GTStream stream;
  GTStream_InitSocket(&stream, c->conn);
  return GTConnection_ParseRequest(c, &stream);
}

int GTServer_Filter(GTServer* svr, char* s, const char* filter)
{
  (void) svr;
  int i;
  char* next = s;
  const int len = strlen(s);
  for (i = 0; i < len; i++) {
    if (strchr(filter, s[i])) {
      *(next++) = s[i];
    }
  }
  *next = '\0';
  return 0;
}

int GTServer_CleanUrl(GTServer* svr, char* url)
{
  static const char* f =
  "abcdefghijklmnopqrstuvwxyz"
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "0123456789"
  "-_/";
  static const char* g =
  "abcdefghijklmnopqrstuvwxyz"
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "0123456789"
  "-_";
  (void) svr;
  char buf[GTServer_UrlSize];
  strncpy(buf, url, GTServer_UrlSize);
  memset(url, 0, GTServer_UrlSize);
  GTWriter w;
  GTWriter_InitString(&w, url, GTServer_UrlSize);
  GTWriter_Write(&w, "public_html/");
  GTLexer l;
  GTLexer_Init(&l, buf);
  GTLexer_Skip(&l, "/");
  char* tok = GTLexer_GetToken(&l, ".");
  if (!tok) {
    GTWriter_Write(&w, "index.html");
    return 0;
  }
  GTServer_Filter(svr, tok, f);
  GTWriter_Write(&w, "%s", tok);
  tok = GTLexer_GetToken(&l, "\0");
  if (!tok) { return 0; }
  GTServer_Filter(svr, tok, g);
  GTWriter_Write(&w, ".%s", tok);
  return 0;
}

int GTServer_HandleRequest(GTServer* svr, GTConnection* c)
{
  if (c->res->status != GTHttpStatus_Ok) { return -1; }
  if (strlen(c->req->body) && c->req->method == GTHttpMethod_Post) {
    // handle body
    (void)svr;
    return 0;
  }
  if (c->req->method != GTHttpMethod_Get) { return 0; }
  debug("raw url is %s", c->req->url);
  GTServer_CleanUrl(svr, c->req->url);
  FILE* f = fopen(c->req->url, "r");
  if (f == NULL) {
    c->res->status = GTHttpStatus_ServerError;
    log_warn("could not open file %s", c->req->url);
    return -1;
  }
  GTStream s;
  GTStream_InitFile(&s, f);
  GTStream_Read(&s, c->res->body, GTServer_BodySize);
  return 0;
}

int GTConnection_WriteResponse(GTConnection* c, GTWriter* w)
{
  GTWriter_Write(w, "HTTP/1.1 %d None\r\n", c->res->status);
  // no header
  GTWriter_Write(w, "\r\n\r\n");
  GTWriter_Write(w, "%s\r\n\r\n", c->res->body);
  return 0;
}

int GTConnection_SendResponse(GTConnection* c)
{
  GTWriter w;
  enum { bufSize = sizeof(GTResponse) };
  char buf[bufSize];
  GTWriter_InitString(&w, buf, bufSize);
  GTConnection_WriteResponse(c, &w);
  write(c->conn, buf, strlen(buf));
  return 0;
}

int GTServer_Run(GTServer* svr)
{
  int err;
  err = GTServer_Socket(svr);
  if (err == -1) {
    return -1;
  }
  in_port_t port = htons(80);
  struct in_addr addr = {
    .s_addr = htonl(INADDR_ANY)
  };
  err = GTServer_Bind(svr, port, addr);
  if (err == -1) {
    close(svr->sock);
    return -1;
  }
  err = GTServer_Listen(svr);
  if (err == -1) {
    close(svr->sock);
    return -1;
  }
  while (1) {
    GTConnection c;
    GTRequest req;
    GTResponse res;
    GTRequest_Init(&req);
    GTResponse_Init(&res);
    GTConnection_Init(&c, &req, &res);
    GTServer_Accept(svr, &c);
    GTConnection_GetRequest(&c);
    GTServer_HandleRequest(svr, &c);
    GTConnection_SendResponse(&c);
    GTConnection_Close(&c);
  }
  return 0;
}
