const librg = require("../../main.js");

librg.initialize({
  tickDelay: 64,
  mode: "client",
  worldSize: [5000.0, 5000.0],
});

librg.on("entityCreate", function(event) {
  console.log("creating entity with id: "+event.entity.id+", of type: "+event.entity.type+", on position: "+event.entity.position.x+" "+event.entity.position.y+" "+event.entity.position.z);
});

librg.on("entityUpdate", function(event) {
  console.log("updating position for entity with id: "+event.entity.id+", of type: "+event.entity.type+", on position: "+event.entity.position.x+" "+event.entity.position.y+" "+event.entity.position.z);
});

librg.on("entityRemove", function(event) {
  console.log("destroying entity with id: "+event.entity.id);
});

librg.start({
  host: "localhost",
  port: 22331 
});

setInterval(function() {
  librg.tick();
}, 0);

process.on('exit', function(code) {
  librg.stop();
  librg.dispose();
});