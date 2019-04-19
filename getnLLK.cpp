#include "mapping.cpp"
#include "getPoints.cpp"
#include "calcWeights.cpp"
#include "header.h"

static bool calledOnce = false;

Double_t getnLLK(Double_t, Double_t, Double_t, Double_t, Int_t, Int_t);
void setup(Double_t, Double_t, Double_t);
void readHistograms();

//TODO - need better name for F[] array
//TODO - fix findNearestPoints in getPoints.cpp so that is directly handles and
  //modifies a Point itself so that convertPointToIndex() isn't needed

Double_t getnLLK(Double_t x, Double_t y, Double_t z, Double_t tEvent, Int_t PMTNum, Int_t hyp) {

  // printf("getnLLK called with PMT = %d, hyp = %d\n", PMTNum,hyp);

  //If tEvent not between -50 and 550, correct
  if (tEvent < -50) {
    tEvent = -50;
  } else if (tEvent > 549) {
    tEvent = 549;
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
    // h->Draw();

    // Do a 2 point linear interpolation
    bin1 = floor(tEvent+51);
    // printf("floor = %d\n", bin1);
    // Double_t y1 = TMath::Exp(-h->GetBinContent(bin1));
    // Double_t y2 = TMath::Exp(-h->GetBinContent(bin1+1));
    y1 = h->GetBinContent(bin1);
    y2 = h->GetBinContent(bin1+1);

    F[i] = y1 + (tEvent+51-bin1)*(y2-y1);

    // TH1D *hh=new TH1D ("hh","taux",600,-50,550);
    // for (Int_t jj=1; jj<600; jj++) hh->SetBinContent(jj,TMath::Exp(-h->GetBinContent(jj-49.5)));

    // printf("bin1 = %d\ny1 = %.5f\ny2 = %.5f\n", bin1,y1,y2);
    // printf("Content at %.2fns is %.9f\n", tEvent, F[i]);
  }

  //Calculate nLLK using weights and histogram contents
  for (size_t i = 0; i < 4; i++) {
    nLLK += F[i] * weights[i];
  }

  // printf("At (%0.f,%0.f,%0.f) PMT=%d\n", x,y,z,PMTNum);
  // printf("nLLK = %f\n", nLLK);
  return nLLK;
}

void setup(Double_t x, Double_t y, Double_t z) {
  // printf("setup called at (%0.f,%0.f,%0.f)\n", x,y,z);

  // evtClock->Start();
  //Only do this once
  // if (calledOnce == false) {
  //   //Read all of the PMT coords into memory
  //   PMTCoords = readGeometryFile();           //From mapping.cpp
  //   // for (size_t i = 0; i < NUM_PMT; i++) {
  //   //   printf("PMTCoords[%zu][0] = %0.f\n", i,PMTCoords[i][0]);
  //   // }
  //   calledOnce = true;
  //   // readHistograms();
  // }
  // evtClock->Stop();
  // timeElapsed += evtClock->CpuTime();
  // evtClock->Reset();

  // evtClock->Start();
  //Create the mapping
  // PMTMap = mapping(x,y);                  //From mapping.cpp
  // for (size_t i = 0; i < NUM_PMT; i++) {
  //   cout << PMTMap[i][0] << " -> " << PMTMap[i][1] << endl;
  // }
  // evtClock->Stop();
  // timeElapsed += evtClock->CpuTime();
  // evtClock->Reset();

  // evtClock->Start();
  //Find closest 4 pairs of (y,z) points
  points = getPoints(x,y,z);
  // cout << "Closest Points:" << endl;
  // for (size_t i = 0; i < 4; i++) {
  //   cout << "(" << points[i]->y << ", " << points[i]->z << ")" << endl;
  // }
  // evtClock->Stop();
  // timeElapsed += evtClock->CpuTime();
  // evtClock->Reset();

  // evtClock->Start();
  weights = calcWeights(points,x,y,z);
  // cout << "Weights:" << endl;
  // for (size_t i = 0; i < 4; i++) {
  //   cout << weights[i] << endl;
  // }
  // evtClock->Stop();
  // timeElapsed += evtClock->CpuTime();
  // evtClock->Reset();

}
//Reads all of the histograms into memory
void readHistograms() {

  TFile *infile = new TFile("EE_out.root");
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
  // histogram[241][4][1][0] = (TH1F*)infile->Get("EE_r22_z0968 241");
  // cout << "Bin Content: " << histogram[241][4][1][0]->GetBinContent(50) << endl;

  infile = new TFile("NR_out.root");
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

// TH1D *h1 = new TH1D ("h1","test histogram",600,-50,550);
// Double_t taux=-50.5;
// // for (Int_t i = 600; i >= 1; i--) {
// for (Int_t i = 1; i <= 600; i++) {
//
//   h1->SetBinContent(i, getnLLK(x, y, z, taux, PMTNum));
//   taux += 1.0;
// }
// h1->Draw();

// TH1D *h1 = new TH1D ("h1","test histogram",6200,-60,560);
// Double_t taux=-50.5;
// // for (Int_t i = 600; i >= 1; i--) {
// for (Int_t i = 1; i <= 6200; i++) {
//
//   h1->SetBinContent(i, getnLLK(x, y, z, taux, PMTNum));
//   taux += 0.1;
// }
// h1->Draw();


// Double_t nLLK = getnLLK(x, y, z, tEvent, PMTNum);
// cout << nLLK << endl;
