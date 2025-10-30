# CS50 Nuggets Game
#### Section 1, Team 10: Joseph Quaratiello, Anna Filyurina, Nafis Saadiq Bhuiyan, Caroline Chung, Nov. 2024
## Game Description
The Nuggets game is a multiplayer treasure hunt set on a grid-based map. Players explore the map to collect hidden gold, which is represented as gold piles scattered across the map. Each player is represented by a unique letter, and their goal is to collect as much gold as possible before the game ends. A spectator can also join to watch the game in real time.

Players move around the map using specific keys and can interact with other players or gold piles. The game ends when all gold piles have been collected.

## Game Rules
1. **Players and Map**:
   - Each player is represented by a unique letter (A-Z).
   - The map consists of walls (`#`), open spaces (`.`), and gold piles (`*`).

2. **Gameplay**:
   - Players can move in all cardinal directions (`h`, `l`, `j`, `k`) and diagonals (`y`, `u`, `b`, `n`).
   - Players can also move to the furthest position in a direction by using capital letters (`H`, `L`, `J`, `K`, `Y`, `U`, `B`, `N`).
   - If a player lands on a gold pile (`*`), they collect the gold, which is added to their total.

3. **Winning**:
   - The game ends when all gold piles have been collected.
   - A final score summary is displayed showing each player's total gold.

4. **Spectators**:
   - One spectator can join the game to view the entire map and track the game in real time.
   - If a new spectator joins, the current spectator is removed.

5. **Special Features**:
   - Players can "steal" gold from other players if they step onto the same space.
   - In "plain" mode, extra features like stealing are disabled.

6. **Commands**:
   - Players send movement commands (`KEY <direction>`).
   - A spectator can join by sending `SPECTATE`. When joining the server, you are a spectator if u provide no playerName argument.
   ```bash 
       ./client <serverName>
   ```
   - Players can quit the game with the `Q` command.

## Usage
To start the server, run:

```bash
./server <mapfile> [seed]
```

### Positional Arguments
- `<mapfile>`: The path to the map file defining the game map.
- `[seed]`: A positive integer seed for random number generation.

#### Custom Map file: maps/team10_custom_map.txt