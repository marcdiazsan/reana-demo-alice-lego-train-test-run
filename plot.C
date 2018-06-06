void plot()
{

  TFile *file = TFile::Open("GammaConvFlow_69.root");
  TList *list = (TList*) gFile->Get("GammaConvV1_69_v2");

  TH1D *plot = (TH1D*) list->FindObject("EPVZ");

  TCanvas* c1 = new TCanvas("can1", "can1", 1200, 800);
  if(plot)
    plot->DrawCopy();

  c1->SaveAs("plot.pdf");
}
