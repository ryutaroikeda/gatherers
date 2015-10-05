"use strict";
function GMap()
{
    this.width = 0;
    this.height = 0;
    this.map = [];
}

GMap.prototype.getIndex =
function(x, y)
{
    return x * this.width + y;
}

GMap.prototype.getTile =
function(x, y)
{
    return this.map[this.getIndex(x, y)];
}

GMap.prototype.setTile = 
function(tile, x, y)
{
    this.map[this.getIndex(x, y)];
}

GMap.prototype.shuffle =
function(array)
{
    var currentIndex = array.length;
    var temporaryValue;
    var randomIndex;
    while(0 < currentIndex)
    {
        randomIndex = Math.floor(Math.random() * currentIndex);
        currentIndex -= 1;
        temporaryValue = array[currentIndex];
        array[currentIndex] = array[randomIndex];
        array[randomIndex] = temporaryValue;
    }
    return array;
}

GMap.prototype.applyMapConfiguration = 
function(mapConfiguration)
{
    this.width = mapConfiguration.width;
    this.height = mapConfiguration.height;
    var currentIndex = 0;
    for(var i = 0; i < GTerrain.size; i++)
    {
        for(var j = 0; j < mapConfiguration.terrain[i]; j++)
        {
            this.map[currentIndex] = i;
            currentIndex += 1;
        }
    }
    this.shuffle(this.map);
}