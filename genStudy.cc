// Copyright [2019] Tyler Mitchell

#include <iostream>
#include <memory>
#include <vector>
#include "TFile.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "interface/CLParser.h"
#include "interface/gen_factory.h"
#include "interface/jets_factory.h"
#include "interface/muon_factory.h"
#include "interface/electron_factory.h"
#include "interface/boosted_factory.h"
#include "interface/tau_factory.h"


using std::string;
using std::vector;

int main(int argc, char** argv) {
  auto parser = std::unique_ptr<CLParser>(new CLParser(argc, argv));
  auto input_name = parser->Option("-i");
  auto tree_name = parser->Option("-t", "ggNtuplizer/EventTree");
  if (tree_name == "") {
    tree_name = "ggNtuplizer/EventTree";
  }

  // read the input TFile/TTree
  auto fin = std::make_shared<TFile>(input_name.c_str(), "READ");
  auto tree = reinterpret_cast<TTree*>(fin->Get(tree_name.c_str()));

  // construct our object factories
  auto gen_factory = Gen_Factory(tree);
  auto jets_factory = Jets_Factory(tree);
  auto muon_factory = Muon_Factory(tree);
  auto electron_factory = Electron_Factory(tree);
  auto boosted_factory = Boosted_Factory(tree);
  auto tau_factory = Tau_Factory(tree);

  auto nevts = tree->GetEntries();
  int progress(1), fraction(nevts/10);
  for (auto i = 0; i < nevts; i++) {
    tree->GetEntry(i);
    if (i == progress*fraction) {
      std::cout << progress*10 << "% complete.\r" << std::flush;
      progress++;
    }

    tau_factory.Run_Factory();
    gen_factory.Run_Factory();
    if (tau_factory.getTaus()->size() > 0 && gen_factory.getGens()->size() > 0) {
      // std::cout << tau_factory.getTaus()->at(0).getPt() << " " << gen_factory.getGens()->at(0).getP4().Pt() << std::endl;
    }
  }
  std::cout << std::endl;
  fin->Close();
  return 1;
}
