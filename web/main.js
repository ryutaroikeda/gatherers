var canvas = document.getElementById("myCanvas");
var context = canvas.getContext("2d");
var engine = new GEngine(canvas, context);

var mapWidth = 5;
var mapHeight = 6;
var plains = 4;
var wood = 6;
var horse = 6;
var iron = 6;
var mountain = 8;
var mapConfig = new GMapConfiguration(mapWidth,
									  mapHeight,
									  plains,
									  wood,
									  horse,
									  iron,
									  mountain);

var map = new GMap();
map.applyMapConfiguration(mapConfig);
console.log(map.map.toString());
function gameLoop()
{
	engine.clear();
	setInterval(engine.draw.bind(engine), 10);
}

gameLoop();