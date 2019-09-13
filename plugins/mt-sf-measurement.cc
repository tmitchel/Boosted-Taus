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

using std::string;
using std::vector;

Muon* get_signal_muon(std::shared_ptr<VMuon>);
Muon* get_antiid_muon(std::shared_ptr<VMuon>);
Boosted* get_signal_tau(std::shared_ptr<VBoosted>);
vector<Muon*> get_control_muons(std::shared_ptr<VMuon>);
vector<Muon*> get_antiid_control_muons(std::shared_ptr<VMuon>);
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

    // create the log file
    std::ofstream logfile;
    logfile.open(logname, std::ios::out | std::ios::trunc);
    logfile << "Processing file: " << input_name <<std::endl;
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

    Double_t evtwt(1.);

    auto nevts = tree->GetEntries();
    int progress(0), fraction((nevts - 1) / 10);
    for (auto i = 0; i < nevts; i++) {
        tree->GetEntry(i);
        if (i == progress * fraction && verbose) {
            logfile << "\t" << progress * 10 << "% complete. (" << nevts << " total events)" << std::endl;
            progress++;
        }

        evtwt = 1.;
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

        // get necessary objects
        auto good_muon = get_signal_muon(muon_factory.getMuons());
        auto anti_muon = get_antiid_muon(muon_factory.getMuons());
        auto good_tau = get_signal_tau(boost_factory.getTaus());
        auto good_muon_pair = get_control_muons(muon_factory.getMuons());
        auto anti_muon_pair = get_antiid_control_muons(muon_factory.getMuons());

        // construct signal region
        if (good_muon != nullptr
            && good_muon->getP4().DeltaR(good_tau->getP4()) > 0.1
            && good_muon->getP4().DeltaR(good_tau->getP4()) < 0.8) {
            if (good_tau->getIso(medium)) {
                // tau pass region
                if (good_muon->getCharge() * good_tau->getCharge() < 0) {
                    hists->Fill("OS_pass", (good_muon->getP4() + good_tau->getP4()).M(), evtwt);
                } else {
                    hists->Fill("SS_pass", (good_muon->getP4() + good_tau->getP4()).M(), evtwt);
                }
            } else if (good_tau->getIso(vloose)) {
                // tau fail region
                if (good_muon->getCharge() * good_tau->getCharge() < 0) {
                    hists->Fill("OS_fail", (good_muon->getP4() + good_tau->getP4()).M(), evtwt);
                } else {
                    hists->Fill("SS_fail", (good_muon->getP4() + good_tau->getP4()).M(), evtwt);
                }
            }
        }  // close signal region

        // construct control region
        if (good_muon_pair.size() == 2) {
            if (good_muon_pair.at(0)->getCharge() * good_muon_pair.at(0)->getCharge() < 0) {
                hists->Fill("OS_control", (good_muon_pair.at(0)->getP4() + good_muon_pair.at(1)->getP4()).M(), evtwt);
            } else {
                hists->Fill("SS_control", (good_muon_pair.at(0)->getP4() + good_muon_pair.at(1)->getP4()).M(), evtwt);
            }
        }  // close control region

        // construct anti-id signal region
        if (anti_muon->getPt() > 0 && good_tau->getPt() > 0
            && anti_muon->getP4().DeltaR(good_tau->getP4()) > 0.1
            && anti_muon->getP4().DeltaR(good_tau->getP4()) < 0.8) {
            if (good_tau->getIso(medium)) {
                // tau pass region
                if (anti_muon->getCharge() * good_tau->getCharge() < 0) {
                    hists->Fill("OS_anti_pass", (anti_muon->getP4() + good_tau->getP4()).M(), evtwt);
                } else {
                    hists->Fill("SS_anti_pass", (anti_muon->getP4() + good_tau->getP4()).M(), evtwt);
                }
            } else if (good_tau->getIso(vloose)) {
                // tau fail region
                if (anti_muon->getCharge() * good_tau->getCharge() < 0) {
                    hists->Fill("OS_anti_fail", (anti_muon->getP4() + good_tau->getP4()).M(), evtwt);
                } else {
                    hists->Fill("SS_anti_fail", (anti_muon->getP4() + good_tau->getP4()).M(), evtwt);
                }
            }
        }  // close anti-id signal region

        // construct anti-id control region
        if (anti_muon_pair.size() == 2) {
            if (anti_muon_pair.at(0)->getCharge() * anti_muon_pair.at(0)->getCharge() < 0) {
                hists->Fill("OS_anti_control", (anti_muon_pair.at(0)->getP4() + anti_muon_pair.at(1)->getP4()).M(), evtwt);
            } else {
                hists->Fill("SS_anti_control", (anti_muon_pair.at(0)->getP4() + anti_muon_pair.at(1)->getP4()).M(), evtwt);
            }
        }  // close anti-id control region
    }      // end event loop
}

bool pass_electron_veto(std::shared_ptr<VElectron> all_electrons) {
    for (auto el : *all_electrons) {
        if (el.getPt() > 10 && fabs(el.getEta()) < 2.1) {
            return false;  // veto any electrons in the event
        }
    }
    return true;
}

Muon* get_signal_muon(std::shared_ptr<VMuon> all_muons) {
    int loose_muons(0);
    Muon *mu, *temp;
    for (unsigned i = 0; i < all_muons->size(); i++) {
        if (loose_muons > 1) {  // exactly 1 muon in the event
            return nullptr;
        }

        temp = &all_muons->at(i);
        if (temp->getPt() > 55 && fabs(temp->getEta()) < 2.4 && temp->getID(medium)) {  // good muon
            loose_muons++;
            mu = &all_muons->at(i);
        } else if (temp->getPt() > 10 && fabs(temp->getEta()) < 2.4 && temp->getID(medium) && temp->getIsoTrk() < 0.15) {  // extra muons
            loose_muons++;
        }
    }

    // if there were multiple muons, reset the good muon
    if (loose_muons > 1) {
        return nullptr;
    }
    return mu;
}

Muon* get_antiid_muon(std::shared_ptr<VMuon> all_muons) {
    int loose_muons(0);
    Muon* good_muon;
    for (Muon* mu : *all_muons) {
        if (loose_muons > 1) {  // exactly 1 muon in the event
            return nullptr;
        }

        if (mu.getPt() > 55 && fabs(mu.getEta()) < 2.4 && !mu.getID(medium)) {  // good muon
            loose_muons++;
            good_muon = &mu;
        } else if (mu.getPt() > 10 && fabs(mu.getEta()) < 2.4 && !(mu.getID(medium) && mu.getIsoTrk() < 0.15)) {  // extra muons
            loose_muons++;
        }
    }

    // if there were multiple muons, reset the good muon
    if (loose_muons > 1) {
        good_muon = Muon(0, 0, 0, 0);
    }
    return &good_muon;
}

Boosted* get_signal_tau(std::shared_ptr<VBoosted> all_taus) {
    int loose_taus(0);
    Boosted good_tau(0, 0, 0, 0);
    for (auto tau : *all_taus) {
        if (tau.getPt() > 30 && fabs(tau.getEta()) < 2.3) {
            good_tau = tau;
            loose_taus++;
        }
    }
    return &good_tau;
}

vector<Muon*> get_control_muons(std::shared_ptr<VMuon> all_muons) {
    int loose_muons(0);
    vector<Muon*> good_pair;
    for (auto mu : *all_muons) {  // tighter muon
        if (loose_muons > 2) {    // veto on 3+ muons in event
            break;
        }

        // keep track of number of muons
        if (mu.getPt() > 10 && fabs(mu.getEta()) < 2.4 && mu.getID(medium) && mu.getIsoTrk() < 0.15) {
            loose_muons++;
        }

        // find a good match
        if (mu.getPt() > 50 && fabs(mu.getEta()) < 2.4 && mu.getID(medium) && mu.getIsoTrk() < 0.15) {
            for (auto mu2 : *all_muons) {  // looser muon
                if (mu2.getPt() > 10 && fabs(mu2.getEta()) < 2.4 && mu2.getID(medium) && mu2.getIsoTrk() < 0.15 &&
                    (mu.getP4() + mu2.getP4()).M() > 60 && (mu.getP4() + mu2.getP4()).M() < 120) {
                    good_pair = {&mu, &mu2};
                }
            }
        }
    }
    return good_pair;
}

vector<Muon*> get_antiid_control_muons(std::shared_ptr<VMuon> all_muons) {
    int loose_muons(0);
    vector<Muon*> good_pair;
    for (auto mu : *all_muons) {  // tighter muon
        if (loose_muons > 2) {    // veto on 3+ muons in event
            break;
        }

        // keep track of number of muons
        if (mu.getPt() > 10 && fabs(mu.getEta()) < 2.4 && !(mu.getID(medium) && mu.getIsoTrk() < 0.15)) {
            loose_muons++;
        }

        // find a good match
        if (mu.getPt() > 50 && fabs(mu.getEta()) < 2.4 && mu.getID(medium) && mu.getIsoTrk() < 0.15) {
            for (auto mu2 : *all_muons) {  // looser muon
                if (mu2.getPt() > 10 && fabs(mu2.getEta()) < 2.4 && !(mu2.getID(medium) && mu2.getIsoTrk() < 0.15) &&
                    (mu.getP4() + mu2.getP4()).M() > 60 && (mu.getP4() + mu2.getP4()).M() < 120) {
                    good_pair = {&mu, &mu2};
                }
            }
        }
    }
    return good_pair;
}
