Assignment 1.03: Path Finding
Luke Pavia

Progressions I made from last week's assignment includes adding a playable character to the map and 
using hikers and rivals to pathfind. I used the movement costs for both hikers and rivals for each
type of cell with Dikstra's Algorithm to calculate the distance to the playable character from every
location in the map for hikers and rivals. Movement is calculated in all eight directions including 
diagonal. This is displayed in two 2D distance maps, the top one for hiker and the bottom one for 
Rival. The distance in the distance maps is displayed as mod 100 as required. Lastly, any 
unreachable cells are displayed as blank spaces also as required in the assignment.