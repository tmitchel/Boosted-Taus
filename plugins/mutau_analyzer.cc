// Copyright [2019] Tyler Mitchell

#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include "../interface/CLParser.h"
#include "../interface/boosted_factory.h"
#include "../interface/event_factory.h"
#include "../interface/gen_factory.h"
#include "../interface/histManager.h"
#include "../interface/jets_factory.h"
#include "../interface/muon_factory.h"
#include "TFile.h"
#include "TH1F.h"
#include "TLorentzVector.h"
#include "TTree.h"

using std::string;
using std::vector;

int main(int argc, char** argv) {
  auto parser = std::unique_ptr<CLParser>(new CLParser(argc, argv));
  auto verbose = parser->Flag("-v");
  auto is_data = parser->Flag("--data");
  auto input_name = parser->Option("-i");
  auto output_name = parser->Option("-o");
  auto tree_name = parser->Option("-t", "ggNtuplizer/EventTree");
  auto histograms = parser->Option("-j", "test.json");

  // read the input TFile/TTree
  auto fin = std::shared_ptr<TFile>(TFile::Open(input_name.c_str()));
  auto hists = std::make_shared<histManager>(output_name);
  hists->load_histograms(histograms);
  auto tree = reinterpret_cast<TTree*>(fin->Get(tree_name.c_str()));
  // construct our object factories
  auto boost_factory = Boosted_Factory(tree);
  auto gen_factory = Gen_Factory(tree, is_data);
  auto jet_factory = Jets_Factory(tree, is_data);
  auto muon_factory = Muon_Factory(tree);
  auto event = Event_Factory(tree);
  auto nevt_hist = reinterpret_cast<TH1F*>(fin->Get("hcount"));

  std::string sample_name = input_name.substr(input_name.rfind("/") + 1, std::string::npos);
  sample_name = sample_name.substr(0, sample_name.rfind(".root"));
  if (input_name.find("WJetsToLNu_HT-2500ToInf") != std::string::npos) {
    sample_name = "WJetsToLNu_HT-2500ToInf";
  } else if (input_name.find("WJetsToLNu_HT-800To1200") != std::string::npos) {
    sample_name = "WJetsToLNu_HT-800To1200";
  }
  std::cout << sample_name << std::endl;
  double evtwt = lumi["2017"] * cross_sections[sample_name] / nevt_hist->GetBinContent(2);
  if (is_data) {
    evtwt = 1.;
  }

  auto nevts = tree->GetEntries();
  int progress(0), fraction((nevts - 1) / 10);
  for (auto i = 0; i < nevts; i++) {
    tree->GetEntry(i);
    if (i == progress * fraction && verbose) {
      std::cout << "\t" << progress * 10 << "% complete. (" << nevts << " total events)\r" << std::flush;
      progress++;
    }

    hists->FillBin("cutflow", 1., evtwt);

    // run all the factories to fill variables
    boost_factory.Run_Factory();
    gen_factory.Run_Factory();
    jet_factory.Run_Factory();
    muon_factory.Run_Factory();
    event.Run_Factory();
    auto boosts = boost_factory.getTaus();
    auto gens = gen_factory.getGens();
    auto jets = jet_factory.getJets();
    auto muons = muon_factory.getMuons();
    auto met = event.getMET();

    /////////////////////
    // Event Selection //
    /////////////////////

    // // only look at mutau channel
    // int taus_to_muons(0);
    // for (auto& gen : *gens) {
    //   if (fabs(gen.getPID()) == 13 && fabs(gen.getMomPID()) == 15) {
    //     taus_to_muons++;
    //   }
    // }

    // // now make the cut
    // if (taus_to_muons != 1) {
    //   continue;
    // }
    
    // apply trigger
    if (!event.getJetTrigger(39) && !event.getJetTrigger(40)) {
      continue;
    }
    hists->FillBin("cutflow", 2., evtwt);

    // if we have one good reco muon, this is mutau channel
    if (muon_factory.getNGoodMuon() != 1) {
      continue;
    }
    hists->FillBin("cutflow", 3., evtwt);

    // make sure we have 2 boosted taus
    if (boosts->size() < 2) {
      continue;
    }
    hists->FillBin("cutflow", 4., evtwt);

    // get our Z boson
    auto found(false);
    Muon good_muon;
    Boosted good_tau;
    TLorentzVector z_boson, z_muon, z_tau;
    for (auto& mu : *muons) {
      if (found) {
        break;
      }
      for (auto& tau : *boosts) {
        auto m4 = mu.getP4();
        auto t4 = tau.getP4();
        if (m4.DeltaR(t4) < 0.02) {
          continue;
        }
        // pick pair
        if (m4.DeltaR(t4) < 0.8) {
          z_boson = (m4 + t4);
          z_muon = m4;
          z_tau = t4;
          good_muon = mu;
          good_tau = tau;
          found = true;
          break;
        }
      }
    }

    // anti-muon discriminator
    if (!good_tau.getMuRejection(tight)) {
      continue;
    }
    hists->FillBin("cutflow", 5., evtwt);

    // make sure Z and high pT jet are back-to-back
    auto lead_jet = jets->at(0);
    if (lead_jet.getP4().DeltaR(z_boson) < 2.) {
      continue;
    }
    hists->FillBin("cutflow", 6., evtwt);

    // high MET cut
    if (met.Pt() < 100) {
      continue;
    }
    hists->FillBin("cutflow", 7., evtwt);

    ///////////////////////////////
    // Calculate other variables //
    ///////////////////////////////

    // calculate HT, MHT
    double HT(0.);
    TLorentzVector MHT;
    MHT.SetPtEtaPhiM(0, 0, 0, 0);
    for (auto& jet : *jets) {
      HT += jet.getPt();
      MHT += jet.getP4();
    }

    // calculate transverse masses
    auto mt_mutau = transverse_mass({z_muon.Pt(), z_muon.Px(), z_muon.Py()}, {z_tau.Pt(), z_tau.Px(), z_tau.Py()});
    auto mt_mumet = transverse_mass({z_muon.Pt(), z_muon.Px(), z_muon.Py()}, {met.Pt(), met.Px(), met.Py()});
    auto mt_mettau = transverse_mass({met.Pt(), met.Px(), met.Py()}, {z_tau.Pt(), z_tau.Px(), z_tau.Py()});

    /////////////////////////
    // OS/SS Ratio Regions //
    /////////////////////////

    // get shape from Same-Sign tau anti-isolated region
    if (!good_tau.getIso(loose)) {
      if (good_muon.getCharge() == good_tau.getCharge()) {
        hists->Fill("SS_antiiso_ht", HT, evtwt);
        hists->Fill("SS_antiiso_mht", MHT.Pt(), evtwt);
        hists->Fill("SS_antiiso_met", met.Pt(), evtwt);
        hists->Fill("SS_antiiso_mu_pt", z_muon.Pt(), evtwt);
        hists->Fill("SS_antiiso_tau_pt", z_tau.Pt(), evtwt);
        hists->Fill("SS_antiiso_Z_mass", z_boson.M(), evtwt);
        hists->Fill("SS_antiiso_Z_pt", z_boson.Pt(), evtwt);
        hists->Fill("SS_antiiso_mu_tau_dr", z_tau.DeltaR(z_muon), evtwt);
        hists->Fill("SS_antiiso_mt_mutau", mt_mutau, evtwt);
      } else {
        hists->Fill("OS_antiiso_ht", HT, evtwt);
        hists->Fill("OS_antiiso_mht", MHT.Pt(), evtwt);
        hists->Fill("OS_antiiso_met", met.Pt(), evtwt);
        hists->Fill("OS_antiiso_mu_pt", z_muon.Pt(), evtwt);
        hists->Fill("OS_antiiso_tau_pt", z_tau.Pt(), evtwt);
        hists->Fill("OS_antiiso_Z_mass", z_boson.M(), evtwt);
        hists->Fill("OS_antiiso_Z_pt", z_boson.Pt(), evtwt);
        hists->Fill("OS_antiiso_mu_tau_dr", z_tau.DeltaR(z_muon), evtwt);
        hists->Fill("OS_antiiso_mt_mutau", mt_mutau, evtwt);
      }
    } else {  // tau passes loose isolation
      if (!good_muon.getID(2)) {  // muon fails medium ID
        if (good_muon.getCharge() == good_tau.getCharge()) {
          // Same-Sign, anti-ID region
          hists->Fill("SS_antiid_ht", HT, evtwt);
          hists->Fill("SS_antiid_mht", MHT.Pt(), evtwt);
          hists->Fill("SS_antiid_met", met.Pt(), evtwt);
          hists->Fill("SS_antiid_mu_pt", z_muon.Pt(), evtwt);
          hists->Fill("SS_antiid_tau_pt", z_tau.Pt(), evtwt);
          hists->Fill("SS_antiid_Z_mass", z_boson.M(), evtwt);
          hists->Fill("SS_antiid_Z_pt", z_boson.Pt(), evtwt);
          hists->Fill("SS_antiid_mu_tau_dr", z_tau.DeltaR(z_muon), evtwt);
          hists->Fill("SS_antiid_mt_mutau", mt_mutau, evtwt);
        } else {
          // Opposite-Sign, anti-ID region
          hists->Fill("OS_antiid_ht", HT, evtwt);
          hists->Fill("OS_antiid_mht", MHT.Pt(), evtwt);
          hists->Fill("OS_antiid_met", met.Pt(), evtwt);
          hists->Fill("OS_antiid_mu_pt", z_muon.Pt(), evtwt);
          hists->Fill("OS_antiid_tau_pt", z_tau.Pt(), evtwt);
          hists->Fill("OS_antiid_Z_mass", z_boson.M(), evtwt);
          hists->Fill("OS_antiid_Z_pt", z_boson.Pt(), evtwt);
          hists->Fill("OS_antiid_mu_tau_dr", z_tau.DeltaR(z_muon), evtwt);
          hists->Fill("OS_antiid_mt_mutau", mt_mutau, evtwt);
        }
      } else if (good_muon.getID(2)) {  // muon passes medium
        if (good_muon.getCharge() == good_tau.getCharge()) {
          // Same-Sign, ID region
          hists->Fill("SS_id_ht", HT, evtwt);
          hists->Fill("SS_id_mht", MHT.Pt(), evtwt);
          hists->Fill("SS_id_met", met.Pt(), evtwt);
          hists->Fill("SS_id_mu_pt", z_muon.Pt(), evtwt);
          hists->Fill("SS_id_tau_pt", z_tau.Pt(), evtwt);
          hists->Fill("SS_id_Z_mass", z_boson.M(), evtwt);
          hists->Fill("SS_id_Z_pt", z_boson.Pt(), evtwt);
          hists->Fill("SS_id_mu_tau_dr", z_tau.DeltaR(z_muon), evtwt);
          hists->Fill("SS_id_mt_mutau", mt_mutau, evtwt);
        }
      }
    }

    // Now apply the normal muon isolation
    if (!good_muon.getID(2)) {
      continue;
    }
    hists->FillBin("cutflow", 8., evtwt);

    // Now apply opposite-sign selection
    if (good_muon.getCharge() * good_tau.getCharge() > 0) {
      continue;
    }
    hists->FillBin("cutflow", 9., evtwt);

    // apply tau isolation
    if (!good_tau.getIso(loose)) {
      continue;
    }
    hists->FillBin("cutflow", 10., evtwt);

    //////////////
    // Plotting //
    //////////////
    hists->Fill("ht", HT, evtwt);
    hists->Fill("mht", MHT.Pt(), evtwt);
    hists->Fill("met", met.Pt(), evtwt);

    hists->Fill("mu_pt", z_muon.Pt(), evtwt);
    hists->Fill("tau_pt", z_tau.Pt(), evtwt);
    hists->Fill("Z_mass", z_boson.M(), evtwt);
    hists->Fill("Z_pt", z_boson.Pt(), evtwt);
    hists->Fill("mu_tau_dr", z_tau.DeltaR(z_muon), evtwt);
    hists->Fill("mt_mutau", mt_mutau, evtwt);
  }

  if (verbose) {
    std::cout << std::endl;
  }
  fin->Close();
  hists->Write();
  return 1;
}
