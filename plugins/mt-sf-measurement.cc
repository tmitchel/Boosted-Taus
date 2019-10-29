// Copyright [2019] Tyler Mitchell

#include <fstream>
#include <iostream>

#include "TTree.h"

// utilities
#include "../interface/CLParser.h"
#include "../interface/histManager.h"

// Objects
#include "../interface/boosted_factory.h"
#include "../interface/electron_factory.h"
#include "../interface/event_factory.h"
#include "../interface/jets_factory.h"
#include "../interface/muon_factory.h"
#include "../interface/util.h"

using std::string;
using std::vector;

int find_loose_muons(std::shared_ptr<VMuon>);
VMuon get_signal_muons(std::shared_ptr<VMuon>);
VMuon get_antiid_muons(std::shared_ptr<VMuon>);
VBoosted get_signal_taus(std::shared_ptr<VBoosted>);
VMuon get_control_muons(std::shared_ptr<VMuon>);
VMuon get_antiid_control_muons(std::shared_ptr<VMuon>);
bool pass_electron_veto(std::shared_ptr<VElectron>);

int main(int argc, char** argv) {
    auto parser = std::unique_ptr<CLParser>(new CLParser(argc, argv));
    auto verbose = parser->Flag("-v");
    auto is_data = parser->Flag("--data");
    auto input_name = parser->Option("-i");
    auto output_name = parser->Option("-o");
    auto tree_name = parser->Option("-t", "ggNtuplizer/EventTree");
    auto histograms = parser->Option("-j", "test.json");

    std::string sample_name = input_name.substr(input_name.rfind("/") + 1, std::string::npos);
    sample_name = sample_name.substr(0, sample_name.rfind(".root"));

    // create the log file
    std::ofstream logfile;
    std::string logname = output_name.substr(0, sample_name.rfind(".root")) + ".log";
    logfile.open(logname , std::ios::out | std::ios::trunc);
    logfile << "Processing file: " << input_name << std::endl;
    logfile << "Using options:" << std::endl;
    logfile << "\t is_data:     " << is_data << std::endl;
    logfile << "\t output_name: " << output_name << std::endl;
    logfile << "\t tree_name:   " << tree_name << std::endl;
    logfile << "\t histograms:  " << histograms << std::endl;

    // read the input TFile/TTree
    auto fin = std::shared_ptr<TFile>(TFile::Open(input_name.c_str()));
    auto hists = std::make_shared<histManager>(output_name);
    hists->load_histograms(histograms);
    auto tree = reinterpret_cast<TTree*>(fin->Get(tree_name.c_str()));

    // construct our object factories
    auto boost_factory = Boosted_Factory(tree);
    auto jet_factory = Jets_Factory(tree, is_data);
    auto muon_factory = Muon_Factory(tree);
    auto electron_factory = Electron_Factory(tree);
    auto event = Event_Factory(tree);
    auto nevt_hist = reinterpret_cast<TH1F*>(fin->Get("hcount"));
    auto cross_section = cross_sections[sample_name];
    Double_t init_weight(cross_section * lumi["2017"] / nevt_hist->GetBinContent(2));
    if (is_data) {
      init_weight = 1.;
    }

    auto nevts = tree->GetEntries();
    int progress(0), fraction((nevts - 1) / 10);
    for (auto i = 0; i < nevts; i++) {
        tree->GetEntry(i);
        if (i == progress * fraction && verbose) {
            logfile << "\t" << progress * 10 << "% complete. (" << nevts << " total events)" << std::endl;
            progress++;
        }

        auto evtwt = init_weight;
        boost_factory.Run_Factory();
        jet_factory.Run_Factory();
        muon_factory.Run_Factory();
        electron_factory.Run_Factory();
        event.Run_Factory();

        /////////////////////////
        // Begin pre-selection //
        /////////////////////////
        if (event.getLepTrigger(19) || event.getLepTrigger(20)) {  // HLT_IsoMu24_v
            hists->Fill("cutflow", 1., evtwt);
        } else {
            continue;
        }

        if (pass_electron_veto(electron_factory.getElectrons())) {  // found electrons in the event
            hists->Fill("cutflow", 2., evtwt);
        } else {
            continue;
        }

        if (jet_factory.getNBTags() > 0) {  // b-jet veto
            hists->Fill("cutflow", 3., evtwt);
        } else {
            continue;
        }

        auto muons = muon_factory.getMuons();
        auto taus = boost_factory.getTaus();

        //////////////////////////////
        // Begin Zmumu CR selection //
        //////////////////////////////

        // get necessary objects
        auto nloose = find_loose_muons(muons);
        if (nloose <= 2) {
            hists->Fill("cutflow", 4., evtwt);
        } else {
            continue;
        }

        // get passing/failing pairs
        auto good_muon_pair = get_control_muons(muons);
        auto anti_muon_pair = get_antiid_control_muons(muons);

        // passing region
        if (good_muon_pair.size() == 2) {
            if (good_muon_pair.at(0).getCharge() * good_muon_pair.at(1).getCharge() < 0) {
                hists->Fill("OS_control", (good_muon_pair.at(0).getP4() + good_muon_pair.at(1).getP4()).M(), evtwt);
            } else {
                hists->Fill("SS_control", (good_muon_pair.at(0).getP4() + good_muon_pair.at(1).getP4()).M(), evtwt);
            }
        } else if (anti_muon_pair.size() == 2) {
            if (anti_muon_pair.at(0).getCharge() * anti_muon_pair.at(1).getCharge() < 0) {
                hists->Fill("OS_anti_control", (anti_muon_pair.at(0).getP4() + anti_muon_pair.at(1).getP4()).M(), evtwt);
            } else {
                hists->Fill("SS_anti_control", (anti_muon_pair.at(0).getP4() + anti_muon_pair.at(1).getP4()).M(), evtwt);
            }
        }

        ///////////////////////////////////
        // Begin signal region selection //
        ///////////////////////////////////

        if (nloose <= 1) {
            hists->Fill("cutflow", 5., evtwt);
        } else {
            continue;
        }

        auto good_muons = get_signal_muons(muons);
        auto anti_muons = get_antiid_muons(muons);
        auto good_taus = get_signal_taus(taus);

        // check if we find a good tau
        if (good_taus.size() > 0) {
            hists->Fill("cutflow", 6., evtwt);
        } else {
            continue;
        }

        // check if we found a good passing or failing muon
        if (good_muons.size() > 0 || anti_muons.size() > 0) {
            hists->Fill("cutflow", 7., evtwt);
        } else {
            continue;
        }

        bool good_match(false), anti_match(false);
        Muon good_muon, anti_muon;
        Boosted good_tau, anti_tau;
        for (auto& tau : good_taus) {
            for (auto& mu : good_muons) {
                if (mu.getP4().DeltaR(tau.getP4()) > 0.4 && mu.getP4().DeltaR(tau.getP4()) < 0.8) {
                    good_match = true;
                    good_muon = mu;
                    good_tau = tau;
                    break;
                }
            }
            for (auto& mu : anti_muons) {
                if (mu.getP4().DeltaR(tau.getP4()) > 0.4 && mu.getP4().DeltaR(tau.getP4()) < 0.8) {
                    anti_match = true;
                    anti_muon = mu;
                    anti_tau = tau;
                    break;
                }
            }
        }

        if (good_match || anti_match) {
            hists->Fill("cutflow", 8., evtwt);
        } else {
            continue;
        }

        // get the 4-vectors
        auto mu_vector(good_muon.getP4()), anti_vector(anti_muon.getP4());
        auto tau_vector(good_tau.getP4()), atau_vector(anti_tau.getP4());

        // construct pass-id signal region
        if (good_match) {
            hists->Fill("cutflow", 9., evtwt);
            if (good_tau.getIso(medium)) {  // tau pass region
                hists->Fill("cutflow", 10., evtwt);
                if (good_muon.getCharge() * good_tau.getCharge() < 0) {
                    hists->Fill("OS_pass", (mu_vector + tau_vector).M(), evtwt);
                } else {
                    hists->Fill("SS_pass", (mu_vector + tau_vector).M(), evtwt);
                }
            } else if (good_tau.getIso(vloose)) {
                if (good_muon.getCharge() * good_tau.getCharge() < 0) {  // tau fail region
                    hists->Fill("OS_fail", (mu_vector + tau_vector).M(), evtwt);
                } else {
                    hists->Fill("SS_fail", (mu_vector + tau_vector).M(), evtwt);
                }
            }
        }

        // construct anti-id signal region
        if (anti_match) {
            hists->Fill("cutflow", 9., evtwt);
            if (good_tau.getIso(medium)) {  // tau pass region
                hists->Fill("cutflow", 10., evtwt);
                if (anti_muon.getCharge() * good_tau.getCharge() < 0) {
                    hists->Fill("OS_anti_pass", (anti_vector + atau_vector).M(), evtwt);
                } else {
                    hists->Fill("SS_anti_pass", (anti_vector + atau_vector).M(), evtwt);
                }
            } else if (good_tau.getIso(vloose)) {
                if (anti_muon.getCharge() * good_tau.getCharge() < 0) {  // tau fail region
                    hists->Fill("OS_anti_fail", (anti_vector + atau_vector).M(), evtwt);
                } else {
                    hists->Fill("SS_anti_fail", (anti_vector + atau_vector).M(), evtwt);
                }
            }
        }
    }      // end event loop
    fin->Close();
    hists->Write();
    logfile.close();
}

bool pass_electron_veto(std::shared_ptr<VElectron> all_electrons) {
    for (auto el : *all_electrons) {
        if (el.getPt() > 10 && fabs(el.getEta()) < 2.5) {
            return false;  // veto any electrons in the event
        }
    }
    return true;
}

int find_loose_muons(std::shared_ptr<VMuon> all_muons) {
    int loose_muons(0);
    for (auto& mu : *all_muons) {
        if (mu.getPt() > 10 && fabs(mu.getEta()) < 2.4 && mu.getID(medium) && mu.getIsoTrk() < 0.15) {
            loose_muons++;
        }
    }
    return loose_muons;
}

VMuon get_signal_muons(std::shared_ptr<VMuon> all_muons) {
    VMuon good_muons;
    for (auto mu : *all_muons) {
        if (mu.getPt() > 30 && fabs(mu.getEta()) < 2.4 && mu.getID(medium)) {
          good_muons.push_back(mu);
        }
    }
    return good_muons;
}

VMuon get_antiid_muons(std::shared_ptr<VMuon> all_muons) {
    VMuon good_muons;
    for (auto mu : *all_muons) {
        if (mu.getPt() > 30 && fabs(mu.getEta()) < 2.4 && !mu.getID(medium)) {
            good_muons.push_back(mu);
        }
    }
    return good_muons;
}

VBoosted get_signal_taus(std::shared_ptr<VBoosted> all_taus) {
    VBoosted good_taus;
    for (unsigned i = 0; i < all_taus->size(); i++) {
        if (all_taus->at(i).getPt() > 30 && fabs(all_taus->at(i).getEta()) < 2.3
            && all_taus->at(i).getMuRejection(tight) && all_taus->at(i).getEleRejection(vloose)) {
            good_taus.push_back(all_taus->at(i));
        }
    }
    return good_taus;
}

VMuon get_control_muons(std::shared_ptr<VMuon> all_muons) {
    VMuon good_pair;
    for (auto mu : *all_muons) {  // tighter muon
        if (mu.getPt() > 30 && fabs(mu.getEta()) < 2.4 && mu.getID(medium) && mu.getIsoTrk() < 0.15) {
            for (auto mu2 : *all_muons) {  // looser muon
                if (mu2.getPt() > 10 && fabs(mu2.getEta()) < 2.4 && mu2.getID(medium) && mu2.getIsoTrk() < 0.15 &&
                    (mu.getP4() + mu2.getP4()).M() > 60 && (mu.getP4() + mu2.getP4()).M() < 120
                    && mu.getP4().DeltaR(mu2.getP4()) > 0.05 && mu.getP4().DeltaR(mu2.getP4()) < 1.5) {
                    good_pair = {mu, mu2};
                }
            }
        }
    }
    return good_pair;
}

VMuon get_antiid_control_muons(std::shared_ptr<VMuon> all_muons) {
    VMuon good_pair;
    for (auto mu : *all_muons) {  // tighter muon
        if (mu.getPt() > 30 && fabs(mu.getEta()) < 2.4 && mu.getID(medium) && mu.getIsoTrk() < 0.15) {
            for (auto mu2 : *all_muons) {  // looser muon
                if (mu2.getPt() > 10 && fabs(mu2.getEta()) < 2.4 && !(mu2.getID(medium) && mu2.getIsoTrk() < 0.15) &&
                    (mu.getP4() + mu2.getP4()).M() > 60 && (mu.getP4() + mu2.getP4()).M() < 120
                    && mu.getP4().DeltaR(mu2.getP4()) > 0.05 && mu.getP4().DeltaR(mu2.getP4()) < 1.5) {
                    good_pair = {mu, mu2};
                }
            }
        }
    }
    return good_pair;
}
