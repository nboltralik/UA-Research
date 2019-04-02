/*
  Reads through all of the histograms in the TFile. Uses a fit funcion
  to fit a line to them. Plots those on one side of a page.
  Plots the negative log of that line to the right of the first graph.
  Saves them to a pdf.
*/

//TODO - do i need these
#include <iostream>
#include <fstream>
#include <cstdio>

#define NUM_PMT 494
#define PARAM_FILE "NR_r66_z1440.par"

//Functions
Double_t** readParams();
Double_t f(Double_t*, Double_t*);

//Variables
TCanvas *c;

void fitting() {

  //Load the histograms
  TFile *inFile = new TFile ("NR_r66_z1440_tc.root");

  //Blue unfitted histograms from the TFile
  TH1D *hr[NUM_PMT];
  //Holds the histogram from gpTail2 used to store
  TH1D *hf[NUM_PMT];
  //Negative log function on the right
  TH1D *hg[NUM_PMT];

  //Fill the hr array
  char* name;
  for (Int_t i = 0; i < NUM_PMT; i++) {
    sprintf(name, "h%d", i);
    // cout << name << endl;
    hr[i] = (TH1D*) inFile->Get(name);
    hr[i]->Scale(1.0/hr[i]->GetEntries());
  }

  Double_t** fitPars = readParams();

  TF1 *gpTail2 = new TF1("gpTail2",f,-50,550,7);
  gpTail2->SetNpx(600);

  //Main Loop
  for (Int_t i = 0; i < NUM_PMT; i++) {

    if (i%3 == 0) {
      //Close the canvas if it exists to supress warnings
      if (c) { c->Close(); gSystem->ProcessEvents(); }
      c = new TCanvas ("c","Name Me", 600, 800);
      c->Divide(2,3);
    }
    Int_t cTmp = (i%3)*2+1;


    c->cd(cTmp);
    gpTail2->SetParameters(fitPars[i]);
    gpTail2->Draw();
    hr[i]->Draw("same hist");

    Double_t x,fv,sum;

    sprintf(name, "Fit%d", i+1);
    delete gROOT->FindObject(name);
    hg[i] = new TH1D(name,name,600,-50,550);
    hf[i] = (TH1D*)gpTail2->GetHistogram();
    for (Int_t j=1; j<=600; j++) {
      x = hf[i]->GetBinContent(j);
      x = -TMath::Log(x);
      hg[i]->SetBinContent(j,x);
    }

    c->cd(cTmp+1);
    hg[i]->Draw();

    // if (hg[i]->GetBinContent(450) > 16) {
    //   printf("%d\n", 1);
    // } else {
    //   printf("%d\n", 0);
    // }


    // if (i == 2) {
    //   c->Print("Histograms.pdf(", "pdf");
    // } else if (i+1 == NUM_PMT) {
    //   c->Print("Histograms.pdf)", "pdf");
    // } else if (i%3 == 2) {
    //   c->Print("Histograms.pdf", "pdf");
    // }

  }
}

Double_t f(Double_t *x, Double_t *params) {
  Double_t retVal =
  sqrt(3.14/2.0) * (params[0]*params[2]/(params[4]*params[5]))
  * exp(-x[0]*(1/params[4]+1/params[5])) * (params[3]
  * exp(params[2]*params[2]/(2*params[4]*params[4]) + params[1]/params[4]+x[0]/params[5])
  * params[5] * TMath::Erfc((params[2]*params[2]
  + (params[1]-x[0]) * params[4])/(sqrt(2.0)*params[2]*params[4]))
  + exp((params[2]*params[2]+2*params[1]*params[5])/(2*params[5]*params[5])+x[0]/params[4])
  * params[4]
  * TMath::Erfc((params[2]*params[2]+(params[1]-x[0])*params[5])/(sqrt(2.0)*params[2]*params[5])))
  + params[6];

  return retVal;
}

Double_t** readParams() {

  Double_t tmp;

  Double_t **fitPars = new Double_t*[NUM_PMT];
  for (size_t i = 0; i < NUM_PMT; i++) {
    fitPars[i] = new Double_t[7];
  }

  //Open file
  ifstream datfile;

  datfile.open(PARAM_FILE);
  if (!(datfile.is_open())) {
    cerr << "File " << PARAM_FILE << " failed to open" << endl;
    return NULL;
  }

  // printf("looping here\n");
  for (int i = 0; i < NUM_PMT; i++) {
    if (datfile.eof()) {
      cerr << "Unexpected End-of-File" << endl;
      // return NULL;
    }
    datfile >> tmp;
    for (int j = 0; j < 7; j++) {
      datfile >> fitPars[i][j];
    }
    datfile >> tmp;
    // printf("%d\n", i);
  }

  // printf("Ending loop\n");

  // for (int i = 0; i < NUM_PMT; i++) {
  //   printf("%d: %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f\n", i,
  //   fitPars[i][0],
  //   fitPars[i][1],
  //   fitPars[i][2],
  //   fitPars[i][3],
  //   fitPars[i][4],
  //   fitPars[i][5],
  //   fitPars[i][6]);
  // }

  datfile.close();
  return fitPars;
}
