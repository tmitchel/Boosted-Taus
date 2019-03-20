// Copyright [2019] Tyler Mitchell

#include <iostream>
#include <memory>
#include <vector>
#include "TFile.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "interface/CLParser.h"
#include "interface/boosted_factory.h"
#include "interface/event_factory.h"
#include "interface/gen_factory.h"
#include "interface/histManager.h"
#include "interface/jets_factory.h"
#include "interface/tau_factory.h"

using std::string;
using std::vector;

int main(int argc, char **argv) {
  auto parser = std::unique_ptr<CLParser>(new CLParser(argc, argv));
  auto verbose = parser->Flag("-v");
  auto input_name = parser->Option("-i");
  auto output_name = parser->Option("-o");
  auto tree_name = parser->Option("-t", "ggNtuplizer/EventTree");

  // read the input TFile/TTree
  auto fin = std::shared_ptr<TFile>(TFile::Open(input_name.c_str()));
  auto hists = std::make_shared<histManager>(output_name);
  auto tree = reinterpret_cast<TTree *>(fin->Get(tree_name.c_str()));
  // construct our object factories
  auto gen_factory = Gen_Factory(tree);
  auto tau_factory = Tau_Factory(tree);
  auto boost_factory = Boosted_Factory(tree);
  auto jet_factory = Jets_Factory(tree);
  auto event = Event_Factory(tree);

  auto nevts = tree->GetEntries();
  int progress(1), fraction((nevts - 1) / 10);
  for (auto i = 0; i < nevts; i++) {
    tree->GetEntry(i);
    if (i == progress * fraction && verbose) {
      std::cout << "\t" << progress * 10 << "% complete.\r" << std::flush;
      progress++;
    }

    // keep track of the number of events processed
    hists->Fill("nevents", 1., 1.);
    hists->FillBin("cutflow", 1, 1.);

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
      hists->Fill("cutflow", 2, 1.);
    } else {
      continue;
    }

    // need to have at least 1 tau in the event
    if (boost_factory.getNGoodBoosted() > 0) {
      hists->Fill("cutflow", 3, 1.);
    } else {
      continue;
    }

    auto lead_tau = boost_factory.getTaus()->at(0);

    /////////////////////
    // trigger studies //
    /////////////////////

    // are triggers prescaled?
    if (event.getJetTriggerIsPre(16)) {
      hists->FillBin("is_prescaled", 1, 1.);
    } else if (event.getJetTriggerIsPre(16)) {
      hists->FillBin("is_prescaled", 2, 1.);
    }

    if (event.getJetTriggerIsPre(19)) {
      hists->FillBin("is_prescaled", 3, 1.);
    } else if (event.getJetTriggerIsPre(19)) {
      hists->FillBin("is_prescaled", 4, 1.);
    }

    if (event.getJetTriggerIsPre(22)) {
      hists->FillBin("is_prescaled", 5, 1.);
    } else if (event.getJetTriggerIsPre(22)) {
      hists->FillBin("is_prescaled", 6, 1.);
    }

    if (event.getJetTriggerIsPre(32)) {
      hists->FillBin("is_prescaled", 7, 1.);
    } else if (event.getJetTriggerIsPre(32)) {
      hists->FillBin("is_prescaled", 8, 1.);
    }

    // trigger efficiencies

    // HLT_PFJet320_v
    if (event.getJetTrigger(16)) {
      hists->FillBin("all_trigger_eff", 1, 1.);
      hists->FillPrevBins("jet320_trigger_eff", lead_tau.getPt(), 1.);
    }

    // HLT_PFJet500_v
    if (event.getJetTrigger(19)) {
      hists->FillBin("all_trigger_eff", 2, 1.);
      hists->FillPrevBins("jet500_trigger_eff", lead_tau.getPt(), 1.);
    }

    // HLT_PFHT300_PFMET110_v
    if (event.getJetTrigger(22)) {
      hists->FillBin("all_trigger_eff", 3, 1.);
      hists->FillPrevBins("HT300_MET110_trigger_eff", lead_tau.getPt(), 1.);
    }

    // HLT_PFHT800_v
    if (event.getJetTrigger(32)) {
      hists->FillBin("all_trigger_eff", 4, 1.);
      hists->FillPrevBins("HT800_trigger_eff", lead_tau.getPt(), 1.);
    }

    ////////////////////////////
    // tau efficiency studies //
    ////////////////////////////

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
          hists->FillBin("ntau", 1, 1.);
          hists->FillPrevBins("all_tau_pt", tau.getPt(), 1.);
          if (tau.getIso(loose)) {
            hists->FillBin("all_iso_eff", 1, 1.);
            hists->FillPrevBins("loose_boosted_eff", tau.getPt(), 1.);
          }
          if (tau.getIso(medium)) {
            hists->FillBin("all_iso_eff", 2, 1.);
            hists->FillPrevBins("medium_boosted_eff", tau.getPt(), 1.);
          }
          if (tau.getIso(tight)) {
            hists->FillBin("all_iso_eff", 3, 1.);
            hists->FillPrevBins("tight_boosted_eff", tau.getPt(), 1.);
          }
        }
      }
    }

    std::cout << std::endl;
    fin->Close();
    hists->Write();
    return 1;
  }
