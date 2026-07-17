#include "mini.h"
#include "fileHelper.h"
#include <iostream>
#include <string>
#include <fstream>
#include <TLorentzVector.h>
#include <filesystem>

using namespace std;

int main(int argn, char *argv[]) {

  //if you want to run this script for several input files, it is useful to call the name of the file when executing this program
  if(argn==1){
    cout << "Please start runSelection.exe with added argument of file to be processed" << endl;
    return 1;
  }
  // path to the file to be studied + filename from argument called when executing file
  string inpath = string(argv[1]);

  // TString filename = TString(inpath).ReplaceAll("/nfs/homes/zprime/zprime10/ttbar_example.root", "ttbar_example.root");
  // TString filename = TString(inpath).ReplaceAll("/nfs/homes/zprime/zprime10/data_example.root", "data_example.root");
  // This is path for samples folder. This NEEDS to be adjusted if you use a different path in order to extract just the filename
  TString filename = TString(inpath).ReplaceAll("/ceph/e4/users/bgocke/Zprime/Samples/", "");

  cout << "Processing " << filename << endl;

  // retrieve the tree from the file
  mini * tree = fileHelper::GetMiniTree(inpath);
  if (tree == 0) {
    cout << "ERROR tree is null pointer" << endl;
    return 1;
  }

  // check that the tree is not empty
  int nEntries = tree->GetEntries();
  cout << "INFO tree contains " << nEntries << " events" << endl;
  if (nEntries == 0) {
    cout << "ERROR tree contains zero events" << endl;
    return 1;
  }

  // create file to which the selection is going to be saved to
  TString outpath = "output_Selection/" + filename;
  outpath.ReplaceAll(".root", "_selected.root");
  TFile * newFile = new TFile(outpath, "RECREATE");
  
  string directory = "output_Selection";
  string safe_filename = static_cast<string>(filename);
  std::replace(safe_filename.begin(), safe_filename.end(), '/', '_');
  string file_name = "output_Selection/efficiency_" + safe_filename + ".csv";

  std::ofstream file(file_name);

  if (!file.is_open()) {
	std::cerr << "Error: not open lol" << filename << std::endl;
  }
  // make a copy of the tree to be saved after selection
  TTree * newTree = tree->CloneTree();

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // To do: initalize Variables for the effiencies
  struct eff{
	double eff_n_lep = 0.;
	double eff_lept_pt = 0.;
	double eff_n_jets = 0.;
	double eff_jet_pt = 0.;
	double eff_scalef = 0.;
	double eff_miss_pt = 0.;
	double eff_tot = 0.;
  };

  eff eff;
  ////////////////////////////////////////////////////////////////////////////////////////////////////

  // now loop over the events in the tree
  for (int iEntry = 0; iEntry < nEntries; ++iEntry) {

    // get entry no. iEntry and tell every 100000th event
    tree->GetEntry(iEntry);
    if ((iEntry+1)%100000 == 0) {
      cout << "INFO processing the " << iEntry+1 << "th event" << endl;
    }

    bool passCriteria = true;

    /////////////////////////////////////////////////////////////////////////////////////////////
    // To do: Implement all required selection criteria //
    TLorentzVector lep;

// lep_n
    if (tree -> lep_n != 1) {
	passCriteria = false;
	continue;
    }
    eff.eff_n_lep += 1;

// lep_pt
    if (tree -> lep_pt -> at(0) < 70000) {
	passCriteria = false;
	continue;
    }
    eff.eff_lept_pt += 1;

// number of jets
    if (tree -> jet_n != 3 && tree -> jet_n != 4) {
	passCriteria = false;
	continue;
    }
    eff.eff_n_jets += 1;

//high-pt jets, 1 b-tagged
    bool any_b_tag = false;
    int high_pt_counter = 0;
    for (unsigned int i=0; i < tree -> jet_n; i++){
      if (tree -> jet_pt -> at(i) > 50000) {
	high_pt_counter += 1;
      }
      if (tree -> jet_MV2c10 -> at(i) > 0.83){
	any_b_tag = true;
      }
    }

    if (high_pt_counter < 2) {
      passCriteria = false;
      continue;
    }
    eff.eff_jet_pt +=1;

    if (!any_b_tag) {
      passCriteria = false;
      continue;
    }
    eff.eff_scalef += 1;

// missing energy
    if (tree -> met_et < 50000) {
	passCriteria = false;
	continue;
    }
    eff.eff_miss_pt += 1;
    /////////////////////////////////////////////////////////////////////////////////////////////
    // you can use continue if one of the cuts is not fullfilled
    /////////////////////////////////////////////////////////////////////////////////////////////

    // check all selection criteria and only save the event to the new
    // tree if all of them are true
    if (passCriteria) {
      newTree->Fill();
    }

  }

  eff.eff_tot = (static_cast<double>(newTree->GetEntries())/static_cast<double>(nEntries));

  file << "eff_n_lep,eff_lept_pt,eff_n_jets,eff_jet_pt,eff_scalef,eff_miss_pt,eff_tot\n";
  file << eff.eff_n_lep/static_cast<double>(nEntries) << ","
	<< eff.eff_lept_pt/static_cast<double>(nEntries) << ","
	<< eff.eff_n_jets/static_cast<double>(nEntries) << ","
	<< eff.eff_jet_pt/static_cast<double>(nEntries) << ","
	<< eff.eff_scalef/static_cast<double>(nEntries) << ","
	<< eff.eff_miss_pt/static_cast<double>(nEntries) << ","
	<< eff.eff_tot << "/n";
  file.close();
  // save new tree to file
  cout << "INFO saving new tree with " << newTree->GetEntries() << " entries" << endl;
  newFile->Write();
  gDirectory->Purge();
  newFile->Close();

  // end program happily
  delete newFile;

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // To do: Write code here to print the efficenies
  ////////////////////////////////////////////////////////////////////////////////////////////////////

  return 0;
}
