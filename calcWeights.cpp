#include "header.h"

/*
  Calculates the weights.  weights[0] corresponds to w11, weights[1] to w12
  and so on.
*/
Double_t* calcWeights(Point** points, Double_t x, Double_t y, Double_t z) {
  Double_t rho = pow((x*x + y*y), 0.5);
  // cout << "rho = " << rho << endl;

  Double_t* weights = new Double_t[4];
  Double_t denominator = (points[2]->y-points[0]->y) * (points[1]->z-points[0]->z);

  weights[0] = (points[2]->y-rho)*(points[1]->z-z)/denominator;
  weights[1] = (points[2]->y-rho)*(z-points[0]->z)/denominator;
  weights[2] = (rho-points[0]->y)*(points[1]->z-z)/denominator;
  weights[3] = (rho-points[0]->y)*(z-points[0]->z)/denominator;

  return weights;
}
