# EilmerLoadCalc

Simple calc that reads in all the .dat files exported from Eilmer CFD program and calculates the loads on a body.
Currently under development.
outputs loads to a txt file and console

## Compile instructions
* windows: g++ -Wall -o LoadCalc.exe -std=c++17 LoadCalc.cpp
* linux  : g++ -Wall -o LoadCalc -std=c++17 LoadCalc.cpp -lstdc++fs
 
## Examples - note directory structure.

for detailed help:
  $LoadCalc help

windows: 
* LoadCalc dir:H:\CFD\sandpit2\loads\t0170\
* LoadCalc dir:H:\CFD\sandpit2\loads\t0170\ file:_output file name_
* LoadCalc dir:H:\CFD\sandpit2\loads\t0170\ file:_output file name_ type:linear
* LoadCalc dir:H:\CFD\sandpit2\loads\t0170\ rho:3.72e-5 speed:9000
          
linux  :
* ./LoadCalc dir:/home/aaron/CFD/sandpit2/loads/t0170/

Don't forget to chmod +x LoadCalc

## Testing
Currently tested:
* Load calc on axisymmetric body.  
* Coefficient of drag on axisymmetric body.  

Remaining to be tested:
* Load calc on linear body.
* Load calc on 3D body.

Under development:

Calculate the coefficient of drag - notes - calculates area on axisymmetric body properly and Cd aligns with excel checks.

      
      




