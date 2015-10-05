//
// gatherers.js
//
"use strict";

var HttpInterface = (function () {
  var pingDelay = 10000;

  function isReady(http) {
    return http.readyState == 4;
  }
  var i = {};
  i.head = function (url, f) {
    var http = new XMLHttpRequest();
    http.open("HEAD", url, true);
    http.send();
    http.onreadystatechange = function () {
      if (isReady(http)) {
        f(http.responseText);
      }
    }
  }
  i.get = function (url, f) {
    var http = new XMLHttpRequest();
    http.open("GET", url, true);
    http.send();
    http.onreadystatechange = function () {
      if (isReady(http)) {
        f(http.responseText);
      }
    }
  }
  i.post = function (url, f, msg) {
    var http = new XMLHttpRequest();
    http.open("POST", url, true);
    http.setRequestHeader("Content-type", "game");
    http.send(msg);
    http.onreadystatechange = function () {
      if (isReady(http)) {
        f(http.responseText);
      }
    }
  }
  
  i.ping = function(url) {
    i.head(url, (function(text){}));
  }

  i.keepAliveId = setInterval(i.ping, pingDelay);

  return i;
}());

var Gatherers = (function () {
  var g = {};

  g.consoleBox = document.getElementById("console");
  g.console = document.getElementById("consoleTxt");
  g.cmdline = document.getElementById("cmdline");

  g.consoleTxt = "Welcome to Gatherers.";

  g.command = "";

  g.receive = function (msg) {
    g.consoleTxt += msg;
    g.update();
  }

  g.send = function () {
    HttpInterface.post("/", g.receive, "game: " + g.command);
  }

  g.update = function() {
    g.console.innerHTML = g.consoleTxt;
    g.consoleBox.scrollTop = g.consoleBox.scrollHeight;
    g.cmdline.innerHTML = g.command;
  }

  return g;
}());

(function() {
  document.onkeypress = function (evt) {
    var key = evt.charCode || evt.which || evt.keyCode;
    if (key == 13) {
      Gatherers.send(Gatherers.command);
      Gatherers.command = "";
    } else if (key == 46 || key == 8 || key == 63272) {
      Gatherers.command = Gatherers.command.slice(0, -1);
      evt.preventDefault();
    } else {
      Gatherers.command += String.fromCharCode(key);
    }
    Gatherers.update();
  }
}());
