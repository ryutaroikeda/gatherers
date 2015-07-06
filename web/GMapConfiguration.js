function GMapConfiguration(width, height, plains, wood, horse, iron, mountain)
{
    this.width = width;
    this.height = height;
    this.terrain = [];
    this.terrain[GTerrain.plains] = plains;
    this.terrain[GTerrain.wood] = wood;
    this.terrain[GTerrain.horse] = horse;
    this.terrain[GTerrain.iron] = iron;
    this.terrain[GTerrain.mountain] = mountain;
}