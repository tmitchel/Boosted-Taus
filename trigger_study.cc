// Copyright [2019] Tyler Mitchell

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>
#include "TFile.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "TH1F.h"
#include "interface/CLParser.h"
#include "interface/boosted_factory.h"
#include "interface/event_factory.h"
#include "interface/gen_factory.h"
#include "interface/histManager.h"
#include "interface/jets_factory.h"
#include "interface/muon_factory.h"

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
  // construct our object factories
  auto gen_factory = Gen_Factory(tree);
  auto boost_factory = Boosted_Factory(tree);
  auto jet_factory = Jets_Factory(tree);
  auto muon_factory = Muon_Factory(tree);
  auto event = Event_Factory(tree);
  auto nevt_hist = reinterpret_cast<TH1F *>(fin->Get("hEvents"));

  std::string sample_name = input_name.substr(input_name.rfind("/") + 1, std::string::npos);
  sample_name = sample_name.substr(0, sample_name.rfind(".root"));
  double evtwt = lumi["2017"] * cross_sections[sample_name] / nevt_hist->Integral();

  auto nevts = tree->GetEntries();
  int progress(0), fraction((nevts - 1) / 10);
  for (auto i = 0; i < nevts; i++) {
    tree->GetEntry(i);
    if (i == progress * fraction && verbose) {
      std::cout << "\t" << progress * 10 << "% complete. (" << nevts << " total events)\r" << std::flush;
      progress++;
    }

    // run all the factories to fill variables
    gen_factory.Run_Factory();
    boost_factory.Run_Factory();
    jet_factory.Run_Factory();
    muon_factory.Run_Factory();
    event.Run_Factory();
    auto gens = gen_factory.getGens();
    auto boosts = boost_factory.getTaus();
    auto jets = jet_factory.getJets();
    auto muons = muon_factory.getMuons();

    /////////////////////
    // Event Selection //
    /////////////////////

    // only look at mutau channel
    int taus_to_muons(0);
    for (auto& gen : *gens) {
      if (fabs(gen.getPID()) == 13 && fabs(gen.getMomPID()) == 15) {
        taus_to_muons++;
      }
    }

    // now make the cut
    if (taus_to_muons != 1) {
      continue;
    }

    if (boosts->size() < 2) {
      continue;
    }

    // get our Z boson
    double best_mass(-999);
    TLorentzVector z_boson, z_muon, z_tau;
    for (auto& mu : *muons) {
      for (auto& tau : *boosts) {
        auto m4 = mu.getP4();
        auto t4 = tau.getP4();
        if (mu.getCharge() * tau.getCharge() > 0 || m4.DeltaR(t4) < 0.02) {
          continue;
        }
        // pick pair based on closest match to Z mass
        if (65. - (m4 + t4).M() < 65. - best_mass && (m4 + t4).M() < 65. && m4.DeltaR(t4) < 1.) {
          best_mass = (m4 + t4).M();
          z_boson = (m4 + t4);
          z_muon = m4;
          z_tau = t4;
        }
      }
    }

    // make sure Z and high pT jet are back-to-back
    auto lead_jet = jets->at(0);
    if (lead_jet.getP4().DeltaR(z_boson) < 2.5) {
      continue;
    }

    // make sure our lead muon is actually a muon
    // same for the tau
    bool is_real_muon(false), is_real_tau(false);
    for (auto& gen : *gens) {
      if (fabs(gen.getPID()) == 13 && gen.getP4().DeltaR(z_muon) < 0.5) {
        is_real_muon = true;
      } else if (fabs(gen.getPID()) == 15 && gen.getP4().DeltaR(z_tau) < 0.5) {
        is_real_tau = true;
      }
    }

    if (!is_real_muon || !is_real_tau) {
      continue;
    }

    //////////////
    // Plotting //
    //////////////
    hists->FillBin("triggers", 1, evtwt);

    auto idx(2);
    std::vector<int> trigger_bits = {37, 38, 39, 40};
    for (auto i = 0; i < trigger_bits.size(); i++) {
      if (event.getJetTrigger(trigger_bits.at(i))) {
        hists->FillBin("triggers", idx, evtwt);
      }
      ++idx;
    }

    for (auto i = 0; i < trigger_bits.size(); i++) {
      for (auto j = i + 1; j < trigger_bits.size(); j++) {
        if (event.getJetTrigger(trigger_bits.at(i)) || event.getJetTrigger(trigger_bits.at(j))) {
          hists->FillBin("triggers", idx, evtwt);
        }
        ++idx;
      }
    }

    if (event.getJetTrigger(37) || event.getJetTrigger(38) || event.getJetTrigger(39)) {
      hists->FillBin("triggers", idx, evtwt);
    }
    ++idx;

    if (event.getJetTrigger(37) || event.getJetTrigger(38) || event.getJetTrigger(40)) {
      hists->FillBin("triggers", idx, evtwt);
    }
    ++idx;

    if (event.getJetTrigger(37) || event.getJetTrigger(39) || event.getJetTrigger(40)) {
      hists->FillBin("triggers", idx, evtwt);
    }
    ++idx;

    if (event.getJetTrigger(38) || event.getJetTrigger(39) || event.getJetTrigger(40)) {
      hists->FillBin("triggers", idx, evtwt);
    }
    ++idx;

    if (event.getJetTrigger(37) || event.getJetTrigger(38) || event.getJetTrigger(39) || event.getJetTrigger(40)) {
      hists->FillBin("triggers", idx, evtwt);
    }
  }

  if (verbose) {
    std::cout << std::endl;
  }
  fin->Close();
  hists->Write();
  return 1;
}
