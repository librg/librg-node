const librg = require("../../main.js");
const myServerSecret = 23788787283782378;
let tickInterval;

const events = {
  onKillServer: 0,
  onOtherMsg1: 1,
  onOtherMsg2: 2
};

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

librg.start({
  host: "localhost",
  port: 22331 
});

tickInterval = setInterval(function() {
  librg.tick();
}, 0);


process.on('exit', function(code) {
  librg.stop();
  librg.dispose();
});