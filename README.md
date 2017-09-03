# lolstats
A tool to calculate your League of Legends winrates etc. from a saved match history

The program consists of 3 stages:
1. Load the file to a buffer
2. Read the buffer and put all data into a structure game by game
3. Calculate stats needed and print them out

Stage 3 could be its own .h file so you could quickly swap it out for a different one (if you wanted different stats)
