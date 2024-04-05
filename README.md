35Manager is a basic command-line software that helps Dungeon Masters (DMs) and players automate aspects of character and party management in Dungeons & Dragons v3.5 (the edition released in 2003). It could also be used, with minimal adaptation, for Pathfinder 1e and other such d20 systems.

35Manager allows DMs to store information on player characters, NPCs, monsters, and anything else with a statblock in human-readable plaintext files. The software automatically calculates hit point totals, saving throw bonuses, and skill check bonuses--in my experience, it does so more reliably than humans do.

Once character data is entered into 35Manager, DMs can use the software to make saving throws and skill checks for an entire party at once. It's perfect for group Spot and Listen checks, hidden skill checks, and hidden saving throws. It's also very useful for NPC parties, who don't benefit from having other humans to roll their dice. The DM can even take timestamped session notes right in the software.

To get starting using 35Manager, go to the Characters folder. The characterFormat.ddc file gives you an overview of what each part of the character file does. When you're ready to use the programme, run the executable. Type anything and then ENTER to see the list of available commands. If you type in an invalid command in the future, the software will show you this list again.

=====Technical Details=====
The .ddc file extension, short for 'D&D Character', is used for all character sheet input to 35Manager.
Any file that does not have the .ddc extension (and is not a special .txt file) but is in a group folder will be ignored by the programme.

Any folder that has a name starting with "_" will be ignored by the group-reading process.

By default, groups have the same name as their folder.
If a 'name.txt' file is present, the first line of that file replaces the default name as the group's name. All subsequent lines are alternate group names (i.e., aliases).
A 'notes.txt' file in a group is used to record random notes for the group.
