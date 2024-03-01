## Assingment 3 
Matthew Osorio
2/29/2023

## How to run program
Copy and paste these command 

```
git submodule update --init --recursive
mkdir build
cd build
cmake ..
make
./as3
```

## Controls
1) Press tab to select a plane
2) Press W to increase selected plane velocity
3) Press D to decrease selected plane velocity
4) Press A to increase selected plane heading
5) Press D to decrease selected plane heading
6) Press space to reset selected plane velocity

## Extra Credit
How difficult would it be to make the plane fly?

It would not be difficult at all. We can incorporate oriented physics for the plane tilting up/down when its flying upwards or downwards like how we did with the heading in this assingment. Just like how we made the plane move forwards based on the heading we can do the same with a planes tilt, resulitng in the plane flying. 