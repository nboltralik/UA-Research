**What is this repository?**
======

Read up on the **LZ Dark Matter Experiment** either [on Wikipedia](https://en.wikipedia.org/wiki/LZ_experiment) or [the official website](http://lz.lbl.gov/).

This program is designed to read a series of events and reconstruct the z-vertex, time, and probability under the Electron Elastic and Nuclear Recoil hypotheses.

## Usage
Download and install the ROOT Data Analysis Framework from their website [here](https://root.cern.ch/).
Make sure your C++ compiler is up to date.

With all the files above in your working directory, run the command:

`root -b -q -x 'minimize.cpp("10_events.ascii", "eventResults.root", .9)'`

using the event file name, the desired results file name, and quantum efficiency.
A root file will be generated containing a TTree with the results.

## The files and their purpose
* header.h - holds common variables each file needs to access and
* minimize.cpp - Performs the minimization of z position and time
* eventReader.cpp - Returns the sum of the likelihoods of each PMT hit in a single event
* getnLLK - Returns the negative log likelihood of a single PMT being hit
* mapping.cpp - Calculates the closest PMT for each PMT after rotating them
* getPoints.cpp - Returns the closest lookup points to the event vertex
* calcWeights.cpp - Calculates the weights for each point

###### storeFits.cpp
This file is not used by the main program but exist to generate the EE_out.root and NR_out.root files.  It looks in a Data/Histograms folder for a root file corresponding to each lookup point and in Data/Params for a corresponding fit parameter file

#### Notice
Last updated 4/25/19.  If this README isn't among the latest commit then there's no guarantee the program will work exactly as described above.
