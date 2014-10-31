draw_angle()
{
/// get the information of final

  gStyle->SetOptTitle(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetOptStat(kFALSE);
  gStyle->SetOptTitle(0);
  gStyle->SetPalette(1,0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);

  TH1D *hsig = new TH1D("hsig","hsig",90,0,180);
  TH1D *hhig = new TH1D("hhig","hhig",90,0,180);
  TH1D *hbkg = new TH1D("hbkg","hbkg",90,0,180);

  TChain *fChain1 = new TChain("MCPart");
  fChain1->Add("/besfs/groups/higgs/data/workarea_chenzx/Analysis/ww/wwAna/result/4lep/signal_*.root");
  TChain *fChain2 = new TChain("MCPart");
  fChain2->Add("/besfs/groups/higgs/data/workarea_chenzx/Analysis/ww/wwAna/result/4lep/hig_bkg_*.root");
  TChain *fChain3 = new TChain("MCPart");
  fChain3->Add("/besfs/groups/higgs/data/workarea_chenzx/Analysis/ww/wwAna_bkg/result/4lep/*.root");



  TCanvas *myCanvas = new TCanvas();
  myCanvas->Divide(1,1);
  myCanvas->SetFrameLineWidth(3);
  myCanvas->SetFillColor(10);
  myCanvas->cd(1);
  gPad->SetTopMargin(0.015);
  gPad->SetRightMargin(0.08);
  gPad->SetLeftMargin(0.18);
  gPad->SetBottomMargin(0.16);
  gPad->SetFrameLineWidth(2);
  gPad->SetFillColor(0);
  gPad->SetFrameFillColor(0);
  gPad->SetLogy();

  fChain1->Draw("LLAngle>>hsig","NCh==4&&NChP==2&&NChM==2&&NLep==4&&(Invll<80||Invll>100)&&InvMass>80&&InvMass<100");
  fChain2->Draw("LLAngle>>hhig","NCh==4&&NChP==2&&NChM==2&&NLep==4&&(Invll<80||Invll>100)&&InvMass>80&&InvMass<100");
  fChain3->Draw("LLAngle>>hbkg","NCh==4&&NChP==2&&NChM==2&&NLep==4&&(Invll<80||Invll>100)&&InvMass>80&&InvMass<100");


  hbkg->GetXaxis()->SetTitle("N_{iso}");
  hbkg->GetYaxis()->SetTitle("Events");
  hbkg->GetYaxis()->SetLabelOffset(0.012);
  hbkg->GetYaxis()->SetTitleOffset(1.25);
  hbkg->GetXaxis()->SetLabelOffset(0.012);
  hbkg->GetXaxis()->SetTitleOffset(0.95);
  hbkg->GetXaxis()->SetTitleSize(0.07);
  hbkg->GetXaxis()->SetLabelSize(0.06);
  hbkg->GetYaxis()->SetTitleSize(0.07);
  hbkg->GetYaxis()->SetLabelSize(0.06);
  hbkg->GetXaxis()->SetNdivisions(505);
  double scale1=1.0*hsig->GetMaximum()/hhig->GetMaximum();
  double scale2=1.0*hsig->GetMaximum()/hbkg->GetMaximum();
  hsig->SetLineWidth(2);
  hhig->SetLineWidth(2);
  hbkg->SetLineWidth(2);
  hsig->SetLineColor(kRed);
  hhig->SetLineColor(kGreen);
  hbkg->SetLineColor(kBlue);
//  hhig->Scale(scale1);
//  hbkg->Scale(scale2);
  hbkg->Scale(0.1);
  hbkg->Draw("");
  hhig->Draw("same");
  hsig->Draw("same");

  lg1 =new TLegend(0.55,0.65,0.9,0.9);//   (0.75,0.75,0.965555,0.95);
  lg1->SetFillColor(10);
  lg1->AddEntry(hsig," Signal");
  lg1->AddEntry(hhig," Higgs Background");
  lg1->AddEntry(hbkg," Other Background");
  lg1->Draw("same");


}
                                   
