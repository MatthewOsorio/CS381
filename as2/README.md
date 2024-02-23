## Assingmnet 2
Matthew Osorio
2/18/23

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
