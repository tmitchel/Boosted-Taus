// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_BOOSTED_FACTORY_H_
#define INTERFACE_BOOSTED_FACTORY_H_

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include "./util.h"
#include "TLorentzVector.h"
#include "TTree.h"

class Boosted;  // Boosted combines all boosted tau information
typedef std::vector<Boosted> VBoosted;

// Boosted combines all boosted tau related information along with
// creating the boosted tau 4-vector. Helper functions are provided
// to access properties of the boosted taus. These properties cannot
// be updated.
class Boosted {
    friend class Boosted_Factory;  // Boosted_Factory constructs individual Boosteds

   public:
    Boosted(Float_t _pt, Float_t _eta, Float_t _phi, Float_t _m) { this->p4.SetPtEtaPhiM(_pt, _eta, _phi, _m); }
    Boosted() { this->p4.SetPtEtaPhiE(0, 0, 0, 0); }

    // getters
    TLorentzVector getP4() { return p4; }
    Float_t getPt() { return p4.Pt(); }
    Float_t getEta() { return p4.Eta(); }
    Float_t getPhi() { return p4.Phi(); }
    Float_t getMass() { return p4.M(); }
    Float_t getIsoRaw() { return Iso; }
    Bool_t getIso(working_point);
    Bool_t getDiscByDM(bool);
    Bool_t getEleRejection(working_point);
    Bool_t getMuRejection(working_point);
    Int_t getDecayMode() { return DecayMode; }
    Float_t getCharge() { return Charge; }
    Float_t getDZ() { return dz; }
    Float_t getDXY() { return dxy; }

   private:
    // these should never be modified once read from the TTree
    TLorentzVector p4;
    Bool_t pass_vloose_iso, pass_loose_iso, pass_medium_iso, pass_tight_iso, pass_vtight_iso;

    Bool_t pfTausDiscriminationByDecayModeFinding, pfTausDiscriminationByDecayModeFindingNewDMs, ByMVA6VLooseElectronRejection,
        ByMVA6LooseElectronRejection, ByMVA6MediumElectronRejection, ByMVA6TightElectronRejection, ByMVA6VTightElectronRejection,
        ByLooseMuonRejection3, ByTightMuonRejection3;

    Int_t DecayMode;

    Float_t Charge, Mass, dz, dxy, Energy, Iso;
};

// Boosted_Factory reads from the TTree and constructs
// individual Boosted objects. The Boosted_Factory holds the
// list of boosted taus, but can provided a shared_ptr to the
// list.
class Boosted_Factory {
   public:
    explicit Boosted_Factory(TTree *, std::string);
    void Run_Factory();

    // getters
    Int_t getNTotalBoosted() { return nBoostedTau; }  // nBoostedTau directly from TTree
    Int_t getNGoodBoosted() { return nGoodTaus; }     // nBoostedTau passing preselection
    std::shared_ptr<VBoosted> getTaus() { return std::make_shared<VBoosted>(boosteds); }

   private:
    Int_t nBoostedTau, nGoodTaus;
    VBoosted boosteds;

    std::vector<Float_t> *boostedTauPt, *boostedTauEta, *boostedTauPhi, *boostedTauMass, *iso, *boostedTaudz, *boostedTaudxy, *boostedTauCharge;

    std::vector<Bool_t> *pass_vloose_iso, *pass_loose_iso, *pass_medium_iso, *pass_tight_iso, *pass_vtight_iso;

    std::vector<Bool_t> *boostedTaupfTausDiscriminationByDecayModeFinding, *boostedTaupfTausDiscriminationByDecayModeFindingNewDMs,
        *boostedTauByMVA6VLooseElectronRejection, *boostedTauByMVA6LooseElectronRejection, *boostedTauByMVA6MediumElectronRejection,
        *boostedTauByMVA6TightElectronRejection, *boostedTauByMVA6VTightElectronRejection, *boostedTauByLooseMuonRejection3,
        *boostedTauByTightMuonRejection3;

    std::vector<Int_t> *boostedTauDecayMode;
};

// Set all branch addresses when constructing a Boosted_Factory.
// The type of tau isolation can be chosen and defaults to
// MVArun2v2DBOldDMwLT.
Boosted_Factory::Boosted_Factory(TTree *tree, std::string isoType = "IsolationMVArun2v2DBoldDMwLT")
    : boostedTauPt(nullptr),
      boostedTauEta(nullptr),
      boostedTauPhi(nullptr),
      boostedTauMass(nullptr),
      iso(nullptr),
      pass_vloose_iso(nullptr),
      pass_loose_iso(nullptr),
      pass_medium_iso(nullptr),
      pass_tight_iso(nullptr),
      pass_vtight_iso(nullptr),
      boostedTaupfTausDiscriminationByDecayModeFinding(nullptr),
      boostedTaupfTausDiscriminationByDecayModeFindingNewDMs(nullptr),
      boostedTauByMVA6VLooseElectronRejection(nullptr),
      boostedTauByMVA6LooseElectronRejection(nullptr),
      boostedTauByMVA6MediumElectronRejection(nullptr),
      boostedTauByMVA6TightElectronRejection(nullptr),
      boostedTauByMVA6VTightElectronRejection(nullptr),
      boostedTauByLooseMuonRejection3(nullptr),
      boostedTauByTightMuonRejection3(nullptr),
      boostedTauDecayMode(nullptr),
      boostedTauCharge(nullptr),
      boostedTaudz(nullptr),
      boostedTaudxy(nullptr) {
    tree->SetBranchAddress("nBoostedTau", &nBoostedTau);
    tree->SetBranchAddress("boostedTauPt", &boostedTauPt);
    tree->SetBranchAddress("boostedTauEta", &boostedTauEta);
    tree->SetBranchAddress("boostedTauPhi", &boostedTauPhi);
    tree->SetBranchAddress("boostedTauMass", &boostedTauMass);
    // only 1-type of isolation available, for now
    tree->SetBranchAddress("boostedTauByIsolationMVArun2v1DBoldDMwLTraw", &iso);
    tree->SetBranchAddress("boostedTauByVLooseIsolationMVArun2v1DBoldDMwLT", &pass_vloose_iso);
    tree->SetBranchAddress("boostedTauByLooseIsolationMVArun2v1DBoldDMwLT", &pass_loose_iso);
    tree->SetBranchAddress("boostedTauByMediumIsolationMVArun2v1DBoldDMwLT", &pass_medium_iso);
    tree->SetBranchAddress("boostedTauByTightIsolationMVArun2v1DBoldDMwLT", &pass_tight_iso);
    tree->SetBranchAddress("boostedTauByVTightIsolationMVArun2v1DBoldDMwLT", &pass_vtight_iso);
    tree->SetBranchAddress("boostedTaupfTausDiscriminationByDecayModeFinding", &boostedTaupfTausDiscriminationByDecayModeFinding);
    tree->SetBranchAddress("boostedTaupfTausDiscriminationByDecayModeFindingNewDMs", &boostedTaupfTausDiscriminationByDecayModeFindingNewDMs);
    tree->SetBranchAddress("boostedTauByMVA6VLooseElectronRejection", &boostedTauByMVA6VLooseElectronRejection);
    tree->SetBranchAddress("boostedTauByMVA6LooseElectronRejection", &boostedTauByMVA6LooseElectronRejection);
    tree->SetBranchAddress("boostedTauByMVA6MediumElectronRejection", &boostedTauByMVA6MediumElectronRejection);
    tree->SetBranchAddress("boostedTauByMVA6TightElectronRejection", &boostedTauByMVA6TightElectronRejection);
    tree->SetBranchAddress("boostedTauByMVA6VTightElectronRejection", &boostedTauByMVA6VTightElectronRejection);
    tree->SetBranchAddress("boostedTauByLooseMuonRejection3", &boostedTauByLooseMuonRejection3);
    tree->SetBranchAddress("boostedTauByTightMuonRejection3", &boostedTauByTightMuonRejection3);
    tree->SetBranchAddress("boostedTauDecayMode", &boostedTauDecayMode);
    tree->SetBranchAddress("boostedTauCharge", &boostedTauCharge);
    tree->SetBranchAddress("boostedTaudz", &boostedTaudz);
    tree->SetBranchAddress("boostedTaudxy", &boostedTaudxy);
}

// Called once per event to construct the Boosteds. A basic
// preselection is applied to all boosted taus. All Boosteds passing
// preselection are sorted by pT and stored.
void Boosted_Factory::Run_Factory() {
    boosteds.clear();
    for (auto i = 0; i < nBoostedTau; i++) {
        // baseline/default selection
        if (boostedTauPt->at(i) < 20 || fabs(boostedTauEta->at(i)) > 2.3) {
            continue;
        }
        auto boosted = Boosted(boostedTauPt->at(i), boostedTauEta->at(i), boostedTauPhi->at(i), boostedTauMass->at(i));
        boosted.Iso = iso->at(i);
        boosted.pass_vloose_iso = pass_vloose_iso->at(i);
        boosted.pass_loose_iso = pass_loose_iso->at(i);
        boosted.pass_medium_iso = pass_medium_iso->at(i);
        boosted.pass_tight_iso = pass_tight_iso->at(i);
        boosted.pass_vtight_iso = pass_vtight_iso->at(i);
        boosted.pfTausDiscriminationByDecayModeFinding = boostedTaupfTausDiscriminationByDecayModeFinding->at(i);
        boosted.pfTausDiscriminationByDecayModeFindingNewDMs = boostedTaupfTausDiscriminationByDecayModeFindingNewDMs->at(i);
        boosted.ByMVA6VLooseElectronRejection = boostedTauByMVA6VLooseElectronRejection->at(i);
        boosted.ByMVA6LooseElectronRejection = boostedTauByMVA6LooseElectronRejection->at(i);
        boosted.ByMVA6MediumElectronRejection = boostedTauByMVA6MediumElectronRejection->at(i);
        boosted.ByMVA6TightElectronRejection = boostedTauByMVA6TightElectronRejection->at(i);
        boosted.ByMVA6VTightElectronRejection = boostedTauByMVA6VTightElectronRejection->at(i);
        boosted.ByLooseMuonRejection3 = boostedTauByLooseMuonRejection3->at(i);
        boosted.ByTightMuonRejection3 = boostedTauByTightMuonRejection3->at(i);
        boosted.DecayMode = boostedTauDecayMode->at(i);
        boosted.Charge = boostedTauCharge->at(i);
        boosted.dz = boostedTaudz->at(i);
        boosted.dxy = boostedTaudxy->at(i);
        boosteds.push_back(boosted);
    }

    // sort by pT
    std::sort(boosteds.begin(), boosteds.end(), [](Boosted &p1, Boosted &p2) -> bool { return p1.getPt() > p2.getPt(); });
    nGoodTaus = boosteds.size();
}

// Check whether the tau passes isolation at
// the provided working point. The type of isolation
// is chosen in the Boosted_Factory constructor.
Bool_t Boosted::getIso(working_point wp) {
    if (wp == vloose) {
        return pass_vloose_iso;
    } else if (wp == loose) {
        return pass_loose_iso;
    } else if (wp == medium) {
        return pass_medium_iso;
    } else if (wp == tight) {
        return pass_tight_iso;
    } else if (wp == vtight) {
        return pass_vtight_iso;
    }
    throw std::invalid_argument("Must use one of the provided tau isolation WPs");
}

// Check whether the boosted tau passes decay mode finding.
// New and old DMs are available with old being the
// default option.
Bool_t Boosted::getDiscByDM(bool newDM = false) {
    if (newDM) {
        return pfTausDiscriminationByDecayModeFindingNewDMs;
    }
    return pfTausDiscriminationByDecayModeFinding;
}

// Check whether the boosted tau passes electron rejection
// at the requested working point.
Bool_t Boosted::getEleRejection(working_point wp) {
    if (wp == vloose) {
        return ByMVA6VLooseElectronRejection;
    } else if (wp == loose) {
        return ByMVA6LooseElectronRejection;
    } else if (wp == medium) {
        return ByMVA6MediumElectronRejection;
    } else if (wp == tight) {
        return ByMVA6TightElectronRejection;
    } else if (wp == vtight) {
        return ByMVA6VTightElectronRejection;
    }
    throw std::invalid_argument("Must use one of the provided electron rejection WPs");
}

// Check whether the boosted tau passes muon rejection
// at the requested working point.
Bool_t Boosted::getMuRejection(working_point wp) {
    if (wp == loose) {
        return ByLooseMuonRejection3;
    } else if (wp == tight) {
        return ByTightMuonRejection3;
    }
    throw std::invalid_argument("Muon rejection working point was neither Tight nor Loose");
}

#endif  // INTERFACE_BOOSTED_FACTORY_H_
