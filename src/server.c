#include "dbg.h"
#include "server.h"
#include "stream.h"
#include "writer.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

// static const char* httpMethod[GTHttpMethod_Size] =
// {
//   "", "GET", "HEAD", "POST"
// };

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

int GTSever_Init(GTServer* svr)
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
  conn = accept(svr->sock, NULL, NULL);
  if (conn == -1) {
    log_err("accept failed");
    return -1;
  }
  c->conn = conn;
  return 0;
}

int GTConnection_ParseRequest(GTConnection* c, GTStream* s)
{
  const char* SP = " ";
  const char* CR = "\r";
  const char* LF = "\n";
  enum { bufSize = 1024 };
  char buf[bufSize];
  c->res->status = GTHttpStatus_Ok;
  GTStream_GetToken(s, buf, bufSize, SP);
  if (strcmp(buf, "GET") == 0) {
    c->req->method = GTHttpMethod_Get;
  } else {
    c->res->status = GTHttpStatus_BadRequest;
    log_warn("bad request");
    return -1;
  }
  GTStream_GetToken(s, c->req->url, GTServer_UrlSize, SP);
  GTStream_GetToken(s, buf, bufSize, CR);
  if (strcmp(buf, "HTTP/1.1") != 0) {
    c->res->status = GTHttpStatus_BadRequest;
    log_warn("bad version");
    return -1;
  }
  GTStream_GetToken(s, buf, bufSize, LF);
  // ignore headers
  GTStream_GetToken(s, buf, bufSize, LF);
  while(strlen(buf)) {
    GTStream_GetToken(s, buf, bufSize, LF);
  }
  // ignore body
  // to do: parse body
  while (GTStream_Read(s, buf, bufSize) == 0) {
    // do nothing
  }
  return 0;
}

int GTConnection_GetRequest(GTConnection* c)
{
  GTStream stream;
  GTStream_InitSocket(&stream, c->conn);
  return GTConnection_ParseRequest(c, &stream);
}


int GTServer_HandleRequest(GTServer* svr, GTConnection* c)
{
  (void)svr;
  if (c->res->status != GTHttpStatus_Ok) { return -1; }
  if (strlen(c->req->body)) {
    // handle body
    return 0;
  }
  char file[GTServer_UrlSize];
  GTWriter w;
  GTWriter_InitString(&w, file, GTServer_UrlSize);
  GTWriter_Write(&w, "public_html/");
  if (strcmp(c->req->url, "/")) {
    GTWriter_Write(&w, "index.html");
  } else {
    // open file
  }
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
    GTConnection_Init(&c, &req, &res);
    GTServer_Accept(svr, &c);
    GTConnection_GetRequest(&c);
    GTServer_HandleRequest(svr, &c);
    GTConnection_SendResponse(&c);
    GTConnection_Close(&c);
  }
  return 0;
}
