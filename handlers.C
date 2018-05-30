void handlers()
{
  {
    gROOT->LoadMacro(gSystem->ExpandPathName("$ALICE_ROOT/ANALYSIS/macros/train/AddESDHandler.C"));
    AliVEventHandler* handler = AddESDHandler();
AliAnalysisManager::GetAnalysisManager()->SetAsyncReading(kFALSE);
  }
}
