const librg = require("../../main.js");

librg.initialize({
  tickDelay: 64,
  mode: "server",
  worldSize: [5000.0, 5000.0],
});

librg.on("connectionAccept", function(event) {
  console.log("someone connected to the server!\n");
});

librg.start({
  port: 22331 
});

setInterval(function() {
  librg.tick();
}, 0);

process.on('exit', function(code) {
  librg.stop();
  librg.dispose();
});