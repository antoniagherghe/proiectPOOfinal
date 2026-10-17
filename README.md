# FarmSim — HayDay-Inspired Terminal Farming Game

A terminal farming game written in C++ for the **Object-Oriented Programming** course
(Faculty of Mathematics and Computer Science, University of Bucharest, 2026).
You run a small farm: buy plots, hire farmers to plant, water and harvest crops,
and keep your balance positive while crops grow, ripen and wilt.

```
|  .-------.     .-------.     .-------.   |
|  | w w w |     | - - - |     | # # # |   |
|  '-------'     '-------'     '-------'   |
|    Plot 1        Plot 2        Plot 3    |
```

## Features
- 6 farm plots: the first one is free, the others can be bought ($15) and sold ($10)
- Crops (wheat, potato, raspberry + your own custom plants) that grow, mature and wilt
- Two kinds of staff: **Junior** farmers (cheap) and **Senior** farmers, who also water the next plot for free
- Live ASCII interface with balance, crop age and `[READY]` / `[WILTED]` status
- Error handling for actions you can't afford or invalid plots

## OOP concepts used
| Concept | Where |
|---|---|
| Abstract class, pure virtual methods | `Plant` (`getType()`, `clone()`) |
| Inheritance and polymorphism | `Vegetable`/`Fruit` : `Plant`, `JuniorFarmer`/`SeniorFarmer` : `Farmer` |
| Virtual cloning | `Plant::clone()` when a crop is planted |
| Composition | `Player` owns a `vector<Plot>`, each `Plot` owns a `Plant*` |
| Custom exceptions | `InsufficientFundsException`, `InvalidPlotException` |
| Singleton | `GameStatManager` |
| Templates | `displayPlantFinancialReport<T>`, `isValueAffordable<T>` |
| Operator overloading | `<<`, `>>`, `==`, `=`, `+=`, `-=` |
| Static members, `const` members, friends | `Farmer::noFarmers`, `Farmer::id` |
| RTTI | `dynamic_cast<SeniorFarmer*>` in the staff menu |
| STL containers | `std::vector`, `std::list` |

The initial class design is in [`docs/class-design.xlsx`](docs/class-design.xlsx).

## Build and run
Requires a C++17 compiler on Linux or macOS.

```bash
make        # or: g++ -std=c++17 -Wall -o farm main.cpp
./farm      # or: ./run.sh
```

## Controls
`A` water · `B` plant · `C` harvest · `D` buy plot · `E` sell plot ·
`F` create a new plant type · `G` staff info and stats · `Q` quit
