
## GameState Subsystems

Replicated Subsystems! For GameStates!

## Why?! - A few reasons actually:

1. Primarily because upon successfully creating the well requested "Replicated WorldSubsystem" I realized that the only context it was actually useful in was one where you already had a GameState and due to the singleton behaviour of Subsystems it actually can't meaningfully function in any other way than like a GameState - aka. the Server may only have 1 instance of a Subsystem, therefore a setup similar to what you have with PlayerControllers isn't really possible.

2. I really don't like Lyra Experiences, however I do see the value of GameState Components from the Modular Gameplay Framework. I find this to be a bit of "best of both worlds", where you don't strictly need to setup some data asset defining which GameState Components are available, and you don't need to do manual SubObject management for the Components like you would with vanilla Modular Gameplay, rather they automatically register themselves.

## Contributions

PRs are welcome! Feel free to contibute <3
