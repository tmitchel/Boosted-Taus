// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_TAU_FACTORY_H_
#define INTERFACE_TAU_FACTORY_H_

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include "./util.h"
#include "TLorentzVector.h"
#include "TTree.h"

class Tau;  // Tau combines all tau information
typedef std::vector<Tau> VTau;

// Tau combines all tau related information along with
// creating the tau 4-vector. Helper functions are provided
// to access properties of the taus. These properties cannot
// be updated.
class Tau {
    friend class Tau_Factory;  // Tau_Factory constructs individual Taus

   public:
    Tau(Float_t _pt, Float_t _eta, Float_t _phi, Float_t _m) { this->p4.SetPtEtaPhiM(_pt, _eta, _phi, _m); }
    Tau() { this->p4.SetPtEtaPhiE(0, 0, 0, 0); }

    // getters
    TLorentzVector getP4() { return p4; }
    Float_t getPt() { return p4.Pt(); }
    Float_t getEta() { return p4.Eta(); }
    Float_t getPhi() { return p4.Phi(); }
    Float_t getMass() { return p4.M(); }
    Bool_t getIso(working_point);
    Bool_t getDiscByDM(bool);
    Bool_t getEleRejection(working_point);
    Bool_t getMuRejection(working_point);
    Bool_t getCombinedIsolationDeltaBetaCorr3Hits(working_point);
    Int_t getDecayMode() { return DecayMode; }
    Float_t getCharge() { return Charge; }
    Float_t getDxy() { return Dxy; }
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

    Float_t Charge, Dxy, Mass, dz, dxy;
};

// Tau_Factory reads from the TTree and constructs
// individual Tau objects. The Tau_Factory holds the
// list of taus, but can provided a shared_ptr to the
// list.
class Tau_Factory {
   public:
    explicit Tau_Factory(TTree *, std::string);
    void Run_Factory();

    // getters
    Int_t getNTotalTau() { return nTau; }     // nTaus directly from TTree
    Int_t getNGoodTau() { return nGoodTau; }  // nTaus passing preselection
    std::shared_ptr<VTau> getTaus() { return std::make_shared<VTau>(taus); }

   private:
    Int_t nTau, nGoodTau;
    VTau taus;

    std::vector<Bool_t> *taupfTausDiscriminationByDecayModeFinding, *taupfTausDiscriminationByDecayModeFindingNewDMs,
        *tauByMVA6VLooseElectronRejection, *tauByMVA6LooseElectronRejection, *tauByMVA6MediumElectronRejection, *tauByMVA6TightElectronRejection,
        *tauByMVA6VTightElectronRejection, *tauByLooseMuonRejection3, *tauByTightMuonRejection3;

    std::vector<Bool_t> *pass_vloose_iso, *pass_loose_iso, *pass_medium_iso, *pass_tight_iso, *pass_vtight_iso;

    std::vector<Int_t> *tauDecayMode;

    std::vector<Float_t> *tauEta, *tauPhi, *tauPt, *tauCharge, *tauP, *tauPx, *tauPy, *tauPz, *tauMass, *tauDxy, *taudz, *taudxy, *iso;
};

// Set all branch addresses when constructing a Tau_Factory.
// The type of tau isolation can be chosen and defaults to
// MVArun2v1DBOldDMwLT.
Tau_Factory::Tau_Factory(TTree *tree, std::string isoType = "IsolationMVArun2v1DBoldDMwLT")
    : tauPt(nullptr),
      tauEta(nullptr),
      tauPhi(nullptr),
      tauMass(nullptr),
      iso(nullptr),
      pass_vloose_iso(nullptr),
      pass_loose_iso(nullptr),
      pass_medium_iso(nullptr),
      pass_tight_iso(nullptr),
      pass_vtight_iso(nullptr),
      taupfTausDiscriminationByDecayModeFinding(nullptr),
      taupfTausDiscriminationByDecayModeFindingNewDMs(nullptr),
      tauByMVA6VLooseElectronRejection(nullptr),
      tauByMVA6LooseElectronRejection(nullptr),
      tauByMVA6MediumElectronRejection(nullptr),
      tauByMVA6TightElectronRejection(nullptr),
      tauByMVA6VTightElectronRejection(nullptr),
      tauByLooseMuonRejection3(nullptr),
      tauByTightMuonRejection3(nullptr),
      tauDecayMode(nullptr),
      tauCharge(nullptr),
      tauP(nullptr),
      tauPx(nullptr),
      tauPy(nullptr),
      tauPz(nullptr),
      tauDxy(nullptr),
      taudz(nullptr),
      taudxy(nullptr) {
    tree->SetBranchAddress("nTau", &nTau);
    tree->SetBranchAddress("tauPt", &tauPt);
    tree->SetBranchAddress("tauEta", &tauEta);
    tree->SetBranchAddress("tauPhi", &tauPhi);
    tree->SetBranchAddress("tauMass", &tauMass);
    tree->SetBranchAddress(("tauBy" + isoType + "raw").c_str(), &iso);
    tree->SetBranchAddress(("tauByVLoose" + isoType).c_str(), &pass_vloose_iso);
    tree->SetBranchAddress(("tauByLoose" + isoType).c_str(), &pass_loose_iso);
    tree->SetBranchAddress(("tauByMedium" + isoType).c_str(), &pass_medium_iso);
    tree->SetBranchAddress(("tauByTight" + isoType).c_str(), &pass_tight_iso);
    tree->SetBranchAddress(("tauByVTight" + isoType).c_str(), &pass_vtight_iso);
    tree->SetBranchAddress("taupfTausDiscriminationByDecayModeFinding", &taupfTausDiscriminationByDecayModeFinding);
    tree->SetBranchAddress("taupfTausDiscriminationByDecayModeFindingNewDMs", &taupfTausDiscriminationByDecayModeFindingNewDMs);
    tree->SetBranchAddress("tauByMVA6VLooseElectronRejection", &tauByMVA6VLooseElectronRejection);
    tree->SetBranchAddress("tauByMVA6LooseElectronRejection", &tauByMVA6LooseElectronRejection);
    tree->SetBranchAddress("tauByMVA6MediumElectronRejection", &tauByMVA6MediumElectronRejection);
    tree->SetBranchAddress("tauByMVA6TightElectronRejection", &tauByMVA6TightElectronRejection);
    tree->SetBranchAddress("tauByMVA6VTightElectronRejection", &tauByMVA6VTightElectronRejection);
    tree->SetBranchAddress("tauByLooseMuonRejection3", &tauByLooseMuonRejection3);
    tree->SetBranchAddress("tauByTightMuonRejection3", &tauByTightMuonRejection3);
    tree->SetBranchAddress("tauDecayMode", &tauDecayMode);
    tree->SetBranchAddress("tauCharge", &tauCharge);
    tree->SetBranchAddress("tauP", &tauP);
    tree->SetBranchAddress("tauPx", &tauPx);
    tree->SetBranchAddress("tauPy", &tauPy);
    tree->SetBranchAddress("tauPz", &tauPz);
    tree->SetBranchAddress("tauDxy", &tauDxy);
    tree->SetBranchAddress("taudz", &taudz);
    tree->SetBranchAddress("taudxy", &taudxy);
}

// Called once per event to construct the Taus. A basic
// preselection is applied to all taus. All Taus passing
// preselection are sorted by pT and stored.
void Tau_Factory::Run_Factory() {
    taus.clear();
    for (auto i = 0; i < nTau; i++) {
        // baseline/default selection
        if (tauPt->at(i) < 20 || tauEta->at(i) > 2.3 || !pass_vloose_iso->at(i)) {
            continue;
        }
        auto tau = Tau(tauPt->at(i), tauEta->at(i), tauPhi->at(i), tauMass->at(i));  // build the tau
        tau.pass_vloose_iso = pass_vloose_iso->at(i);
        tau.pass_loose_iso = pass_loose_iso->at(i);
        tau.pass_medium_iso = pass_medium_iso->at(i);
        tau.pass_tight_iso = pass_tight_iso->at(i);
        tau.pass_vtight_iso = pass_vtight_iso->at(i);
        tau.pfTausDiscriminationByDecayModeFinding = taupfTausDiscriminationByDecayModeFinding->at(i);
        tau.pfTausDiscriminationByDecayModeFindingNewDMs = taupfTausDiscriminationByDecayModeFindingNewDMs->at(i);
        tau.ByMVA6VLooseElectronRejection = tauByMVA6VLooseElectronRejection->at(i);
        tau.ByMVA6LooseElectronRejection = tauByMVA6LooseElectronRejection->at(i);
        tau.ByMVA6MediumElectronRejection = tauByMVA6MediumElectronRejection->at(i);
        tau.ByMVA6TightElectronRejection = tauByMVA6TightElectronRejection->at(i);
        tau.ByMVA6VTightElectronRejection = tauByMVA6VTightElectronRejection->at(i);
        tau.ByLooseMuonRejection3 = tauByLooseMuonRejection3->at(i);
        tau.ByTightMuonRejection3 = tauByTightMuonRejection3->at(i);
        tau.DecayMode = tauDecayMode->at(i);
        tau.Charge = tauCharge->at(i);
        tau.Dxy = tauDxy->at(i);
        tau.dz = taudz->at(i);
        tau.dxy = taudxy->at(i);
        taus.push_back(tau);
    }

    // sort by pT
    std::sort(taus.begin(), taus.end(), [](Tau &p1, Tau &p2) -> bool { return p1.getPt() > p2.getPt(); });
    nGoodTau = taus.size();
}

// Check whether the tau passes isolation at
// the provided working point. The type of isolation
// is chosen in the Tau_Factory constructor.
Bool_t Tau::getIso(working_point wp) {
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

// Check whether the Tau passes decay mode finding.
// New and old DMs are available with old being the
// default option.
Bool_t Tau::getDiscByDM(bool newDM = false) {
    if (newDM) {
        return pfTausDiscriminationByDecayModeFindingNewDMs;
    }
    return pfTausDiscriminationByDecayModeFinding;
}

// Check whether the Tau passes electron rejection
// at the requested working point.
Bool_t Tau::getEleRejection(working_point wp) {
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

// Check whether the Tau passes muon rejection
// at the requested working point.
Bool_t Tau::getMuRejection(working_point wp) {
    if (wp == loose) {
        return ByLooseMuonRejection3;
    } else if (wp == tight) {
        return ByTightMuonRejection3;
    }
    throw std::invalid_argument("Muon rejection working point was neither Tight nor Loose");
}

#endif  // INTERFACE_TAU_FACTORY_H_
