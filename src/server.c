#include "dbg.h"
#include "ai.h"
#include "board.h"
#include "game.h"
#include "lexer.h"
#include "netcmd.h"
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

static const char* httpConn[GTHttpConnection_Size] =
{
  "",
  "keep-alive",
  "close"
};

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
  res->contentLength = 0;
  res->conn = GTHttpConnection_None;
  memset(res->body, 0, GTServer_BodySize);
  return 0;
}

int GTConnection_Init(GTConnection* c, GTRequest* req, GTResponse* res)
{
  c->req = req;
  c->res = res;
  return 0;
}

int GTSession_Init(GTSession* s)
{
  // s->ttl = GTServer_TimeToLive;
  s->conn = -1;
  s->isDone = 1;
  s->err = GTSessionError_None;
  return 0;
}

int GTServer_Init(GTServer* svr, in_port_t port)
{
  svr->port = port;
  svr->listenSize = 10;
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

int GTConnection_ParseRequestLine(GTConnection* c, GTStream* s)
{
  enum { bufSize = 1024 };
  char buf[bufSize];
  c->res->status = GTHttpStatus_Ok;
  // avoid getting empty methods
  GTStream_Skip(s, " \r\t\n");
  GTStream_GetToken(s, buf, bufSize, " ");
  debug("request method: %s", buf);
  if (strcmp(buf, "HEAD") == 0) {
    c->req->method = GTHttpMethod_Head;
  } else if (strcmp(buf, "GET") == 0) {
    c->req->method = GTHttpMethod_Get;
  } else if (strcmp(buf, "POST") == 0) {
    debug("POST request");
    c->req->method = GTHttpMethod_Post;
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
    // debug("%s", line);
    if (!strlen(line)) { 
      // eat the last newline
      GTStream_GetToken(s, line, lineSize, "\n");
      break;
    }
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
  // debug("parsing headers");
  err = GTConnection_ParseHeaders(c, s);
  if (err == -1) {
    return -1;
  }
  #define Min(x, y) ((x) < (y) ? (x) : (y))
  // debug("body is %ld bytes", Min(c->req->contentLength, GTServer_BodySize));
  GTStream_Read(s, c->req->body, Min(c->req->contentLength, GTServer_BodySize));
  #undef Min
  return 0;
}

// fixme?: set timeout for recv and check if connection is alive
// or alternatively, check ttl and close connection on timeout
int GTSession_GetRequest(GTSession* s, GTConnection* c)
{
  GTStream stream;
  GTStream_InitSocket(&stream, s->conn);
  return GTConnection_ParseRequest(c, &stream);
}

int GTServer_Filter(char* s, const char* filter)
{
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

int GTServer_CleanUrl(char* url)
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
  GTServer_Filter(tok, f);
  GTWriter_Write(&w, "%s", tok);
  tok = GTLexer_GetToken(&l, "\0");
  if (!tok) { return 0; }
  GTServer_Filter(tok, g);
  GTWriter_Write(&w, ".%s", tok);
  return 0;
}

int GTSession_DoCommand(GTSession* s, GTNetCommand* nc, GTWriter* w)
{
  (void) s;
  if (nc->type == GTNetCommandType_None) { 
    log_err("no command type");
    return -1;
  } else if (nc->type == GTNetCommandType_Start) {
    log_info("command start");
  } else if (nc->type == GTNetCommandType_Game) {
    GTGame_DoCommand(s->g, &nc->game, w);
    if (nc->game.err == GTCommandError_Illegal) {
      GTWriter_Write(w, "Illegal move, try again\n");
      log_err("illegal command");
      return -1;
    } else if (nc->game.err == GTCommandError_Exit) {
      s->isDone = 1;
      GTWriter_Write(w, "player %d resigns\n", s->g->p);
      log_info("player %d resigns\n", s->g->p);
      return 0;
    }
    GTGame_Print(s->g, w);
    if (GTGame_IsEnd(s->g)) {
      s->isDone = 1;
      GTWriter_Write(w, "player %d wins\n", s->g->p);
      log_info("player %d wins\n", s->g->p);
    }
  }
  return 0;
}

int GTSession_HandleAI(GTSession* s, GTWriter* w)
{
  // check if it is the AI's turn and the game is not ended
  while (s->ai->p == s->g->p && !s->isDone) {
    GTNetCommand nc;
    GTNetCommand_Init(&nc);
    nc.type = GTNetCommandType_Game;
    GTAI_PlayRandom(s->ai, &nc.game);
    GTCommand_Write(&nc.game, w);
    GTSession_DoCommand(s, &nc, w);
  }
  return 0;
}

int GTSession_HandleRequest(GTSession* s, GTConnection* c)
{
  if (c->res->status != GTHttpStatus_Ok) { return -1; }
  if (c->req->method == GTHttpMethod_Head) { return 0; }
  if (c->req->method == GTHttpMethod_Post) {
    GTNetCommand nc;
    GTNetCommand_Init(&nc);
    GTWriter w;
    GTWriter_InitString(&w, c->res->body, GTServer_BodySize);
    if (GTNetCommand_Parse(&nc, c->req->body) == -1) {
      GTWriter_Write(&w, "Error parsing command\n");
      log_err("error parsing command: %s", c->req->body);
      c->res->contentLength = strlen(c->res->body);
      return 0;
    }
    GTSession_DoCommand(s, &nc, &w);
    GTSession_HandleAI(s, &w);
    c->res->contentLength = strlen(c->res->body);
    return 0;
  }
  if (c->req->method != GTHttpMethod_Get) { return 0; }
  debug("raw url is %s", c->req->url);
  GTServer_CleanUrl(c->req->url);
  FILE* f = fopen(c->req->url, "r");
  if (f == NULL) {
    c->res->status = GTHttpStatus_NotFound;
    log_err("could not open file %s", c->req->url);
    return -1;
  }
  GTStream stream;
  GTStream_InitFile(&stream, f);
  if (GTStream_Read(&stream, c->res->body, GTServer_BodySize) == 0) {
    // we did not read the entire file
    log_warn("buffer too small for file");
  }
  c->res->contentLength = strlen(c->res->body);
  return 0;
}

int GTConnection_WriteResponse(GTConnection* c, GTWriter* w)
{
  GTWriter_Write(w, "HTTP/1.1 %d None\r\n", c->res->status);
  GTWriter_Write(w, "Content-Length: %ld\r\n", c->res->contentLength);
  if (c->res->conn != GTHttpConnection_None) {
    GTWriter_Write(w, "Connection: %s\r\n", httpConn[c->res->conn]);
  }
  GTWriter_Write(w, "\r\n");
  GTWriter_Write(w, "%s", c->res->body);
  return 0;
}

int GTSession_SendResponse(GTSession* s, GTConnection* c)
{
  GTWriter w;
  enum { bufSize = sizeof(GTResponse) };
  char buf[bufSize];
  GTWriter_InitString(&w, buf, bufSize);
  GTConnection_WriteResponse(c, &w);
  write(s->conn, buf, strlen(buf));
  return 0;
}

int GTSession_PrepareGame(GTSession* s, GTBoard* b, GTGame* g, GTAI* ai)
{
  char file[] =
   "tiles {"
  " w, m, h, i, w,"
  " p, h, w, m, h,"
  " m, i, p, m, i,"
  " i, m, p, i, m,"
  " h, m, w, h, p,"
  " w, i, h, m, w, }"
  "units {"
  "--,--,G1,--,--,"
  "--,--,--,--,--,"
  "--,--,--,--,--,"
  "--,--,--,--,--,"
  "--,--,--,--,--,"
  "--,--,g1,--,--, }";
  GTBoard_Init(b);
  GTBoard_Parse(b, file);
  GTGame_Init(g, b);
  GTAI_Init(ai);
  ai->p = GTPlayer_White;
  ai->b = b;
  s->b = b;
  s->g = g;
  s->ai = ai;
  s->isDone = 0;
  return 0;
}

int GTSession_HandleConnection(GTSession* s)
{
  GTBoard b;
  GTGame g;
  GTAI ai;
  GTSession_PrepareGame(s, &b, &g, &ai); 
  GTBoard_EndTurn(&b);
  while (s->err == GTSessionError_None && !s->isDone) {
    GTRequest req;
    GTResponse res;
    GTConnection c;
    GTRequest_Init(&req);
    GTResponse_Init(&res);
    GTConnection_Init(&c, &req, &res);
    GTSession_GetRequest(s, &c);
    GTSession_HandleRequest(s, &c);
    GTSession_SendResponse(s, &c);
  }
  log_info("closing connection");
  close(s->conn);
  return 0;
}

int GTServer_Run(GTServer* svr)
{
  int err;
  err = GTServer_Socket(svr);
  if (err == -1) {
    return -1;
  }
  in_port_t port = htons(svr->port);
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
    GTSession s;
    GTSession_Init(&s);
    s.conn = accept(svr->sock, NULL, NULL);
    if (s.conn == -1) {
      log_err("failed to establish connection");
      continue;
    }
    pid_t pid = fork();
    if (pid == -1) {
      log_err("fork failed");
      continue;
    } else if (pid == 0) {
      close(svr->sock);
      GTSession_HandleConnection(&s);
      break;
    } else{
      close(s.conn);
      continue;
    }
  }
  return 0;
}
