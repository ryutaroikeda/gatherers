//
// gatherers.js
//
"use strict";

var HttpInterface = (function () {
  var pingDelay = 15000;
  function isReady(http) {
    return http.readyState == 4;
  }
  var i = {};
  i.send = function (method, url, f, msg) {
    var http = new XMLHttpRequest();
    http.open(method, url, true);
    if (method === "POST") {
      http.send(msg);
    } else {
      http.send();
    }
    http.onreadystatechange = function () {
      if (isReady(http)) {
        f(http);
      }
    }
  }

  i.head = function (url, f) {
    i.send("HEAD", url, f);
  }

  i.get = function (url, f) {
    i.send("GET", url, f);
  }

  i.post = function (url, f, msg) {
    i.send("POST", url, f, msg);
  }

  i.ping = function() {
    i.head("/", (function (http) {}));
  }

  i.keepAliveId = setInterval(i.ping, pingDelay);

  return i;
}());

var Gatherers = (function () {
  var g = {};

  g.consoleBox = document.getElementById("console");
  g.console = document.getElementById("consoleTxt");
  g.cmdline = document.getElementById("cmdline");

  g.consoleTxt = "Welcome to Gatherers.\nType info to begin.\n";

  g.command = "";

  g.update = function() {
    g.console.innerHTML = g.consoleTxt;
    g.consoleBox.scrollTop = g.consoleBox.scrollHeight;
    if (g.command) {
      g.cmdline.innerHTML = g.command;
    } else {
      g.cmdline.innerHTML = "Enter command:";
    }
  }

  g.receive = function (http) {
    g.consoleTxt += http.responseText;
    g.update();
  }

  g.send = function (cmd) {
    g.consoleTxt += g.command + "\n";
    HttpInterface.post("/", g.receive, "game: " + cmd);
    g.update();
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
      var chr = String.fromCharCode(key);
      if ("abcdefghijklmnopqrstuvwxyz0123456789".indexOf(chr) > -1) {
        Gatherers.command += String.fromCharCode(key);
      } else if (chr === " ") {
        Gatherers.command += " ";
        evt.preventDefault();
      }
    }
    Gatherers.update();
  }

  window.onbeforeunload = function (evt) {
    // Gatherers.send("exit");
  }

}());
