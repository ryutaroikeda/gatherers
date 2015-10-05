//
// gatherers.js
//

var Gatherers = (function () {
  var g = {};
  var pingDelay = 30000;
  g.head = function (url) {
    var http = new XMLHttpRequest();
    http.open("HEAD", url, true);
    http.send();
  }
  // g.keepAlive = setInterval(head, pingDelay);
  // g.get = function(url, f) {
  //   var http = new XMLHttpRequest();
  //   http.onreadystatechange = function () {
  //     if (http.readyState == 4 && http.status == 200) {
  //       f(http);
  //     }
  //   }
  //   http.open("GET", url, true);
  //   http.send();
  // }
  // g.post = function(url, f, message) {
  //   var http = new XMLHttpRequest();
  //   http.onreadystatechange = function() {
  //     if (http.readyState == 4 && http.status == 200) {
  //       f(http);
  //     } 
  //   }
  //   http.open("POST", url, true);
  //   http.send(message);
  // }
  g.button = document.getElementById("myButton");
  g.button.onclick = function () {
    g.head("localhost");
  }
  return g;
}());


