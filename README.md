
## GameState Subsystems

Replicated Subsystems! For GameStates!

## Why didn't you just replicate world subsystems?

Because World Subsystems have bizarre and awkward lifetimes that highly complicate replication and networking. It is completely possible with Iris, but in practice it was actually very awkward to use and didn't really offer any considerable benefits over GameState Subsystems. The other more technical reason is because a subsystem is a singleton, by it's nature the patterns end up matching that of the GameState when it's replicated anyway - you can't really meaningfully implement a Subsystem that behaves like the Player Controller because the Server would require an authoritative version for each player with individual remotes having their local proxy of said subsystem. However that doesn't work because it breaks the singleton pattern on the server due to requiring multiple instances.

## Why is this useful?

1. Primarily because upon successfully creating the well requested "Replicated WorldSubsystem" I realized that the only context it was actually useful in was one where you already had a GameState and due to the singleton behaviour of Subsystems it actually can't meaningfully function in any other way than like a GameState - aka. the Server may only have 1 instance of a Subsystem, therefore a setup similar to what you have with PlayerControllers isn't really possible.

2. I really don't like Lyra Experiences, however I do see the value of GameState Components from the Modular Gameplay Framework. I find this to be a bit of "best of both worlds", where you don't strictly need to setup some data asset defining which GameState Components are available, and you don't need to do manual SubObject management for the Components like you would with vanilla Modular Gameplay, rather they automatically register themselves.

## Contributions

PRs are welcome! Feel free to contibute <3
