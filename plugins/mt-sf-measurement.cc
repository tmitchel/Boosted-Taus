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
Muon get_signal_muon(std::shared_ptr<VMuon>);
Muon get_antiid_muon(std::shared_ptr<VMuon>);
Boosted get_signal_tau(std::shared_ptr<VBoosted>);
vector<Muon> get_control_muons(std::shared_ptr<VMuon>);
vector<Muon> get_antiid_control_muons(std::shared_ptr<VMuon>);
bool pass_electron_veto(std::shared_ptr<VElectron>);

int main(int argc, char** argv) {
    auto parser = std::unique_ptr<CLParser>(new CLParser(argc, argv));
    auto verbose = parser->Flag("-v");
    auto is_data = parser->Flag("--data");
    auto input_name = parser->Option("-i");
    auto output_name = parser->Option("-o");
    auto tree_name = parser->Option("-t", "ggNtuplizer/EventTree");
    auto histograms = parser->Option("-j", "test.json");
    auto logname = parser->Option("-l");

    std::string sample_name = input_name.substr(input_name.rfind("/") + 1, std::string::npos);
    sample_name = sample_name.substr(0, sample_name.rfind(".root"));

    // create the log file
    std::ofstream logfile;
    logfile.open(logname, std::ios::out | std::ios::trunc);
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

        if (!event.getLepTrigger(21)) {  // Mu50 trigger
            continue;
        }

        // general selection
        if (!pass_electron_veto(electron_factory.getElectrons())) {
            continue;  // found electrons in the event
        }

        if (jet_factory.getNBTags() > 0) {  // TODO(tmitchel): implement
            continue;                       // b-jet veto
        }

        auto muons = muon_factory.getMuons();
        auto taus = boost_factory.getTaus();

        // get necessary objects
        auto nloose = find_loose_muons(muons);
        auto good_muon = get_signal_muon(muons);
        auto anti_muon = get_antiid_muon(muons);
        auto good_tau = get_signal_tau(taus);
        auto good_muon_pair = get_control_muons(muons);
        auto anti_muon_pair = get_antiid_control_muons(muons);

        // construct signal region
        if (nloose < 2 && good_muon.getPt() > 0 && good_tau.getPt() > 0 && good_muon.getP4().DeltaR(good_tau.getP4()) > 0.1 &&
            good_muon.getP4().DeltaR(good_tau.getP4()) < 0.8) {
            if (good_tau.getIso(medium)) {
                // tau pass region
                if (good_muon.getCharge() * good_tau.getCharge() < 0) {
                    hists->Fill("OS_pass", (good_muon.getP4() + good_tau.getP4()).M(), evtwt);
                } else {
                    hists->Fill("SS_pass", (good_muon.getP4() + good_tau.getP4()).M(), evtwt);
                }
            } else if (good_tau.getIso(vloose)) {
                // tau fail region
                if (good_muon.getCharge() * good_tau.getCharge() < 0) {
                    hists->Fill("OS_fail", (good_muon.getP4() + good_tau.getP4()).M(), evtwt);
                } else {
                    hists->Fill("SS_fail", (good_muon.getP4() + good_tau.getP4()).M(), evtwt);
                }
            }
        }  // close signal region

        // construct control region
        if (nloose < 3 && good_muon_pair.size() == 2) {
            if (good_muon_pair.at(0).getCharge() * good_muon_pair.at(1).getCharge() < 0) {
                hists->Fill("OS_control", (good_muon_pair.at(0).getP4() + good_muon_pair.at(1).getP4()).M(), evtwt);
            } else {
                hists->Fill("SS_control", (good_muon_pair.at(0).getP4() + good_muon_pair.at(1).getP4()).M(), evtwt);
            }
        }  // close control region

        // construct anti-id signal region
        if (nloose < 2 && anti_muon.getPt() > 0 && good_tau.getPt() > 0 && anti_muon.getP4().DeltaR(good_tau.getP4()) > 0.1 &&
            anti_muon.getP4().DeltaR(good_tau.getP4()) < 0.8) {
            if (good_tau.getIso(medium)) {
                // tau pass region
                if (anti_muon.getCharge() * good_tau.getCharge() < 0) {
                    hists->Fill("OS_anti_pass", (anti_muon.getP4() + good_tau.getP4()).M(), evtwt);
                } else {
                    hists->Fill("SS_anti_pass", (anti_muon.getP4() + good_tau.getP4()).M(), evtwt);
                }
            } else if (good_tau.getIso(vloose)) {
                // tau fail region
                if (anti_muon.getCharge() * good_tau.getCharge() < 0) {
                    hists->Fill("OS_anti_fail", (anti_muon.getP4() + good_tau.getP4()).M(), evtwt);
                } else {
                    hists->Fill("SS_anti_fail", (anti_muon.getP4() + good_tau.getP4()).M(), evtwt);
                }
            }
        }  // close anti-id signal region

        // construct anti-id control region
        if (nloose < 3 && anti_muon_pair.size() == 2) {
            if (anti_muon_pair.at(0).getCharge() * anti_muon_pair.at(1).getCharge() < 0) {
                hists->Fill("OS_anti_control", (anti_muon_pair.at(0).getP4() + anti_muon_pair.at(1).getP4()).M(), evtwt);
            } else {
                hists->Fill("SS_anti_control", (anti_muon_pair.at(0).getP4() + anti_muon_pair.at(1).getP4()).M(), evtwt);
            }
        }  // close anti-id control region
    }      // end event loop
    fin->Close();
    hists->Write();
    logfile.close();
}

bool pass_electron_veto(std::shared_ptr<VElectron> all_electrons) {
    for (auto el : *all_electrons) {
        if (el.getPt() > 10 && fabs(el.getEta()) < 2.1) {
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

Muon get_signal_muon(std::shared_ptr<VMuon> all_muons) {
    for (auto mu : *all_muons) {
        if (mu.getPt() > 55 && fabs(mu.getEta()) < 2.4 && mu.getID(medium)) {
            return mu;
        }
    }
    return Muon(0, 0, 0, 0);
}

Muon get_antiid_muon(std::shared_ptr<VMuon> all_muons) {
    for (auto mu : *all_muons) {
        if (mu.getPt() > 55 && fabs(mu.getEta()) < 2.4 && !mu.getID(medium)) {
            return mu;
        }
    }
    return Muon(0, 0, 0, 0);
}

Boosted get_signal_tau(std::shared_ptr<VBoosted> all_taus) {
    for (unsigned i = 0; i < all_taus->size(); i++) {
        if (all_taus->at(i).getPt() > 30 && fabs(all_taus->at(i).getEta()) < 2.3) {
            return all_taus->at(i);
        }
    }
    return Boosted(0, 0, 0, 0);
}

vector<Muon> get_control_muons(std::shared_ptr<VMuon> all_muons) {
    vector<Muon> good_pair;
    for (auto mu : *all_muons) {  // tighter muon
        if (mu.getPt() > 50 && fabs(mu.getEta()) < 2.4 && mu.getID(medium) && mu.getIsoTrk() < 0.15) {
            for (auto mu2 : *all_muons) {  // looser muon
                if (mu2.getPt() > 10 && fabs(mu2.getEta()) < 2.4 && mu2.getID(medium) && mu2.getIsoTrk() < 0.15 &&
                    (mu.getP4() + mu2.getP4()).M() > 60 && (mu.getP4() + mu2.getP4()).M() < 120) {
                    good_pair = {mu, mu2};
                }
            }
        }
    }
    return good_pair;
}

vector<Muon> get_antiid_control_muons(std::shared_ptr<VMuon> all_muons) {
    vector<Muon> good_pair;
    for (auto mu : *all_muons) {  // tighter muon
        if (mu.getPt() > 50 && fabs(mu.getEta()) < 2.4 && mu.getID(medium) && mu.getIsoTrk() < 0.15) {
            for (auto mu2 : *all_muons) {  // looser muon
                if (mu2.getPt() > 10 && fabs(mu2.getEta()) < 2.4 && !(mu2.getID(medium) && mu2.getIsoTrk() < 0.15) &&
                    (mu.getP4() + mu2.getP4()).M() > 60 && (mu.getP4() + mu2.getP4()).M() < 120) {
                    good_pair = {mu, mu2};
                }
            }
        }
    }
    return good_pair;
}
