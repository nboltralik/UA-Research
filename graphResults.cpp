void graphResults(string ee, string nr)
{
 TFile *inee=new TFile (ee.c_str());
 TFile *innr=new TFile (nr.c_str());

 TTree *tee=(TTree*)inee->Get("Event");
 TTree *tnr=(TTree*)innr->Get("Event");

 TH1D *hee=new TH1D ("hee","EE",100,-0.2,0.2);
 TH1D *hnr=new TH1D ("hnr","NR",100,-0.2,0.2);

 tee->Project("hee","fRecEE-fRecNR");
 tnr->Project("hnr","fRecEE-fRecNR");

 hee->SetLineColor(2);
 hnr->SetFillColor(16);
 hnr->Draw();
 hee->Draw("same");

}
