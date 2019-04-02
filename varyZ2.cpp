#include "header.h"
#include "getnLLK.cpp"

TCanvas *c;

Double_t tEvent = -6.3, value = 0, sum;
TH1D *hl[4], *zhPMT;
Int_t zBin;

void varyZ2(Int_t PMTNum) {

  // sprintf(name, "h%d", PMTNum);
  // delete gROOT->FindObject(name);
  zhPMT = new TH1D("","",550,200,750);

  Int_t PMT2 = PMTNum;
  if (PMTNum > 252) {
    PMT2 = PMTNum - 47;
  }

  for (size_t i = 0; i < 4; i++) {
    hl[i] = new TH1D("","",550,200,750);
  }

  for (Int_t z = 200; z < 750; z+=1) {
    // zBin = z/10 - 19;
    zBin = z-199;
    setup(-300,250,z);
    value = getnLLK(-300,250,z,tEvent,PMTNum);
    zhPMT->SetBinContent(zBin, value);

    for (Int_t q = 0; q < 4; q++) {
      Int_t yIndex, zIndex;
      TH1F *h;
      yIndex = points[q]->yIndex;
      zIndex = points[q]->zIndex;
      // printf("Getting h[%d][%d][%d][0]\n", PMTNum, zIndex, yIndex);
      h = histogram[PMT2][zIndex][yIndex][0];
      // Do a 2 point linear interpolation
      Int_t bin1 = floor(tEvent+51);
      Double_t y1 = h->GetBinContent(bin1);
      Double_t y2 = h->GetBinContent(bin1+1);
      Double_t tmp = y1 + (tEvent+51-bin1)*(y2-y1);
      hl[q]->SetBinContent(zBin, tmp*weights[q]);
    }


      // printf("z=%d\n", z);
      // sum=0;
      // for (size_t k = 0; k < 4; k++) {
      //   printf("Point (%d, %d)\t%.3f\n", points[k]->y, points[k]->z, weights[k]);
      //   sum+=weights[k];
      // }
      // printf("sum=%0.2f\n", sum);
    }

  //Close the canvas if it exists to supress warnings
  printf("Deleting old canvas and creating new\n");
  if (c) { c->Close(); gSystem->ProcessEvents(); }
  c = new TCanvas ("c","Name", 900, 1000);
  // c->Divide(2,3);
  c->Divide(2,2);

  // printf("Printing zh[%d] onto c->cd(%d)\n", i, (i+cTmp)%17);
  c->cd(1);
  zhPMT->Draw();

// for (size_t qq = 0; qq < 4; qq++) {
//   hl[qq]->Scale(1.0/hl[qq]->GetEntries());
// }

  c->cd(2);
  TColor *c1 = new TColor(9001,255,0,0);
  hl[0]->SetLineColor(9001);
  hl[0]->Draw();

  // c->cd(3);
  TColor *c2 = new TColor(9002,0,255,0);
  hl[1]->SetLineColor(9002);
  hl[1]->Draw("same");

  c->cd(3);
  TColor *c3 = new TColor(9003,0,0,255);
  hl[2]->SetLineColor(9003);
  hl[2]->Draw("same");

  // c->cd(5);
  TColor *c4 = new TColor(9004,255,255,0);
  hl[3]->SetLineColor(9004);
  hl[3]->Draw("same");

  c->cd(4);
  // zhPMT->Draw();
  hl[2]->Draw("same");
  hl[3]->Draw("same");
  hl[0]->Draw("same");
  hl[1]->Draw("same");
  // hl[3]->GetYAxis()->SetUserRange(0,7);

  c->Print("zSingleHistogram.pdf", "pdf");

}
