const librg = require("../../main.js");
const myServerSecret = 23788787283782378;

librg.initialize({
  tickDelay: 64,
  mode: "client",
  worldSize: [5000.0, 5000.0],
});

librg.on("connectionRequest", function(entity) {
});

librg.on("connectionAccept", function(entity) {
});

librg.on("entityCreate", function(entity) {
});

librg.on("entityUpdate", function(entity) {
});

librg.on("entityRemove", function(entity) {
});

librg.start({
  host: "localhost",
  port: 22331 
});

process.on('exit', function(code) {
  librg.stop();
  librg.dispose();
});