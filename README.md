# Findseed
Findseed is a discord bot exclusive to the [Minecraft TAS Discord Community](https://discord.gg/jGhNxpd). This small bot adds a command called /findseed, which returns a randomly generated end portal. The end portal can have up to 12 eyes <sup>(or can it?!)</sup>. *There's something odd with this bot and I'm not sure what...*

The idea of this bot originated from [admiral_stapler](https://www.twitch.tv/admiral_stapler) and [Illumina](https://www.twitch.tv/illumina1337)'s twitch chat. The original message for `!findseed` was `<user> -> Your seed is a <eye count> eye.`. Since then, [Emma (Curcuit)](https://www.youtube.com/@Curcuit) made 2 minecraft maps about findseed, both of which are on [speedrun.com](https://www.speedrun.com/findseed). Now the bot has been made into a discord bot that can be used by anyone in the Minecraft TAS Discord Community.

## Usage
To use the bot, simply type `/findseed` in any channel the bot has access to (preferably [#bot-spam](https://discord.com/channels/373166430478401555/889595617305837649)). The bot will then generate a seed and return an end portal. You can also provide a seed yourself

## Libraries
This bot uses the following libraries:
- [concord v2.2.1.r18.g25aceab6-1](https://github.com/Cogmasters/concord/commit/25aceab6c73da646f94b8c9de25e4d8013938e8e)
- [libcurl 8.6.0-3](https://github.com/curl/curl/tree/curl-8_6_0)
- [zalgo (embedded in source)](https://github.com/maeln/zalgo)