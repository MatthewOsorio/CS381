# Matthew Osorio AS9

# Lore
You are John Rambo and have been hired to rescue a little girl that has been taken hostage by an evil organization. The evil organization has sent out monsters in an attempt to stop you. Your job is to avoid the monster at all costs and save the hostage.

# Images
<img width="1198" alt="Screenshot 2024-05-04 at 10 40 41 PM" src="https://github.com/MatthewOsorio/CS381/assets/112652339/d78180a0-88e4-4b32-806b-475b28997f62">
<img width="424" alt="Screenshot 2024-05-04 at 11 18 25 PM" src="https://github.com/MatthewOsorio/CS381/assets/112652339/ccee2392-b2e9-4e8b-aa7c-e15c7fb907ff">
<img width="292" alt="Screenshot 2024-05-04 at 11 22 23 PM" src="https://github.com/MatthewOsorio/CS381/assets/112652339/c44f9e18-2f87-4254-bbae-a183f93b070b">

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
10. EC: Grade Yourself
    - 100/100 excluding extra credit. 115/100 including extra credit.
# How To Run Code

```
git submodule update --init --recursive
mkdir build
cd build
cmake ..
make
./as9
```
