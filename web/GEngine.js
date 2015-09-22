"use strict";
function GEngine(canvas, context)
{
    this.canvas = canvas;
    this.context = context;
    this.tileWidth = 0;
    this.tileHeight = 0;
    this.map = {};
}

GEngine.prototype.getCanvasRect =
function(canvas)
{
    var x = 0;
    var y = 0;
    var w = canvas.width;
    var h = canvas.height;
    return new GRect(x, y, w, h);
}

GEngine.prototype.initialize =
function()
{
    this.addListeners();
}

GEngine.prototype.addListeners =
function()
{
    var registerEventInCapturingPhase = false;
    canvas.addEventListener("mouseup",
                            this.mouseUpHandler.bind(this),
                            registerEventInCapturingPhase);
}

GEngine.prototype.mouseUpHandler =
function(mouseEvent)
{
    var mouse = new GMouse(mouseEvent.offsetX, mouseEvent.offsetY, mouseEvent.button);
    console.log(this.getMapPosition(mouse));
}

GEngine.prototype.getMapPosition =
function(mouse)
{
    var mapPositionX = Math.floor((mouse.x )/this.tileWidth);
    var mapPositionY = Math.floor((mouse.y )/this.tileHeight);
    return mapPositionY * this.map.width + mapPositionX;
}

GEngine.prototype.applyMap =
function(map)
{
    this.tileWidth = Math.floor(canvas.width / map.width);
    this.tileHeight = Math.floor(canvas.height / map.height);
    this.map = map;
}

GEngine.prototype.clear =
function()
{
    context.clearRect(0, 0, this.canvas.width, this.canvas.height);
}

GEngine.prototype.drawRect =
function(rect, color)
{
    this.context.beginPath();
    this.context.rect(rect.x, rect.y, rect.w, rect.h);
    this.context.fillStyle = color;
    this.context.fill();
    this.context.closePath();
}

GEngine.prototype.draw =
function()
{
    this.clear();
    var rect = new GRect(20, 30, 50, 50);
    var color = "rgba(0, 0, 255, 1.0)";
    this.drawRect(rect, color);
}
