//
// gatherers.js
//

var HttpInterface = (function () {
  var pingDelay = 5000;

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
        f(http);
      }
    }
  }
  i.get = function (url, f) {
    var http = new XMLHttpRequest();
    http.open("GET", url, true);
    http.send();
    http.onreadystatechange = function () {
      if (isReady(http)) {
        f(http);
      }
    }
  }
  i.post = function (url, f, msg) {
    var http = new XMLHttpRequest();
    http.open("POST", url, true);
    http.send();
    http.onreadystatechange = function () {
      if (isReady(http)) {
        f(http);
      }
    }
  }
  
  i.ping = function(url) {
    i.head(url, (function(http){}));
  }

  i.keepAliveId = setInterval(i.ping, pingDelay);

  return i;
}());

var Gatherers = (function () {
  var pingDelay = 5000;

  var g = {};

  g.button = document.getElementById("myButton");

  function printHttp(http) {
    console.log(http.statusText);
    console.log(http.responseText);
  }

  g.head = function (url, f) {
    var http = new XMLHttpRequest();
    http.open("HEAD", url, true);
    http.send();
    http.onreadystatechange = function () {
      if (http.readyState == 4) {
        f(http);
      }
    }
  }

  g.get = function(url, f) {
    var http = new XMLHttpRequest();
    http.onreadystatechange = function () {
      if (http.readyState == 4) {
        f(http);
      }
    }
    http.open("GET", url, true);
    http.send();
  }

  g.post = function(url, f, message) {
    var http = new XMLHttpRequest();
    http.onreadystatechange = function() {
      if (http.readyState == 4) {
        f(http);
      } 
    }
    http.open("POST", url, true);
    http.send(message);
  }

  g.button.onclick = function () {
    g.head("localhost", printHttp);
  }

  return g;
}());
