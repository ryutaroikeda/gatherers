#Gatherers

A boardgame for two players.

## How to play

### Commands
`mv` (pos) (dir) - for moving a unit
`pd` (pos) (dir) (unit) - for producing a unit
`rg` (pos) (dir) - for performing a ranged attack
`done` - for ending your turn
`info` - for displaying information about resources and population
`exit` - end the game

### Indicating position
Type the rank and file without spaces, e.g. c1.

### Indicating direction
`n` - north
`e` - east
`s` - south
`w` - west
`nn` - two tiles north
`ne` - north east
`ee` - two tiles east
`se` - south east
`ss` - two tiles south
`sw` - south west
`ww` - two tiles west
`nw` - north west

### Units
`g` or `G` - gatherer, who can produce other units
`a` or `A` - archer, who can perform ranged attacks
`c` or `C` - cavalryman, who can move twice per turn
`s` or `S` - spearman, who can take more damage
`f` or `F` - fortress

### Resources
`W` - wood, for producing an archer
`H` - horse, for producing a cavalryman
`I` - iron, for producing a spearman
`L` - lake, for producing a fortress
`P` - plain, just a plain plain
`M` - mountain, which gets in the way of archers and cavalry

### Objective
The objective is to destroy all of your opponent's gatherers.

### Reading the board
The board is a five-by-six grid of tiles. Each tile can hold at most one unit. If there is a unit on a tile, it will be displayed as something like 's3', which is a spearman. The number after the letter is the amount of damage that unit can take before being destroyed. Lower-case and upper-case letters are used to indicate ownership of the unit by the first and second player, respectively.

### Units
On your turn, you can issue commands to all of your units. You can issue at most one command to each unit per turn (except for cavalrymen). You can move a unit by typing, for example 'mv c1 n'. This will move your unit at position d1 north, to 'd2'. You can move your units north, east, south, or west, but not diagonally. You can attack enemy units with your cavalry and spearmen by attempting to move them to a tile occupied by an enemy unit.

### Resources and producing units
Each tile has a resource (see Resources above). You can look at the resources on the board by entering 'info'. You can only see resources on tiles that have been revealed - a tile is revealed when a unit moves on it. You acquire resources by moving one of your gatherers on a resource tile. Then, your gatherer can produce a unit associated with the resource. For example, a gatherer placed on W (wood) can produce an archer. However, you must be occupying more of the resource for that unit than there are units of that type. For example, if you have two wood and two archers on the board, you cannot create another archer until you acquire more wood or an archer is destroyed. 

You can issue a produce command by typing, for example, 'pd c1 n a', which means "produce, with the gatherer at c1, an archer on c2 (north of c1)".

### Producing gatherers
The gatherer is a special unit that does not require resources to be produced. A gatherer on any tile can produce a gatherer. However, on the turn that you produce a gatherer, you cannot produce any other unit.

### Ranged attack
Archers can perform a ranged attack on enemy units up to two tiles away. To perform a ranged attack, type, for example, 'rg b2 nn' to attack the unit on tile b4 with your archer on b2. 

### Mountains
A cavalryman must stop moving if it lands on a mountain tile. An archer cannot perform a ranged attack over mountains. If the ranged attack is over an unrevealed tile, that tile is revealed. If it is a mountain, the ranged attack fails.

## To do
* <del>server</del>
* <del>client javascript</del>
* <del>user manual</del>
* board tile randomisation
* AI
* <del>turns</del>
* <del>win check and resignation</del>
* <del>command line interface</del>
	* <del>'info' command</del>
* <del>board parser</del>
* save and load game
* license

### Optional
* GUI
