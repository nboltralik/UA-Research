#include "mapping.cpp"
#include "getPoints.cpp"
#include "calcWeights.cpp"
#include "header.h"

/*
  This program calculates the negative log likelihood (nLLK) of a photon
  originating from the given x,y,z coords hitting the specified PMT at tCorrected
  under the hypothesis specified (0=EE, 1=NR)
*/

// static bool calledOnce = false;

Double_t getnLLK(Double_t, Double_t, Double_t, Double_t, Int_t, Int_t);
void readHistograms();

//TODO - need better name for F[] array
//TODO - fix findNearestPoints in getPoints.cpp so that is directly handles and
  //modifies a Point itself so that convertPointToIndex() isn't needed

Double_t getnLLK(Double_t x, Double_t y, Double_t z, Double_t tCorrected, Int_t PMTNum, Int_t hyp) {

  // printf("getnLLK called with PMT = %d, hyp = %d\n", PMTNum,hyp);

  //If tCorrected not between -50 and 550, correct
  if (tCorrected < -50) {
    tCorrected = -50;
  } else if (tCorrected > 549) {
    tCorrected = 549;
  }
  //If PMT not between 0 and 252 or 300 and 540, throw error
  // if (!((PMTNum >= 0 && PMTNum <= 252) || (PMTNum >=300 && PMTNum <= 540))) {
  //   printf("PMTNum %d not in proper range\n", PMTNum);
  // }

//Takes official PMT and converts it to continuous in order to use as an array index
  // if (PMTNum > 252) {
  //   PMTNum -= 47;
  // }
  // cout << "PMT " << PMTNum << " -> ";
  // PMTNum = PMTMap[PMTNum][1];
  // cout << PMTNum << endl;

  TH1F *h;
  Int_t yIndex, zIndex, bin1;
  Double_t F[4], nLLK=0, y1, y2;

  //For each pair, get the histogram corresponding to that point
  for (size_t i = 0; i < 4; i++) {
    yIndex = points[i]->yIndex;
    zIndex = points[i]->zIndex;
    // printf("Getting h[%d][%d][%d][%d]\n", PMTNum, zIndex, yIndex, hyp);
    h = histogram[PMTNum][zIndex][yIndex][hyp];

    // Do a 2 point linear interpolation
    bin1 = floor(tCorrected+51);
    // printf("floor = %d\n", bin1);
    y1 = h->GetBinContent(bin1);
    y2 = h->GetBinContent(bin1+1);

    F[i] = y1 + (tCorrected+51-bin1)*(y2-y1);

    // printf("bin1 = %d\ny1 = %.5f\ny2 = %.5f\n", bin1,y1,y2);
    // printf("Content at %.2fns is %.9f\n", tCorrected, F[i]);
  }

  //Calculate nLLK using weights and histogram contents
  for (size_t i = 0; i < 4; i++) {
    nLLK += F[i] * weights[i];
  }

  // printf("At (%0.f,%0.f,%0.f) PMT=%d\n", x,y,z,PMTNum);
  // printf("nLLK = %f\n", nLLK);
  return nLLK;
}

//Reads all of the histograms into memory
void readHistograms() {

  TFile *infile = new TFile(EE_HISTO_FILE);
  TString f;
  string g;

  for (int i = 0; i < NUM_PMT; i++) {
    for (size_t j = 0; j < NUM_Z; j++) {
      for (size_t k = 0; k < NUM_Y; k++) {
        g = type[0] + yVals[k] + zVals[j];
        // printf("%s %d\n", g.c_str(),i);
        // printf("Reading h[%d][%d][%d][0]\n", i, j, k);
        histogram[i][j][k][0] = (TH1F*)infile->Get(TString::Format("%s %d", g.c_str(), i));
        // cout << "Bin Content: " << histogram[i][j][k][0]->GetBinContent(50) << endl;
      }
    }
  }
  // infile->Close();

  infile = new TFile(NR_HISTO_FILE);
  for (int i = 0; i < NUM_PMT; i++) {
    for (size_t j = 0; j < NUM_Z; j++) {
      for (size_t k = 0; k < NUM_Y; k++) {
        // printf("Reading h[%d][%d][%d][1]\n", i, j, k);
        g = type[1] + yVals[k] + zVals[j];
        histogram[i][j][k][1] = (TH1F*)infile->Get(TString::Format("%s %d", g.c_str(), i));
      }
    }
  }
  // infile->Close();
}
