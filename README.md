
## GameState Subsystems

Does what it says on the tin. Extensible GameState via subsystems rather than GameState Components.
To setup please enable the plugin and reparent your GameState to AExtensibleGameStateBase, then just create a UGameStateSubsystem and it will be replicated and initialized automatically.

You can find an example of the functionality in UExampleGameStateSubsystem, it's disabled by default, to turn it on just change ShouldCreateSubsystem to return true.

## Why didn't you just replicate world subsystems?

Because World Subsystems have bizarre and awkward lifetimes that highly complicate replication and networking. It is completely possible with Iris, but in practice it was actually very awkward to use and didn't really offer any considerable benefits over GameState Subsystems. The other more technical reason is because a subsystem is a singleton, by it's nature the patterns end up matching that of the GameState when it's replicated anyway - you can't really meaningfully implement a Subsystem that behaves like the Player Controller because the Server would require an authoritative version for each player with individual remotes having their local proxy of said subsystem. However that doesn't work because it breaks the singleton pattern on the server due to requiring multiple instances.

## Why is this useful?

I am not the biggest fan of the Lyra Experience system, however I do see it's value - especially in GameState Components. However for some cases I think that it would be nice if they sort of just registered themselves and "just worked"tm rather than needing to be manually added via a SubObject or added to a Lyra Experience. So I did, and I find this solution to be a bit of a best of both worlds.

## Support

This code was designed to work on a version > 5.3 but it's likely possible to backport the code. It doesn't necessarily do any black magic or special things that are new to the engine.

## Contributions

PRs are welcome! Feel free to contibute <3
