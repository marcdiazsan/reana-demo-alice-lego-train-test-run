#ifdef __CLING__
R__ADD_INCLUDE_PATH($ALICE_ROOT)
#include "ANALYSIS/macros/train/AddESDHandler.C"
#endif

void handlers()
{
  {
    #ifndef __CLING__
    gROOT->LoadMacro(gSystem->ExpandPathName("$ALICE_ROOT/ANALYSIS/macros/train/AddESDHandler.C"));
    #endif
    AliVEventHandler* handler = AddESDHandler();
  }
}
