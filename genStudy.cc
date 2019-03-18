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
    auto gens = gen_factory.getGens();
    auto gen_jets = gen_factory.getGenJets();

    // loop over all gen particles
    VGen all_gen_higgs, all_gen_taus, all_gen_jets;
    for (auto const &gen : *gens) {
      if (fabs(gen.getPID()) == 25) {
        all_gen_higgs.push_back(gen);
      } else if (fabs(gen.getPID()) == 15) {
        all_gen_taus.push_back(gen);
      } else if ((fabs(gen.getPID()) > 0 && fabs(gen.getPID()) < 6) || fabs(gen.getPID()) == 21) {
        all_gen_jets.push_back(gen);
      }
    }

    // only look at di-tau events
    if (all_gen_taus.size() < 2) {
      continue;
    }

    for (auto const &jet : all_gen_jets) {
      // switch gluon pdg ID to be 7
      auto pid = fabs(jet.getPID());
      if (pid == 21) {
        pid = 7;
      }

      // get jet composition at different pTs
      hists->Fill("pt0_jet_flavor", pid, 1.);
      if (jet.getPt() > 400) {
        hists->Fill("pt400_jet_flavor", pid, 1.);
      }
      if (jet.getPt() > 600) {
        hists->Fill("pt600_jet_flavor", pid, 1.);
      }
      if (jet.getPt() > 800) {
        hists->Fill("pt800_jet_flavor", pid, 1.);
      }
    }

    // plot lead pT to do gen jet selection efficiency
    if (gen_jets->size() > 0) {
      hists->FillPrevBins("lead_gen_jet_eff", gen_jets->at(0).getPt(), 1.);
    }

    // again with gen jets
    if (all_gen_jets.size() > 0) {
      hists->FillPrevBins("lead_jet_eff", all_gen_jets.at(0).getPt(), 1.);
    }

    // require high pT lead jet
    if (all_gen_jets.size() == 0 || all_gen_jets.at(0).getPt() < 400) {
      continue;
    }

    // fill some histograms
    hists->Fill("lead_jet_flavor", all_gen_jets.at(0).getPID(), 1.);
    hists->Fill("dr_taus", all_gen_taus.at(0).getP4().DeltaR(all_gen_taus.at(1).getP4()), 1.);
    hists->Fill("dphi_taus", all_gen_taus.at(0).getP4().DeltaPhi(all_gen_taus.at(1).getP4()), 1.);
    hists->Fill("dr_jet_MET", all_gen_jets.at(0).getP4().DeltaR(gen_factory.getMETP4()), 1.);
    if (all_gen_jets.size() > 0) {
      hists->Fill("dr_higgs_MET", all_gen_higgs.at(0).getP4().DeltaR(gen_factory.getMETP4()), 1.);
    }
  }

  std::cout << std::endl;
  fin->Close();
  hists->Write();
  return 1;
}
