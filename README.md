# EilmerLoadCalc

Simple calc that reads in all the .dat files exported from Eilmer CFD program and calculates the loads on a body.
Currently under development.



## Compile instructions
* windows: g++ -Wall -o calc_loads.exe -std=c++17 LoadCalc.cpp
* linux  : g++ -Wall -o calc_loads.exe -std=c++17 LoadCalc.cpp -lstdc++fs
 
## Examples - note directory structure.

windows: 
* calc_loads Dir:H:\CFD\sandpit2\loads\t0170\
* calc_loads Dir:H:\CFD\sandpit2\loads\t0170\ File:_output file name_
* calc_loads Dir:H:\CFD\sandpit2\loads\t0170\ File:_output file name_ Type:linear
          
linux  :
* ./calc_loads Dir:/home/aaron/CFD/sandpit2/loads/t0170/

## Testing
Currently tested:
* Load calc on axisymmetric body.  

Remaining to be tested:
* Load calc on linear body.
* Load calc on 3D body.

Under development:

Calculate the coefficient of drag - notes - calculates area on axisymmetric body properly and Cd aligns with excel checks.
TODO - density and speed are currently hard coded - need to add these to the args or parse some other file to extract them.
      
      




