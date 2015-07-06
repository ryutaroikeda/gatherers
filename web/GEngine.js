function GEngine(canvas, context)
{
    this.canvas = canvas;
    this.context = context;
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
