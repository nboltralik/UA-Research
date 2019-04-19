**What is this repository?**
======

Read up on the **LZ Dark Matter Experiment** either [on Wikipedia](https://en.wikipedia.org/wiki/LZ_experiment) or [the official website](http://lz.lbl.gov/).

This program is designed to read a series of events and reconstruct the z-vertex, time, and probability under the "EE" and "NR" hypotheses.

## Usage
Download and install the ROOT Data Analysis Framework from their website [here](https://root.cern.ch/).
Make sure your C++ compiler is up to date.

With all the files above in your working directory, run the command:

`root -b -q -x 'minimize.cpp("10_events.ascii", "eventResults.root", .9)'`

using your event file name, the desired results file name, and quantum efficiency.

It will print the results of each Event under the two hypotheses and give you a TTree containing them.

#### Notice
Last updated 4/19/19.  If this README isn't among the latest commit then there's no guarantee the program will work as described above.

Currently works as described but without printing the results of each event
