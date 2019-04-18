// Copyright [2019] Tyler Mitchell

#include <iostream>
#include <memory>
#include <vector>
#include "TFile.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "interface/CLParser.h"
#include "interface/boosted_factory.h"
#include "interface/histManager.h"
#include "interface/jets_factory.h"
#include "interface/tau_factory.h"
#include "interface/gen_factory.h"

using std::string;
using std::vector;

int main(int argc, char **argv) {
  auto parser = std::unique_ptr<CLParser>(new CLParser(argc, argv));
  auto verbose = parser->Flag("-v");
  auto input_name = parser->Option("-i");
  auto output_name = parser->Option("-o");
  auto tree_name = parser->Option("-t", "ggNtuplizer/EventTree");
  auto histograms = parser->Option("-j", "test.json");

  // read the input TFile/TTree
  auto fin = std::shared_ptr<TFile>(TFile::Open(input_name.c_str()));
  auto hists = std::make_shared<histManager>(output_name);
  hists->load_histograms(histograms);
  auto tree = reinterpret_cast<TTree *>(fin->Get(tree_name.c_str()));

  auto gen_factory = Gen_Factory(tree);
  auto tau_factory = Tau_Factory(tree);
  auto boost_factory = Boosted_Factory(tree);
  auto jet_factory = Jets_Factory(tree);

  auto nevts = tree->GetEntries();
  int progress(1), fraction((nevts - 1) / 10);
  for (auto i = 0; i < nevts; i++) {
    tree->GetEntry(i);
    if (i == progress * fraction && verbose) {
      std::cout << "\t" << progress * 10 << "% complete.\r" << std::flush;
      progress++;
    }

    gen_factory.Run_Factory();
    tau_factory.Run_Factory();
    boost_factory.Run_Factory();
    jet_factory.Run_Factory();
    auto gens = gen_factory.getGens();
    auto taus = tau_factory.getTaus();
    auto boosts = boost_factory.getTaus();
    auto jets = jet_factory.getJets();

    hists->FillBin("cutflow", 1, 1.);
    if (boost_factory.getNGoodBoosted() > 0 && jet_factory.getNGoodJets() > 0) {
      hists->Fill2d("boost-vs-jet-pt", boost_factory.getTaus()->at(0).getPt(), jets->at(0).getPt(), 1.);
    }

    bool found_jet(false);
    for (auto &jet : *jets) {
      if (jet.getPt() > 200) {
        found_jet = true;
        break;
      }
    }

    // only look at events with high pT jet
    if (found_jet) {
      hists->FillBin("cutflow", 2, 1.);
    } else {
      continue;
    }

    // need to have at least 1 tau in the event
    if (boost_factory.getNGoodBoosted() > 0 && boost_factory.getTaus()->at(0).getPt() > 50) {
      hists->FillBin("cutflow", 3, 1.);
    } else {
      continue;
    }

    hists->Fill("boost_pt", boost_factory.getTaus()->at(0).getPt(), 1.);
    for (auto const &gen : *gens) {
      if (fabs(gen.getPID()) == 25) {
        hists->FillBin("cutflow", 4, 1.);
      }
    } 

    fin->Close();
    hists->Write();
    return 1;
  }
}
