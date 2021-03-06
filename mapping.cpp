#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include "header.h"

#define NUM_TOP_PMT 253
#define PI 3.141592653589793

/*
  This program creates a 2D Int_t array containing a mapping from PMT numbers
  to their closest PMT after a rotation of theta degrees
  Currently accounts for 97% of the time spent executing the whole entire program
*/

//TODO
//Why does theta need to be subtracted for the bottom PMTs?
//Simplify this so that it doesn't take so long

//Reads the file into a Double_t[494][5] array containing PMT Number, R, ANG, x, y
Double_t** readGeometryFile();
//Returns a mapping of each PMT to the closest PMT after rotating
Int_t** createMapping(Double_t);
//Calculates the distance between two points given in polar coords
Double_t polarDistance(Double_t, Double_t, Double_t, Double_t);
//Loops through the PMTArray to find the closest PMT to the given polar coords
Int_t findClosest(Double_t, Double_t, Int_t);

//Main function
Int_t** mapping(Double_t x, Double_t y) {

  mappingCount++;

  // Double_t **PMTArray = readGeometryFile();
  Double_t rho = pow((x*x + y*y), 0.5);
  Double_t theta = acos(y/rho) * 180/PI;
  //If x > 0 then we need a counter-clockwise rotation
  if (x > 0) {
    theta *= -1;
  }
  // cout << "Rho = " << rho << endl << "Theta = " << theta << endl;

  // for (size_t i = 0; i < NUM_PMT; i++) {
  //   printf("%zu: %.2f, %.2f, %.2f\n", i, PMTArray[i][0], PMTArray[i][1], PMTArray[i][2]);
  // }

  Int_t** map = createMapping(theta);

  // for (size_t i = 0; i < NUM_PMT; i++) {
  //   printf("%d\t->\t%d\n", map[i][0], map[i][1]);
  // }

  // printf("Distance is %f\n", polarDistance(2, 45, 3, 135));

  return map;

}

//Loops through every PMT, calling findClosest to get its closest PMT after rotation
Int_t** createMapping(Double_t theta) {

  //Alocate memory for a 2D Int_t array
  Int_t **map = new Int_t*[NUM_PMT];
  for (size_t i = 0; i < NUM_PMT; i++) {
    map[i] = new Int_t[2];
  }

  // evtClock->Start();
  //Loop through the PMTs
  for (Int_t i = 0; i < NUM_PMT; i++) {
    //First column of map is original PMT number
    map[i][0] = PMTCoords[i][0];
    //Second column is new PMT number after rotation.
    if (i < NUM_TOP_PMT) {
      //Pass 0 as pos if the PMT is in the top array
      map[i][1] = findClosest(PMTCoords[i][1], PMTCoords[i][2]+theta, 0);
    } else {
      //Else pass 1 for bottom PMTs
      map[i][1] = findClosest(PMTCoords[i][1], PMTCoords[i][2]-theta, 1);
    }
  }
  // evtClock->Stop();
  // timeElapsed += evtClock->CpuTime();
  // evtClock->Reset();

  return map;
}

//Given the array of PMTs, a given polar coordinate, and an Int_t indicating a top
//or bottom PMT, returns the PMT number of the closest PMT
Int_t findClosest(Double_t rho, Double_t theta, Int_t pos) {

  Double_t dist, min;
  Int_t PMT;

  //If the coords for the given PMT are on top then:
  if (pos == 0) {
    //Set minimum distance to first PMT in list (PMT 0)
    min = polarDistance(PMTCoords[0][1], PMTCoords[0][2], rho, theta);
    PMT = 0;

    //Loop through all top PMTs
    for (size_t i = 1; i < NUM_TOP_PMT; i++) {
      //Calculate to distance between the two PMTs
      dist = polarDistance(PMTCoords[i][1], PMTCoords[i][2], rho, theta);
      // printf("Distance between PMT# %.0f at %.2f, %.2f and %.2f, %.2f is: %.2f\n", PMTCoords[i][0], PMTCoords[i][1], PMTCoords[i][2], rho, theta, dist);

      //If this is less than the previous minimum distance then change the minimum
      if (dist < min) {
        PMT = (Int_t)PMTCoords[i][0];
        min = dist;
      }
    }
    return PMT;

  //Same steps as above but for bottom PMTs (#'s 300-540)
  } else {
    min = polarDistance(PMTCoords[NUM_TOP_PMT][1], PMTCoords[NUM_TOP_PMT][2], rho, theta);
    PMT = 300;
    for (size_t i = NUM_TOP_PMT; i < NUM_PMT; i++) {
      dist = polarDistance(PMTCoords[i][1], PMTCoords[i][2], rho, theta);
      // printf("Distance between PMT# %.0f at %.2f, %.2f and %.2f, %.2f is: %.2f\n", PMTCoords[i][0], PMTCoords[i][1], PMTCoords[i][2], rho, theta, dist);
      if (dist < min) {
        PMT = (Int_t)PMTCoords[i][0];
        min = dist;
      }
    }
    return PMT;
  }
}

//Takes two polar coordinates and returns the distance between them
Double_t polarDistance(Double_t rho1, Double_t theta1, Double_t rho2, Double_t theta2) {
  Double_t dist = (rho1*rho1) + (rho2*rho2) - 2*rho1*rho2*cos((theta1-theta2)*PI/180);
  // printf("cos is: %f\n", cos(theta1-theta2));
  return pow(dist, 0.5);
}

//Reads the LZ geometry file into a 2D array of doubles
Double_t** readGeometryFile() {

  //Allocate space for 2D array of doubles.
  //NUM_PMT rows and 3 columns for PMT #, rho, and angle respectively
  Double_t **PMTArray = new Double_t*[NUM_PMT];
  for (size_t i = 0; i < NUM_PMT; i++) {
    PMTArray[i] = new Double_t[5];
  }

  //Open file
  ifstream infile;
  infile.open(GEOMETRY_FILE);
  if (!(infile.is_open())) {
    cerr << "File " << GEOMETRY_FILE << " failed to open" << endl;
    return NULL;
  }

  Double_t num, x, y, rho, ang;

  for (Int_t i = 0; i < NUM_PMT; i++) {
    if (infile.eof()) {
      cerr << "Unexpected End-of-File" << endl;
    }
    infile >> num;
    infile >> x;
    infile >> y;
    infile >> rho;
    infile >> ang;
    if (num > 252) {
      num -= 47;
    }
    PMTArray[i][0] = num;
    PMTArray[i][1] = rho;
    PMTArray[i][2] = ang;
    PMTArray[i][3] = x;
    PMTArray[i][4] = y;
    // printf("%d: %.1f, %.1f, %.1f\n", i, num, rho, ang);
  }

  // for (size_t i = 0; i < NUM_PMT; i++) {
  //   printf("%zu: %.2f, %.2f, %.2f\n", i, PMTArray[i][0], PMTArray[i][1], PMTArray[i][2]);
  // }
  infile.close();
  return  PMTArray;
}
