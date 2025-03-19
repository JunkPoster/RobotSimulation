# Robot Simulation Project
**By Ian Featherston** <br /> 
For CISP400 <br />
Originally Written: May, 2024 <br />
Revisited for File Splitting: March, 2025

This was my final project for CISP400 (Object-Oriented Programming in C++). I'm quite proud of the end result, and I learned so much when programming it. I only get 2 warnings for it in Visual Studio, and that's regarding the Macros I use for logging. Everything you see in this project, I wrote (we also weren't allowed to use third party libraries). I'm sure there are better ways to write a LOT of this, but this is the work of a somewhat novice student.

## Special Features
- Estimated runtime of the simulation
- Plotted results after the simulation completes
- Text formatting
    - I wrote a `FormatText.h` class that utilizes a few other structs I wrote to format text in a nice way.
        - `AlignText()`: A struct I wrote to calculate the spacing within a given length and a given string, and aligns it either to the Left, Center, or Right, based on the passed arguments.
        - `WrapText()`: A struct that utilizes `AlignText()` in order to word-wrap text based on parameters. This was especially useful when writing text within a table/confined container.
        - `StatusBar()`: This prints a colorized status/progress bar. It could be a bit more modular, but it worked for this program.

**You can find the prompt that we were given by our professor at the end of this README**.

Here are some screenshots from the console when running the program:
**Main Menu**:
![image](https://github.com/user-attachments/assets/2517c090-3fad-46df-b1f0-5c8fe9751280)

**Runtime Estimation**
![WindowsTerminal_qXjmwjqhFs](https://github.com/user-attachments/assets/4c1fcb16-34eb-46a7-b3d8-ab466adf7cd2)

**Plotted Results**
![image](https://github.com/user-attachments/assets/614b0672-01b6-4a3a-97cd-e11941ee73e5)

**Generational Stats**
![image](https://github.com/user-attachments/assets/ef26be12-65dd-434d-bd65-deb8bf12f600)


# Project Prompt
**Note**: We were also required to submit all of this code in ONE file, but I came back and separated each module into their own respective files as best as I could. 

## Problem.
You are going to reproduce an experiment first conducted at Harvard
University in 1968. This assignment is a bit different from the other
homework assignments in this class because there are no specifica-
tion bundles for this. You either make it work or you don’t. You are
going to study the effects of evolution on a population of robots. The
robots need to maneuver around a grid collecting energy. The robots
must collect more energy than they expend to survive. Your robots
will maneuver around a 10 x 10 grid looking for batteries. A battery
gives the robot five more power. Moving a square costs 1 power. The
sensors are always on and cost no power. Robots have five power
when they first emerge on the map.

The key is the robot behavior. Each robot has a collection of di-
rection sensors and a motor. Robot success depends upon the map
between these two elements. We do not hard code the map between
the sensor data and the motor actions, however. The robots start with
a random mapping, but over time, the mappings evolve into success-
ful strategies. We’ll get to how they do this in a minute.
Figure 1: Example of sensor readings
with various obstacles on the map.

## Robot Sensors.
The robot has four sensors - each facing in a different direction. This
way, the robot can detect what is in the squares adjacent to the North,
South, East, and West. Each map feature will generate a different
code for that sensor. See figure 1 for four examples. The codes you
see here are examples, you can use whatever codes you wish.
Robot sensor states:
• No object in square.
• Wall object.
• Battery object.
• Don’t care if anything is there.

## Robot Genes.
Each robot will have 16 genes. Each gene is an array containing
five codes. See figure 2 of a single gene. The first four codes corre-
spond to possible sensor states. The last code instructs the robot what
to do in the event the current sensor state matches the four states on
the gene.
genetic algorithm robots 2
Robot actions:
• Move 1 north.
• Move 1 south.
• Move 1 east.
• Move 1 west.
• Move 1 random direction.

This means that every turn the robot is comparing the current
sensor state with it’s genetic code in an attempt to find a match. A
gene must match exactly for it’s behavior to be executed. If it does
not find an exact match, the robot will execute the last gene (number
16). Robots will continue to do this until they run out of energy.
**KEEP TRACK OF THE NUMBER OF TURNS A ROBOT SURVIVES.**
This will become very important later. A robot gains energy running
into a square containing a battery. The battery is consumed in the
process. Figure 3 is an example of 1 robot turn.
Figure 3: Example of a robot moving
along the map for 1 turn.

## The Map.
Randomly place each robot on a spot on the map. Populate 40% of
the squares with batteries. Generate a new map for each robot. We
run one robot through at a time. Moving each square consumes 1
energy unit - even if it’s not a valid move (a wall). Invalid moves
consume energy, but keep the robot on the original square. This is
usually the death knell for that robot unless the move action was
move in a random direction.

You don’t need to display the map on the console. It’s interesting
to watch if you want to do so, however. Perhaps display the map of
the best performing robot or the worst performing one.

## Robot Population.
Evolution works on populations, not individuals. You need a pop-
ulation of robots to run through your maps. Create a population of
200 randomly generated robots to start. That is, you randomly gener-
ate the mappings between allowable behaviors and sensor codes for
the first generate of robots only. Each robot is run through a random
map and the number of turns they survive is recorded.
Robot Reproduction.

Once all the robots in a population have had a turn (and acquired
energy scores), record the total energy harvested by the entire gen-
eration and breed your robots. Sort your population by energy har-
vested and kill off the lowest 50%. Then pair up the top 2 robots and
genetic algorithm robots 3 produce 2 children by combining genes from 
both parents. The children enter the gene pool with the parents in the 
next round. Then, breed the 3rd and 4th highest scoring robots. Repeat 
until all the parent robots have reproduced. Keep the number of robots 
at a fixed number for the entire simulation. Genes are randomly created 
for the first population only - the robots breed after that.

Each parent supplies 50% of the 16 genes for a child. The simplest
way is for one parent to supply the first 8 and the other to supply the
last 8. You will not want the siblings to have the same genes (unless
you are investigating the effects of identical twins). However, you can
use a different swap scheme if you would like.

Swapping genes is a tricky business and mistakes happen. In
5% of the individual genes swapping there is an error - a mutation.
Randomly change one character on the gene the child has inherited
from it’s parent. Just generate another value and insert that value
over the old one. You can also shift all genes down 1 and the 16th
gene moves to the top.

## Genetic Algorithms.
The degree to which the robot successfully harvests energy from the
environment is called ’fitness’. We measure that by the total amount
of power harvested when each individual robot’s time ends. When
we finish with the entire population of 200, we calculate an average
fitness score for the population. Save the average fitness for each gen-
eration. You will most likely see slow and steady improvement over
time - evolution at work. When the simulation completes, print out
the average fitness scores on the console. This is even more effective
if you are able to draw a console graph (not a requirement).

The fascinating thing about this experiment is your robots will get
better and better at navigating the map over time. They will evolve
strategies dealing with walls and corners. The most exciting part is
it doesn’t require any further participation from you - just set it in
motion and watch it go.

## Bonus Features.
No bonus features are needed with this assignment. However, past
students have:
• Added obstacles for the robots to avoid.
• Added predator robots.
• Added vision so the robots can ’see’ 2 spaces away.
• Added memory of moves.
• Plotted fitness over time on the console (a common modification).
genetic algorithm robots 4
• Created Don Juan robots that have children with several partners.
• Stored many of the constraints as constants so they could explore
the effects of modifying them (ex: change the mutation rate to 8%
from 5%) - another common modification.
• Keep track of how many generations a specific robot survived.
