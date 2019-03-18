// Copyright [2019] Tyler Mitchell

#include <iostream>
#include <memory>
#include <vector>
#include "TFile.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "interface/CLParser.h"
#include "interface/gen_factory.h"
#include "interface/histManager.h"
#include "interface/tau_factory.h"
#include "interface/boosted_factory.h"
#include "interface/jets_factory.h"

using std::string;
using std::vector;

int main(int argc, char** argv) {
  auto parser = std::unique_ptr<CLParser>(new CLParser(argc, argv));
  auto verbose = parser->Flag("-v");
  auto input_name = parser->Option("-i");
  auto output_name = parser->Option("-o");
  auto tree_name = parser->Option("-t", "ggNtuplizer/EventTree");

  // read the input TFile/TTree
  auto fin = std::shared_ptr<TFile>(TFile::Open(input_name.c_str()));
  auto hists = std::make_shared<histManager>(output_name);
  auto tree = reinterpret_cast<TTree*>(fin->Get(tree_name.c_str()));
  // construct our object factories
  auto gen_factory = Gen_Factory(tree);
  auto tau_factory = Tau_Factory(tree);
  auto boost_factory = Boosted_Factory(tree);
  auto jet_factory = Jets_Factory(tree);

  auto nevts = tree->GetEntries();
  int progress(1), fraction((nevts-1)/10);
  for (auto i = 0; i < nevts; i++) {
    tree->GetEntry(i);
    if (i == progress * fraction && verbose) {
      std::cout << "\t" << progress*10 << "% complete.\r" << std::flush;
      progress++;
    }

    // keep track of the number of events processed
    hists->Fill("nevents", 1., 1.);

    // run all the factories to fill variables
    gen_factory.Run_Factory();
    tau_factory.Run_Factory();
    boost_factory.Run_Factory();
    jet_factory.Run_Factory();
    auto gens = gen_factory.getGens();
    auto taus = tau_factory.getTaus();
    auto boosts = boost_factory.getTaus();
    auto jets = jet_factory.getJets();

    bool found_jet(false);
    for (auto &jet : *jets) {
      if (jet.getPt() > 500) {
        found_jet = true;
        break;
      }
    }

    // only look at events with high pT jet
    if (!found_jet) {
      continue;
    }

    // need to have at least 1 tau in the event
    if (boost_factory.getNGoodBoosted() < 1) {
      continue;
    }

    // find gen-level taus
    VGen all_gen_higgs, all_gen_taus, all_gen_jets;
    for (auto const &gen : *gens) {
      if (fabs(gen.getPID()) == 15) {
        all_gen_taus.push_back(gen);
      }
    }

    // loop over the boosted tau collection
    for (auto &tau : *boosts) {
      for (auto &gen : all_gen_taus) {
        if (tau.getP4().DeltaR(gen.getP4()) < 0.5) {
          std::cout << "DENOMINATOR" << std::endl;
          if (tau.getIso(loose)) {
            std::cout << "NUMERATOR" << std::endl;
          }
        }
      }
    }

  std::cout << std::endl;
  fin->Close();
  hists->Write();
  return 1;
}
