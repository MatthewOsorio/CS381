## Assingmnet 4
Matthew Osorio
3/3/24

## Lore
You are a ship captain responible for transporting important cargo to the president of the United States. Coming to the end of your journey an enemy of the United States releases dangerous monsters to stop you from delivering the cargo. It is your jobs as the ships captain evade all the monsters, meanwhile traversing the horrible seas filled with gargantuan mountains.


## How to maneuver the ship
  W- increase the speed of the boat
  S- decrease the speed of the boat
  A- rotate the ship to the
## How to run program
Copy and make these commands;

```
mkdir build
cd build
cmake ..
make
./as2
```
## Controls
  W- plane moves away from camera
  S- Plane moves towards camera
  A- Plane moves left
  D- Plane moves right
  Q- Plane mvoes up
  E- PLane moves down
## Questions
5) No I don't think we can simplify the problem by having the camera be remapped to specific world axes because raylib uses a differnt coordinate system. Raylib has a left handing coordinate system whereas I believe in the real world we have a right handed coordinate system.

## Extra Credit
For extra credit I added three among us meshes to the scence. I thought it would be fun to have the plan to have something to fly around.

I believe it would be quite hard to implement camera movement. Assuming we want a first person POV of the plan, we need to continuous change the camera position based on the positon and angle of the plane. Since the plane is always moving that can make things complicated.

To make the plane move naturally, I believe we can modify the heading variable in accordance to the position. For example if we flying in the right direction and then we decide to fly the plan towards the camera, we would have to change heading so the plane gradually turns unit we are no longer turning.
