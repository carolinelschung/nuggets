**Total: 89/100 points** Team 1-10

CANVAS final project grade XXX %  SCORE*80% + Attendance*20%

```
  Base score: 86/100
Extra credit: +3 
----------------------
 Final score: 89/100
```

## (15) Design spec & Presentation

12/15 points:

Design discussion (3 points)

- 1/1 Good overview of the design.
- 1/1 Decomposition and data structures.
- 1/1 Plan for division of the work.

Document (12 points)

>Client (4 ):
>
> 1/1 User interface and Inputs and outputs
> 1/1 Functional decomposition into functions/modules
> 1/1 High-level pseudo code (plain English-like language) for logic/algorithmic flow
> 0/1 Testing plan, including unit tests, integration tests, system tests
    -1 the word "test" does not appear in the DESIGN.md Document
>
>Server (8):
>
> 1/1 User interface
> 1/1 Inputs and outputs
> 2/2 Functional decomposition into functions/modules
> 2/2 Major data structures
> 0/2 Testing plan, including unit tests, integration tests, system tests
    -2 the word "test" does not appear in the DESIGN.md Document

## (15) Implementation Spec

14/15 points:

    -1 leftover skeleton/outline text at top of  IMPLEMENTATION.md ("x,y,z")

- 1/1 module structure, cohesion, coupling
- 2/2 function prototypes (server)
- 3/3 function descriptions & pseudocode (server)
- 2/2 data structures (server)
- 2/2 function prototypes & pseudocode (client)
- 1/1 function descriptions (client)
- 2/2 data structures (client)
- 1/1 testing plan (server)
- 1/1 testing plan (client)

## (20) Code Style / Quality

16/20 points:

    -3 Not always checking for `malloc()` failure: `game.c:150,178,676`, `server.c:280`, `client.c:248,342`, `map.c:268`
    -1 `snprintf` is safer than `sprintf` in `server.c:150,154,158`, `client.c:140,251,254,262,264,344` 
    FYI `make clean` does not `rm client.log`

## (15) `git` Practices

13/15 points:

why no commits from carolinelschung ?

> 6/6 git commit history - logical groupings of changes in each commit, and meaningful commit messages.
> 4/6 git commit history - good use of branches and git-flow.
    -2 git flow and scrum weren't really used if you have only two merges. 
> 3/3 no executable files, object files, core dump files, editor backup files, or extraneous files existing in any directory.

## (35) Functionality  

31/35 points:

- 1/1 server commandline, per spec
    -1 does not detect excess parameters passed to `server`
- 6/6 server supports one player, per spec
- 6/6 server supports multiple players, per spec
- 1/1 server supports spectator, per spec
- 6/6 server supports 'visibility' spec
- 1/1 server tracks gold, per spec
- 1/1 server produces Game Over summary, per spec
- x/2 new, valid, non-trivial mapfile
    -2 fails `checkmap` AND when I use your map with your server any spectator that joins will never display the map 
- 0/1 client commandline, per spec
    -1 does not detect non-numeric port passed to `client`
- 4/4 client plays as player, per spec
- 4/4 client plays as spectator, per spec
- 1/1 client asks for window to grow, per spec
- 1/1 client prints Game Over summary, per spec

## Extra Credit

- 0 points for new options: "./server_module/server maps/main.txt --gold 240 --minpiles 5 --maxpiles 4" causes floating point exception and crash. Poor defensive programming!

- +3 for Gold stealing:  


### Gold Stealing (up to 3)

### Range Limit of Vision (up to 5)

### Alert Players (up to 5)
