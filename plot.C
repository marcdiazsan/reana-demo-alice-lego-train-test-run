void plot() {
  TFile *file = TFile::Open("AnalysisResults.root");
  TList *list = (TList*) gFile->Get("NetNpiMCBASE");

  TH1F *plot = (TH1F*) list->FindObject("fHistCentPid");

  TCanvas* c1 = new TCanvas("can1", "can1", 1200, 800);
  if(plot){
    plot->GetXaxis()->SetTitle("centrality");
    plot->GetYaxis()->SetTitle("events");
    plot->DrawCopy();

    c1->SaveAs("plot.pdf");
  }
}
