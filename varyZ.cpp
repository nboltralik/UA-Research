#include "header.h"
#include "getnLLK.cpp"

TCanvas *c;

Double_t tEvent = -6.3, value = 0;
TH1D *zh[NUM_PMT];
Int_t zBin, PMTNum, cTmp=17;

void varyZ() {

  for (Int_t i = 0; i < NUM_PMT; i++) {

    PMTNum = i;
    if (PMTNum > 252) {
      PMTNum += 47;
    }

    char* tmp = (char*)malloc(5*(sizeof(char)));
    sprintf(tmp, "h%d", PMTNum);
    // delete gROOT->FindObject(tmp);
    zh[i] = new TH1D(tmp,tmp,55,200,750);

    for (Int_t z = 200; z < 750; z+=10) {
      zBin = z/10 - 19;
      setup(-300,250,z);
      value = getnLLK(-300,250,z,tEvent,PMTNum);
      zh[i]->SetBinContent(zBin, value);
    }

    if (i%16 == 0) {
      //Close the canvas if it exists to supress warnings
      printf("Deleting old canvas and creating new\n");
      if (c) { c->Close(); gSystem->ProcessEvents(); }
      c = new TCanvas ("c","Name", 800, 1000);
      c->Divide(4,4);
      cTmp -=16;
    }

    printf("Printing zh[%d] onto c->cd(%d)\n", i, (i+cTmp)%17);
    c->cd((i+cTmp)%17);
    zh[i]->Draw();  

    if (i == 15) {
      printf("First page\n");
      c->Print("zHistograms.pdf(", "pdf");
      // c->Print("zHistograms.pdf", "pdf");
      // return;
    } else if (i+1 == NUM_PMT) {
      printf("Last page\n");
      c->Print("zHistograms.pdf)", "pdf");
    } else if (i%16 == 15) {
      printf("New page\n");
      c->Print("zHistograms.pdf", "pdf");
      // return;
    }

  }

}
