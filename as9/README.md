# Matthew Osorio AS9

# Lore
You are John Rambo and have been hired to rescue a little girl that has been taken hostage by an evil organization. The evil organization has sent out monsters in an attempt to stop you. Your job is to avoid the monster at all costs and save the hostage.

# Images

# Description of criterion 

1. Must be implemented using DOECS
    - I followed the DOECS "template" provided to us. I created a scene, added entities to the scenes, used the entities as indecies to add compoents, and finally created systems to utilze the components
2. Must be interactable
    - I added keyboard functionally to my game. The keyboard starts the game and moves the player.
3. Must Draw or Play Sound
    - I drew numerous custom models to the scene and added a custom song to the game to act as background music.
4. EC: Must be custom made ^^
    - I got some models and sounds online and added them to the scene.
5. Must Display Text
    - At the start screen I display text advising the player to "Press Enter" to start the game. I also have different texts that will display depending on the outcome of the game.
6. Description of criterion
    - I am describing the criterion right now.
7. Window named AS9
    - I created a window names AS9
8. Minimum 2 Systems not Proposed in Class
    - I created two new components called "ProximityComponent" and "EnemyComponent". In addition there are two new systems called "ProximitySystem" and "GameActiveSytem" that uses those components respectively.
9. Read Me (with Pictures)
    - I added images of the game and its characters to the README.

# How To Run Code

```
git submodule update --init --recursive
mkdir build
cd build
cmake ..
make
./as9
```