/*
  Depending on eventType, either loop through all EE or NR files.
  For each one, load it and its respective param file.
  For every PMT, use the fit params and the gpTail2 function to
  create a fit.  Store the negative log of that fit in the histogram
  array.
*/

Double_t** readParams(string);
Double_t func(Double_t*, Double_t*);
void fitting(string);

#define NUM_PMT 494
#define NUM_Z 7
#define NUM_R 4
#define NUM_BIN 600

TH1F *h[NUM_R][NUM_Z][NUM_PMT];
int rIndex = 0, zIndex = 0, pmtIndex = 0;

string rVals[] = {"r00_", "r22_", "r44_", "r66_"};
string zVals[] = {"z0001", "z0242", "z0484", "z0726", "z0968", "z1210", "z1440"};
string type[] = {"EE_", "NR_"};

void storeFits(int eventType) {

  string f;
  for (rIndex = 0; rIndex < NUM_R; rIndex++) {
    for (zIndex = 0; zIndex < NUM_Z; zIndex++) {
      f = type[eventType] + rVals[rIndex] + zVals[zIndex];
      // cout << f << endl;
      fitting(f);
    }
  }

  // fitting(f);
  // readParams("EE_r22_z0242");
  // TCanvas *c = new TCanvas("c","",600,800);
  // h[3][6][493]->Draw();

  string outFile = type[eventType] + "out.root";

  TFile *out = new TFile (outFile.c_str(),"recreate");
  for (size_t i = 0; i < NUM_R; i++) {
    // cout << "i: " << i;
    for (size_t j = 0; j < NUM_Z; j++) {
      // cout << "\tj: " << j;
      for (size_t k = 0; k < NUM_PMT; k++) {
        // cout << "\tk: " << k;
        // TCanvas *c = new TCanvas("c","",600,800);
        // h[i][j][k]->Draw();
        h[i][j][k]->Write();
      }
    }
  }
  out->Close();

}

void fitting(string f) {

  string fileName = "Data/Histograms/" + f +"_tc.root";

  cout << "Fitting file " << fileName << endl;

  //Call constructor for histograms
  for (int q = 0; q < NUM_PMT; q++) {
    h[rIndex][zIndex][q] = new TH1F(TString::Format("%s %d", f.c_str(),q),"Histogram",NUM_BIN,-50,550);
    // printf("Wrote to h[%d][%d][%d]\n", rIndex,zIndex,q);
  }

  //Load the file
  // TFile *inFile = new TFile (fileName.c_str());

  //Holds the histogram from gpTail2
  TH1F *hf;

  //Read the fit parameters, each row in fitPars corresponds to one the fit for one PMT
  Double_t** fitPars = readParams(f);

  TF1 *gpTail2 = new TF1("gpTail2",func,-50,550,7);
  gpTail2->SetNpx(NUM_BIN);

  //Loop through the PMTs
  for (int i = 0; i < NUM_PMT; i++) {

    gpTail2->SetParameters(fitPars[i]);
    hf = (TH1F*)gpTail2->GetHistogram();

    for (int j = 1; j <= NUM_BIN; j++) {
      // x = hf[i]->GetBinContent(j);
      // x = -TMath::Log(x);
      h[rIndex][zIndex][i]->SetBinContent(j, -TMath::Log(hf->GetBinContent(j)));
    }
  }
  // inFile->Close();

  // h[0][0][1]->Draw();

}

Double_t** readParams(string f) {

  Double_t tmp;
  string fileName = "Data/Params/" + f + ".par";

  // cout << "Reading params for file " << fileName << endl;

  //Allocate memory
  Double_t **fitPars = new Double_t*[NUM_PMT];
  for (size_t i = 0; i < NUM_PMT; i++) {
    fitPars[i] = new Double_t[7];
  }

  //Open file
  ifstream datfile;
  datfile.open(fileName);

  //Error Checking
  if (!(datfile.is_open())) {
    cerr << "File " << fileName << " failed to open" << endl;
    return NULL;
  }

  // printf("looping here\n");
  for (int i = 0; i < NUM_PMT; i++) {
    if (datfile.eof()) {
      cerr << "Unexpected End-of-File" << endl;
      return NULL;
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

  // cout << "returning from readParams" << endl;
  return fitPars;
}

Double_t func(Double_t *x, Double_t *params) {
  Double_t retVal =
  sqrt(3.14/2.0) * (params[0]*params[2]/(params[4]*params[5]))
  * exp( -x[0] * (1/params[4] + 1/params[5]) )
  * ( params[3]
    * exp( params[2]*params[2] / (2*params[4]*params[4])
      + params[1]/params[4] + x[0]/params[5] )
    * params[5]
    * TMath::Erfc( (params[2]*params[2] + (params[1]-x[0]) * params[4])
      / (sqrt(2.0)*params[2]*params[4]) )
    + exp( (params[2]*params[2] + 2*params[1]*params[5])
      / (2*params[5]*params[5]) + x[0]/params[4] )
    * params[4]
    * TMath::Erfc( (params[2]*params[2] + (params[1]-x[0])*params[5])
      / (sqrt(2.0)*params[2]*params[5]) )
    )
  + params[6];

  return retVal;
}
