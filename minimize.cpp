#include "header.h"
#include "eventReader.cpp"

/*
  This is the main file of the program.  The function minimize should be called
  with the name of the event file to read, the desired name of the output file,
  and the desired quantum efficiency (.3 for NR and .2158 for EE).
*/

void min2vtx (Int_t &nPar, Double_t *zGrad, Double_t &fVal, Double_t *par, Int_t iFlag);
Int_t eventListCreator();
void minimizeEE();
void minimizeNR();

//Program vars
vector<vector<Double_t>> eventList;
ifstream infile;
Int_t hyp, nIterations;
TRandom3 *r;
Float_t qEff;

//Minuit vars
TMinuit *myMinuit;
Double_t vStart[2],argList[2];
Double_t fMin,edm,errDef;
Int_t errFlag;
Int_t nvpar,nparx,icstat;
Double_t parValue,parError;
Float_t zAvg,tAvg, zzMax = 1440, zzMin = 0;

//TTree vars
Float_t zRecNR,tRecNR,fRecNR;
Int_t nIterNR = 0, nHitNR = 0;
Float_t zRecEE,tRecEE,fRecEE;
Int_t nIterEE = 0, nHitEE = 0;

//Main function
void minimize(string f, string out, Float_t q) {

  //Stopwatch to keep track of time to read histograms
  TStopwatch *myClock = new TStopwatch();
  myClock->Start();
  readHistograms();
  myClock->Stop();
  printf("%.2f seconds to read histograms\n", myClock->CpuTime());
  myClock->Reset();

   //open event file
   infile.open(f);
   if (!(infile.is_open())) {
     cerr << "File " << f << " failed to open" << endl;
     return 0;
   }

   //--- Open the ROOT output file:
   TFile *outFile = new TFile (out.c_str(),"recreate");

   //--- Define the event tree:
   TTree *Event = new TTree ("Event","Event tree info");

   Event->Branch ("nIterEE",&nIterEE,"niter/I");
   Event->Branch ( "zRecEE", &zRecEE,"zrec/F");
   Event->Branch ( "tRecEE", &tRecEE,"trec/F");
   Event->Branch ( "fRecEE", &fRecEE,"frec/F");
   Event->Branch ( "nHitNR", &nHitNR,"nhit/I");

   Event->Branch ("nIterNR",&nIterNR,"niter/I");
   Event->Branch ( "zRecNR", &zRecNR,"zrec/F");
   Event->Branch ( "tRecNR", &tRecNR,"trec/F");
   Event->Branch ( "fRecNR", &fRecNR,"frec/F");
   Event->Branch ( "nHitEE", &nHitNR,"nhit/I");

   Int_t status = 0, eventNum = 0;
   Double_t x,y;

   r = new TRandom3(88);
   qEff = q;

   //Status keeps track of whether there are more events in the file
   status = eventListCreator();

   myClock->Start();

   //Read the geometry file to PMTCoords so mapping can occur
   PMTCoords = readGeometryFile();

   do {
      eventNum++;

      //get x and y coords so mapping can occur
      x = eventList[0][0];
      y = eventList[0][1];
      PMTMap = mapping(x,y);

      //Perform Minuit minimization using both EE and NR histograms
      minimizeEE();
      minimizeNR();

      // printf("\n\t\tEvent #%d\nEE:\n", eventNum);
      // printf("nIter = %d\nz = %.4f\nt = %.4f\nf = %.4f\nhits = %d\n", nIterEE,zRecEE,tRecEE,fRecEE,nHitEE);
      // printf("NR:\n");
      // printf("nIter = %d\nz = %.4f\nt = %.4f\nf = %.4f\nhits = %d\n", nIterNR,zRecNR,tRecNR,fRecNR,nHitNR);

      //Fill the TTree
      Event->Fill();

      //Clear the dynamic array then fill it with the next event
      eventList.clear();
      status = eventListCreator();
    } while(status != 0);

    myClock->Stop();

    infile.close();
    Event->Write();
    outFile->Close();

    printf ("\nTime to generate %d events = %.3f s\n",eventNum,myClock->CpuTime());
    printf ("Average time per event = %.3f s\n",myClock->CpuTime()/eventNum);
    // printf("Time spent in evtClock is %f\n", timeElapsed);
    // printf("Avg time per mapping is %f\n", timeElapsed/mappingCount);

}

//Used by Minuit to perform the minimization
void min2vtx (Int_t &nPar, Double_t *zGrad, Double_t &fVal, Double_t *par, Int_t iFlag) {

  fVal = eventReader(eventList,par[0],par[1], hyp);
  nIterations++;

  // //--- Rescale the functional value and update the number of iterations.
  // //    Also add punishing terms if the vertex goes out of bounds.
  //
  //  nIterations ++;
  //  fVal /= (totHits[0]+totHits[1]);// scale by total number of hits
  //
  if (par[0] > zzMax) fVal += 1.0e6 * TMath::Power (par[0]-zzMax,2);
  if (par[0] < zzMin) fVal += 1.0e6 * TMath::Power (par[0]-zzMin,2);
}

void minimizeEE() {
  hyp = 0;
  nIterations = 0;

  myMinuit = new TMinuit (2); // 2-vertex minimization
  myMinuit->SetPrintLevel(-1); // [-1,3], default=0 (minimum output)
  myMinuit->SetFCN (min2vtx);
  vStart[0] = 726;
  vStart[1] = 0;
  //                                   step-size, previously 10 & 1
  //                                    |
   myMinuit->mnparm (0,"z",vStart[0],10.0,0.0,0.0,errFlag);
   myMinuit->mnparm (1,"t",vStart[1], 2.0,0.0,0.0,errFlag);
  //                           |
  //                           starting values (tevt=0 zevt=726)
  //--- Set the maximum number of iterations, tolerance, and initialize the
  //    number of iterations.
   argList[0]  = 5.0e+2;// maximum number of iterations
   argList[1]  = 1.0e-4;// functional tolerance

  // for (size_t i = 0; i < eventList.size(); i++) {
  //   for (size_t j = 0; j < eventList[i].size(); j++) {
  //     cout << eventList[i][j] << " ";
  //   }
  //   cout << endl;
  // }
  // cout << endl << "-------------------------------------------------------" << endl;

  // Perform the minimization
    // evtClock->Start();
    myMinuit->mnexcm ("SIMPLEX",argList,2,errFlag);
    // evtClock->Stop();
    // evtClock->Reset();
    // printf("EE minimization took %.3f seconds\n", evtClock->CpuTime());

  //--- Get the minimization results:
    myMinuit->mnstat (fMin,edm,errDef,nvpar,nparx,icstat);
    fRecEE = fMin;
    myMinuit->GetParameter (0,parValue,parError); zRecEE = parValue;
    myMinuit->GetParameter (1,parValue,parError); tRecEE = parValue;
    nIterEE = nIterations;
    nHitEE = eventList.size();
    // cout << "Event #" << eventNum << endl << "  n-iter = " << nIterations << endl << "  z = " << zRec << endl << "  t = " << tRec
    //      << endl << "  f = " << fRec << endl;
}

void minimizeNR() {
  hyp = 1;
  nIterations = 0;

  myMinuit = new TMinuit (2); // 2-vertex minimization
  myMinuit->SetPrintLevel(-1); // [-1,3], default=0 (minimum output)
  myMinuit->SetFCN (min2vtx);
  vStart[0] = 726;
  vStart[1] = 0;
  //                                     step-size, previously 10 & 1
  //                                      |
  myMinuit->mnparm (0,"z",vStart[0],10.0,0.0,0.0,errFlag);
  myMinuit->mnparm (1,"t",vStart[1], 1.0,0.0,0.0,errFlag);
  //                           |
  //                           starting values (tevt=0 zevt=726)
  //--- Set the maximum number of iterations, tolerance, and initialize the
  //    number of iterations.
  argList[0]  = 5.0e+2;// maximum number of iterations
  argList[1]  = 1.0e-4;// functional tolerance

  // for (size_t i = 0; i < eventList.size(); i++) {
  //   for (size_t j = 0; j < eventList[i].size(); j++) {
  //     cout << eventList[i][j] << " ";
  //   }
  //   cout << endl;
  // }
  // cout << endl << "-------------------------------------------------------" << endl;

  // Perform the minimization
  // evtClock->Start();
  myMinuit->mnexcm ("SIMPLEX",argList,2,errFlag);
  // evtClock->Stop();
  // evtClock->Reset();
  // printf("NR minimization took %.3f seconds\n", evtClock->CpuTime());

  //--- Get the minimization results:
  myMinuit->mnstat (fMin,edm,errDef,nvpar,nparx,icstat);
  fRecNR = fMin;
  myMinuit->GetParameter (0,parValue,parError); zRecNR = parValue;
  myMinuit->GetParameter (1,parValue,parError); tRecNR = parValue;
  nIterNR = nIterations;
  nHitNR = eventList.size();
  // cout << "Event #" << eventNum << endl << "  n-iter = " << nIterations << endl << "  z = " << zRec << endl << "  t = " << tRec
  //      << endl << "  f = " << fRec << endl;
  }

//Reads from the event file one event at a time into a dynamic array and returns
//1 if there are events remaining, 0 otherwise.
//Stores the x and y of an event in the first row
Int_t eventListCreator() {

  Double_t x,y, tPMT, PMT;

  //2D array of doubles
  vector<Double_t> tmpVec;

  //throw away junk
  string tmp;
  infile >> tmp;
  infile >> tmp;
  infile >> tmp;

  if (infile.eof()) {
    return 0;
  }

  // printf("line1:%s\n", tmp.c_str());
  //read vertex
  infile >> x;
  infile >> y;
  tmpVec.push_back(x);
  tmpVec.push_back(y);
  eventList.push_back(tmpVec);
  tmpVec.clear();

  infile >> tmp;  //ignore z
  // printf("---------------------------------\nVertex is (%0.f,%0.f,%0.f)\n", x,y,z);

  //read a (PMT,tPMT) pair until there is a read failure (this happens when
    //it attempts to read the "Event/nhS1/vtx(mm):" string into a Double_t)
  while (!(infile.fail())) {

    // printf("Read PMT %d\n", PMT);

    //Read the next (PMT,tPMT) pair
    // printf("%d %0.2f\n", PMT, tPMT);
    infile>>PMT;
    infile>>tPMT;

    //Only use the PMT hit if a randomly generated number is below the quantum efficiency
    if (r->Uniform() < qEff) {
      tmpVec.push_back(PMT);
      tmpVec.push_back(tPMT);
      eventList.push_back(tmpVec);
      tmpVec.clear();
    }
  }
  eventList.pop_back();

  //Clear failbit
  infile.clear();

  return 1;

}


//--------------------------------------------------
//Code below is for grid search minimization, currently unused

//TODO - remove
// #include <chrono>
// typedef std::chrono::high_resolution_clock Clock;

// void minimize() {
//
//   Double_t value=.5;
//   Int_t zBin,tBin;
//   TH2D *h2 = new TH2D("h2","Event",169,-150,1540,100,-10,10);
//
//   testread();
//
//   auto t1 = Clock::now();
//   for (Double_t tEvent = -100; tEvent < 100; tEvent+=2) {
//     for (Double_t z = -150; z < 1540; z+=10) {
//       // value = eventReader("10_events.ascii",z,tEvent/10);
//       value = eventReader("1_event.ascii",z,tEvent/10);
//       printf("Value at t=%.1f, z=%0.f is %.5f\n", tEvent/10,z,value);
//       zBin = (z+160)/10;
//       tBin = ((tEvent+120)/2)-9;
//       // printf("z=%.0f\tzBin = %d\tt=%.1f\ttBin = %d\n", z,zBin,tEvent,tBin);
//       h2->SetBinContent(zBin, tBin, value);
//     }
//   }
//   auto t2 = Clock::now();
//
//   cout << "Time Elapsed: " << std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count()
//         << " seconds" << endl;
//
//   h2->Draw("LEGO");
//
//   TFile *out = new TFile("FifthEvent.root", "recreate");
//   h2->Write();
//
//
// }
