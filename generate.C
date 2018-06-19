// Macro to generate macros for the ALICE TRAIN
// Authors: Andrea Gheata, Jan Fiete Grosse-Oetringhaus, Costin Grigoras, Markus Zimmermann
//
//
// requires env variables

void generate(const char *module = "__ALL__")
{
  Int_t nFiles = 1;//TEST_FILES_NO
  Int_t nTestEvents = TString(gSystem->Getenv("TEST_FILES_NO")).Atoi();//for the on the fly trains this contains the number of test events
  TString dataBasePath(gSystem->Getenv("TEST_DIR"));//TEST_DIR
  TString dataAnchor(gSystem->Getenv("FILE_PATTERN"));//FILE_PATTERN
  Int_t splitMaxInputFileNumber = TString(gSystem->Getenv("SPLIT_MAX_INPUT_FILE_NUMBER")).Atoi();//SPLIT_MAX_INPUT_FILE_NUMBER
  Int_t maxMergeFiles = TString(gSystem->Getenv("MAX_MERGE_FILES")).Atoi();//MAX_MERGE_FILES
  Int_t debugLevel = TString(gSystem->Getenv("DEBUG_LEVEL")).Atoi();//DEBUG_LEVEL
  Int_t ttl = TString(gSystem->Getenv("TTL")).Atoi();//TTL
  TString excludeFiles(gSystem->Getenv("EXCLUDE_FILES"));//EXCLUDE_FILES
  TString additionalpackages(gSystem->Getenv("ADDITIONAL_PACKAGES"));//ADDITIONAL_PACKAGES
  //TString dataFolder("/home/mazimmer/Test2/dataFolder");//put here a local directory
  TString dataFolder("./");
  Int_t AOD = TString(gSystem->Getenv("AOD")).Atoi();//AOD: 0 ESD, 1 AOD, 2 AOD produced together with ESD, 3 Kinematics only, 4 ESD (special WSDD production) 5 ESD cpass1 (Barrel), 6 ESD cpass1 (Outer)
  Int_t isPP = strcmp(gSystem->Getenv("PP"), "true"); //0 false, 1 true //PP
  TString validOutputFiles = gSystem->Getenv("OUTPUT_FILES");//OUTPUT_FILES
  TString periodName(gSystem->Getenv("PERIOD_NAME"));//PERIOD_NAME

   const char *train_name="lego_train";

   if (strcmp(module, "__ALL__") == 0)
     module = "";

   gSystem->Load("libANALYSIS");
   gSystem->Load("libANALYSISalice");

   TObjArray *arr = AliAnalysisTaskCfg::ExtractModulesFrom("MLTrainDefinition.cfg");

   Printf(">>>>>>> Read train configuration");
   arr->Print();


   AliAnalysisAlien *plugin = new AliAnalysisAlien(train_name);
   // General plugin settings here
   plugin->SetProductionMode();

   plugin->SetAPIVersion("V1.1x");

   // libraries because we start with root!
   plugin->SetAdditionalRootLibs("libVMC.so libPhysics.so libTree.so libMinuit.so libProof.so libSTEERBase.so libESD.so libAOD.so");

   plugin->SetJobTag("test/test");

   plugin->SetMaxMergeFiles(maxMergeFiles);
   plugin->SetTTL(ttl);
   plugin->SetAnalysisMacro(Form("%s.C", train_name));
   plugin->SetValidationScript("validation.sh");

   plugin->SetRegisterExcludes(excludeFiles + " AliAOD.root");

   // jemalloc
   additionalpackages += " jemalloc::v3.0.0";
   plugin->AddExternalPackage(additionalpackages);

//    plugin->SetExecutableCommand("root -b -q");
   plugin->SetJDLName(Form("%s.jdl", train_name));
   plugin->SetExecutable(Form("%s.sh", train_name));
   plugin->SetSplitMode("se");

   plugin->SetGridOutputDir("./");
   plugin->SetGridWorkingDir("./");

   plugin->SetKeepLogs(kTRUE);

   plugin->SetMergeViaJDL();

   if (dataFolder.Length() == 0)
   {
      Printf("ERROR: TRAIN_TESTDATA not set. Exiting...");
      return;
   }

   if (dataBasePath.Length() > 0)
   {
     TString archiveName = "root_archive.zip";
     if (AOD == 2)
       archiveName = "aod_archive.zip";

     TString dataFileName("data.txt");

     plugin->SetFileForTestMode(Form("%s/%s", dataFolder.Data(), dataFileName.Data()));
     plugin->SetNtestFiles(nFiles);

     // Is MC only?
     if (AOD == 3)
       {
	 Printf(">>>> Expecting MC only production");
	 plugin->SetUseMCchain();
       }
   }

   // Load modules here
   plugin->AddModules(arr);
   plugin->CreateAnalysisManager("train","handlers.C");

   // specific if data is processed or MC is generated on the fly
   if (atoi(gSystem->Getenv("AOD")) == 100) { // MC production
     Long64_t totalEvents = TString(gSystem->Getenv("GEN_TOTAL_EVENTS")).Atoll();

     Long64_t neededJobs = totalEvents / splitMaxInputFileNumber;

     plugin->SetMCLoop(true);
     plugin->SetSplitMode(Form("production:1-%d", neededJobs));
     plugin->SetNMCjobs(neededJobs);
     plugin->SetNMCevents(nTestEvents);
     plugin->SetExecutableCommand("aliroot -b -q");


     plugin->SetKeepLogs(kTRUE);
   } else { // Data, ESD/AOD
     plugin->SetSplitMaxInputFileNumber(splitMaxInputFileNumber);

     plugin->SetExecutableCommand("root -b -q");
     plugin->SetInputFormat("xml-single");
   }

   AliAnalysisManager* mgr = AliAnalysisManager::GetAnalysisManager();
   mgr->SetDebugLevel(debugLevel);
   mgr->SetNSysInfo((isPP == 1) ? 1000 : 40);

   mgr->SetFileInfoLog("fileinfo.log");

   // execute custom configuration
   Int_t error = 0;
   gROOT->Macro("globalvariables.C", &error);
   if (error != 0)
   {
      Printf("ERROR: globalvariables.C was not executed successfully...");
      return;
   }

   plugin->GenerateTest(train_name, module);

   // check for illegally defined output files
   validOutputFiles += "," + excludeFiles;
   TString outputFiles = plugin->GetListOfFiles("out");
   tokens = outputFiles.Tokenize(",");

   Bool_t valid = kTRUE;
   for (Int_t i=0; i<tokens->GetEntries(); i++)
   {
     if (!validOutputFiles.Contains(tokens->At(i)->GetName()))
     {
       Printf("ERROR: Output file %s requested which is not among the defined ones for this train (%s)", tokens->At(i)->GetName(), validOutputFiles.Data());
       valid = kFALSE;
     }
   }
   delete tokens;

   if (!valid)
   {
     Printf(">>>>>>>>> Invalid output files requested. <<<<<<<<<<<<");
     gSystem->Unlink("lego_train.C");
   }
}
