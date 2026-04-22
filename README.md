This is a personal project, created to help automate work I found myself doing on a regular basis as a part of an hobby. Often, Dungeon Masters in a game of *Dungeons & Dragons* v3.5 (or any similar system, like Pathfinder 1e) find themselves performing dice rolls for 3-6 player characters at once. D&D v3.5, in particular, calls for many dice rolls to be performed secretly, so that players cannot necessarily see the results. This means that the dice rolls cannot be offloaded onto the players themselves. Although many DMs may have access to the players' character sheets, quickly finding the relevant numbers, rolling several dice, and applying modifiers, all to resolve an event that should happen quickly and seamlessly, is difficult. This tool automates that process with a simple, easy-to-use CLI application. The interface is designed primarily for speed to keep the game flowing smoothly. I hope you find it as useful as I have!

This project functions as a session manager for D&D 3.5e. It allows DMs to keep records of skill check roll times, to log key events in a central location, to import PC and NPC statblocks through formatted text files, and, most importantly, to make full-party skill checks.

The Characters directory stores parties of characters. See the applicable README for further details.

If characterData.cc is compiled with -D NO_FRAC, then 35Manager will not apply fractional base bonus rules (Unearthed Arcana, page 73). By default, it will apply those rules.

After running 35Manager, enter anything to see the available commands.
