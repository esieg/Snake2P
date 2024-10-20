# Snake for 2Players
Let join the Snake-Game a second Player

## Compile
I use simple g++ for compiling and add the ncurses-Header
> g++ -std=c++17 -o Snake2P Snake2P.cpp -lncurses

## Start the game
Use this in the folder containing the Snake-Binary (Terminal should have at least 42x42 [columnsxrows])
> ./Snake2P
or 
> ./Snake2P -s/--simple
to start the game in the simple mode. Here you can give the snake directly the direction you want here, no need for a right/left steering with perspectives. 

## Steps
### Adjustments for 2 Player
#### Snake(s)
(/) add a second Snake
(/) add steering
(/) move snake
(/) draw snake
(/) change color
(/) draw score
#### new stuff
(/) let the first Fruit appears at a random position
(/) adjust the frame rate trough the sum of the lengths
(/) new collision: if a snake bites another, she loses
(/) set new win/lose conditions