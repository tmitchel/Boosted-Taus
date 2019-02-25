// Copyright [2019] Tyler Mitchell

#include <iostream>
#include <memory>
#include <vector>
#include "TFile.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "interface/histManager.h"
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
  auto output_name = parser->Option("-o");
  auto tree_name = parser->Option("-t", "ggNtuplizer/EventTree");

  // read the input TFile/TTree
  auto fin = std::make_shared<TFile>(input_name.c_str(), "READ");
  auto hists = std::make_shared<histManager>(output_name);
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
    if (i == progress * fraction) {
      std::cout << progress*10 << "% complete.\r" << std::flush;
      progress++;
    }

    // run all the factories to fill variables
    gen_factory.Run_Factory();

    auto gens = gen_factory.getGens();
    VGen all_gen_higgs, all_gen_taus;
    for (auto gen : *gens) {
      if (fabs(gen.getPID()) == 25) {
        all_gen_higgs.push_back(gen);
      } else if (fabs(gen.getPID()) == 15) {
        all_gen_taus.push_back(gen);
      }
    }

    // only look at di-tau events
    if (all_gen_taus.size() < 2) {
      continue;
    }

    hists->Fill("allpt_dr_gen", all_gen_taus.at(0).getP4().DeltaR(all_gen_taus.at(1).getP4()), 1.);

    // see if there is a boosted Higgs in this event
    auto boosted_higgs(false);
    for (auto higgs : all_gen_higgs) {
      if (higgs.getPt() > 500) {
        boosted_higgs = true;
        break;  // found one so we don't need to search any longer
      }
    }

    if (boosted_higgs) {
      hists->Fill("highpt_dr_gen", all_gen_taus.at(0).getP4().DeltaR(all_gen_taus.at(1).getP4()), 1.);
    } else {
      hists->Fill("lowpt_dr_gen", all_gen_taus.at(0).getP4().DeltaR(all_gen_taus.at(1).getP4()), 1.);
    }
  }

  std::cout << std::endl;
  fin->Close();
  hists->Write();
  return 1;
}
