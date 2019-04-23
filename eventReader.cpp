#include "getnLLK.cpp"
#include "header.h"
#include <iostream>
#include <string>

#define C 177.39 // mm/ns
#define BOTTOM_Z -149
#define TOP_Z 1540

/*
  This program takes a list of PMT hits (an event) and sums the negative log
  likelihoods of each hit to get the likelihood of the event which it returns
*/

Double_t calcDistance(Double_t, Double_t, Double_t,Double_t, Double_t, Double_t);

//Given a list of events, calculates the likelihood of each
Double_t eventReader(vector<vector<Double_t>> eventList, Double_t z, Double_t tEvent, Int_t hyp) {
  Double_t x,y,tPMT,tCorrected,distance,xPMT,yPMT,zPMT,eventLikelihood=0;
  Int_t PMT,hitCount;

  x = eventList[0][0];
  y = eventList[0][1];
  // printf("x = %.2f\ty = %.2f\n", x,y);

  hitCount = 0;

  //Do this once per hit
  points = getPoints(x,y,z);
  weights = calcWeights(points,x,y,z);

  //Iterate through the list of PMT hits
  for (size_t q = 1; q < eventList.size(); q++) {
    hitCount++;

    //TODO - the calculation of tCorrected should be either moved to a seperate
    //function either here or in getnLLK

    PMT = (Int_t)eventList[q][0];
    tPMT = eventList[q][1];
    // printf("PMT = %d\ttPMT = %.2f\n", PMT,tPMT);

    //Convert PMT numbering to continuous style and set z value
    if (PMT > 252) {
      PMT -= 47;
      zPMT = BOTTOM_Z;
      // printf("Subtracted 47 to get %d\n", PMT);
    } else {
      zPMT = TOP_Z;
    }
    //Get PMT# after rotation
    PMT = PMTMap[PMT][1];
    xPMT = PMTCoords[PMT][3];
    yPMT = PMTCoords[PMT][4];
    distance = calcDistance(x,y,z,xPMT,yPMT,zPMT);

    // printf("Distance between vertex: (%0.f,%0.f,%0.f) and PMT %d at (%0.f,%0.f,%0.f) is %0.2f\n", x,y,z,PMT,xPMT,yPMT,zPMT,distance);

    //Use speed of light to get time of flight then subtract t event to get t corrected
    tCorrected = tPMT - distance/C - tEvent;
    // printf("tCorrected = %f\n",tCorrected);

    //Pass t corrected and PMt to getnLLK() and sum the returns
    eventLikelihood += getnLLK(x,y,z,tCorrected,PMT,hyp);

  }

    //Normalize by dividing sum by number of hits, answer should be between 2 and 11
      eventLikelihood /= hitCount;
      // printf("%d hits\n", hitCount);
      // printf("eventLikelihood = %.4f\n", eventLikelihood);

  return eventLikelihood;
}

//Calculates the distance between two 3d points
Double_t calcDistance(Double_t x1, Double_t y1, Double_t z1, Double_t x2, Double_t y2, Double_t z2) {
  return pow((pow(x1-x2,2) + pow(y1-y2,2) + pow(z1-z2,2)), .5);
}
