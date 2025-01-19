Project structure:
==================

## Base
All base includes

## World
Stores data about the world state

### class Ball
Data of individual ball

**InitialPosition**
Coordinates at last collision/user intervention

**CurrentPosition**
Actual current coordinates

**Radius**
Ball radius

**Mass**
Mass, used by Physics calculations

**Color**
Display color

**Draw()**
Calls respective Display functions

### vector<Ball> Balls
List of all balls

### NewBall(smth)
Create a new ball and add it to Balls


## Window
Draws things to the screen

### drawLoop()
Perpetually goes through all Balls, draws them, updates buffer, repeats. Returns when m_state != simulation

### displayScale
Scale at which everything is drawn to the screen

### drawCircle(smth)
Draw a circle on screen; main function for drawing balls

## [TODO]

### &balls
Reference to World::Balls

## Input
Anything related to getting input from the user

### InputLoop()
Perpetually goes through all inputs, handles them, and repeats. Can trigger ConsoleInput(). Returns when Main::AppState == QUIT.

### ConsoleInput()
Receive user input from console, RunCommand(ParseCommand([input]))

### ParseCommand(string)
Create Command object according to syntax: [Verb] [Semicolon-separated Parameters]

### CommandDictionary Dictionary<string, *void(vector<string> Parameters)>
Pre-made list of Verb-Function pairs used by RunCommand() to determine which function to run

### RunCommand(Command)
Uses CommandDictionary to determine which function to run, calls that funtion with Command.Parameters

### [huge list of functions corresponding to commands]
* TimeSet()
* TimeGet()
* TimeMove()
* BallNew()
* etc.

### class Command
Command info

**string Verb**
Primary command; "verb"

**vector<string> Parameters**
Input data for command

## Physics
All that relates to ball movement, collisions, etc.

### double Time
Time since last collision/user intervention in seconds

### double TouchingThreshold
Distance from radius at which two balls are considered to be touching

### &Balls
Reference to World::Balls

### UpdateBallList(&vector<Balls>)
Updates Balls to be a reference to provided input (World::Balls)

### PhysicsLoop()
Perpetually updates Time, goes through all Balls, calls UpdatePositions(), then ResolveCollisions(). Returns when Main::AppState == QUIT.

### UpdatePositions()
Goes through all Balls, sets CurrentPosition of each to InitialPosition + Velocity * Time

### ResolveCollisions()
Two balls are "colliding" if there is an overlap between their radii. They are "touching" if there is an overlap between their radii + TouchingThreshold.

1. Detect all touches and store them in vector<pair<Ball>> TouchPairList. If none are found and this is the first time step 1. is run, go to 7.
2. Detect all collisions and store them in vector<pair<Ball>> CollisionPairList.
3. If TouchPairList.size > 0 and CollisionPairList.size == 0, go to 7.
4. Modify Time
5. Call UpdatePositions()
6. Go to 1.
7. ApplyPushback(TouchPairList)

### ApplyPushback(vector<pair<Ball>> TouchingBalls)
Calculate and set appropriate velocities to push back the touching balls, taking into account impulse and angle

## Main
Handles program initialization, calls loops

### main()
World::UpdateBallLists();
thread Display::DisplayLoop();
thread Input::InputLoop();
thread Physics::PhysicsLoop();

## utils
Stores application info and helper functions; split into several files, represented as one for simplicity

### AppState
If RUNNING, app runs as normal. If QUIT, all ...Loop() functions return, app closes.

### namespace InOut

**In()**
Get console input

**Out()**
Output to console

Include Graph
=============
What is above includes what is below:

             Main
-------------^-------------
|            |            |
Physics      Input        Display
^            ^            ^
-------------|-------------
             World
	     	^
	     	utils
	     	^
	     	Base
