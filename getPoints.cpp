#include "header.h"

Double_t* findNearestPoints(Double_t, Double_t[], Int_t);
Int_t convertPointToIndex(Double_t);

/*
  Main function, takes the coordinates of an event and returns the weights.
*/
Point** getPoints(Double_t x, Double_t y, Double_t z){

  Double_t rho = pow((x*x + y*y), 0.5);

  Double_t *arr = findNearestPoints(rho, yPoints, NUM_Y);
  Double_t *arr2 = findNearestPoints(z, zPoints, NUM_Z);

  // printf("y = %.1f, %.1f\n", arr[0], arr[1]);
  // printf("z = %.1f, %.1f\n", arr2[0], arr2[1]);

  Point **points = new Point*[4];
  for (size_t i = 0; i < 4; i++) {
    points[i] = new Point;
  }

  for (size_t i = 0; i < 4; i++) {
    points[i]->y = arr[i/2];
    points[i]->z = arr2[i%2];
  }

  for (size_t i = 0; i < 4; i++) {
    points[i]->yIndex = convertPointToIndex(points[i]->y);
    points[i]->zIndex = convertPointToIndex(points[i]->z);
  }

  // for (size_t i = 0; i < 4; i++) {
  //   printf("Point (%d, %d)\n", points[i]->y, points[i]->z);
  // }

  return points;

}

/*
  Returns an array of two Double_t representing the two closest values to pos.
  arr is the array of points, length is the length of said array
*/
Double_t* findNearestPoints(Double_t pos, Double_t arr[], Int_t length) {

  Int_t i=0;
  Double_t *returnVal = new Double_t[2];
  returnVal[0] = -1;
  returnVal[1] = -1;

  // if (pos > arr[length-1]) {
  //   returnVal[0] = length-1;
  //   returnVal[1] = length-2;
  //   return returnVal;
  // }
  //
  // while (i<length && pos > arr[i]) {
  //   // printf("i=%d arr[i]=%.1f\n", i, arr[i]);
  //   i++;
  // }
  //
  // if (i == 0) {
  //   returnVal[0] = i;
  //   returnVal[1] = i+1;
  // } else {
  //   returnVal[0] = i-1;
  //   returnVal[1] = i;
  // }

  if (pos > arr[length-1]) {
    returnVal[0] = arr[length-1];
    returnVal[1] = arr[length-2];
    return returnVal;
  }

  while (i<length && pos > arr[i]) {
    // printf("i=%d arr[i]=%.1f\n", i, arr[i]);
    i++;
  }

  if (i == 0) {
    returnVal[0] = arr[i];
    returnVal[1] = arr[i+1];
  } else {
    returnVal[0] = arr[i-1];
    returnVal[1] = arr[i];
  }

  return returnVal;
}

//Converts a Double_t representing either a y or z point, converts it to its index
//in the array of y/z points.  Points need to be converted so that
Int_t convertPointToIndex(Double_t p) {

  for (Int_t i = 0; i < NUM_Y; i++) {
    if (p == yPoints[i]) {
      return i;
    }
  }
  for (Int_t i = 0; i < NUM_Z; i++) {
    if (p == zPoints[i]) {
      return i;
    }
  }

  return -1;
}
