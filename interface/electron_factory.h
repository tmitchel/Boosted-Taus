// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_ELECTRON_FACTORY_H_
#define INTERFACE_ELECTRON_FACTORY_H_

#include <algorithm>
#include <memory>
#include <vector>
#include "./util.h"
#include "TLorentzVector.h"
#include "TTree.h"

class Electron;  // Electorn combines all electron information
typedef std::vector<Electron> VElectron;

// Electron combines all electron related information along with
// creating the electron 4-vector. Helper functions are provided
// to access properties of the electrons. These properties cannot
// be updated.
class Electron {
    friend class Electron_Factory;  // Electron_Factory constructs individual Electrons

   public:
    Electron(Float_t _pt, Float_t _eta, Float_t _phi, Float_t _en) { this->p4.SetPtEtaPhiE(_pt, _eta, _phi, _en); }
    Electron() { this->p4.SetPtEtaPhiE(0, 0, 0, 0); }

    // getters
    TLorentzVector getP4() { return p4; }
    Int_t getCharge() { return Charge; }
    Int_t getConvVeto() { return ConvVeto; }
    Int_t getMissHits() { return MissHits; }
    Float_t getPt() { return p4.Pt(); }
    Float_t getEta() { return p4.Eta(); }
    Float_t getPhi() { return p4.Phi(); }
    Float_t getMass() { return p4.M(); }
    Float_t getD0() { return D0; }
    Float_t getDz() { return Dz; }
    Float_t getSCEta() { return eleSCEta; }
    Bool_t getIDMVAIso() { return IDMVAIso; }
    Bool_t getIDMVANoIso() { return IDMVANoIso; }
    Bool_t getID(working_point key) { return (IDbit >> key & 1) == 1; }

   private:
    // these should never be modified once read from the TTree
    TLorentzVector p4;
    Short_t IDbit;
    Int_t Charge, ConvVeto, MissHits;
    Float_t D0, Dz, IDMVAIso, IDMVANoIso, eleSCEta;
};

// Electron_Factory reads from the TTree and constructs
// individual Electron objects. The Electron_Factory holds the
// list of electrons, but can provided a shared_ptr to the
// list.
class Electron_Factory {
   public:
    explicit Electron_Factory(TTree *);
    void Run_Factory();

    // getters
    Int_t getNTotalElectron() { return nEle; }     // nEle directly from TTree
    Int_t getNGoodElectron() { return nGoodEle; }  // nEle passing preselection
    std::shared_ptr<VElectron> getElectrons() { return std::make_shared<VElectron>(electrons); }

   private:
    Int_t nEle, nGoodEle;
    VElectron electrons;
    std::vector<Short_t> *eleIDbit;
    std::vector<Int_t> *eleCharge, *eleConvVeto, *eleMissHits;
    std::vector<Float_t> *eleEn, *eleD0, *eleDz, *elePt, *eleEta, *elePhi, *eleIDMVAIso, *eleIDMVANoIso, *eleSCEta;
};

// Set all branch addresses when constructing a Electron_Factory.
Electron_Factory::Electron_Factory(TTree *tree)
    : eleCharge(nullptr),
      eleEn(nullptr),
      eleD0(nullptr),
      eleDz(nullptr),
      elePt(nullptr),
      eleEta(nullptr),
      elePhi(nullptr),
      eleConvVeto(nullptr),
      eleMissHits(nullptr),
      eleIDMVAIso(nullptr),
      eleIDMVANoIso(nullptr),
      eleIDbit(nullptr),
      eleSCEta(nullptr) {
    tree->SetBranchAddress("nEle", &nEle);
    tree->SetBranchAddress("eleCharge", &eleCharge);
    tree->SetBranchAddress("eleD0", &eleD0);
    tree->SetBranchAddress("eleDz", &eleDz);
    tree->SetBranchAddress("elePt", &elePt);
    tree->SetBranchAddress("eleEn", &eleEn);
    tree->SetBranchAddress("eleEta", &eleEta);
    tree->SetBranchAddress("elePhi", &elePhi);
    tree->SetBranchAddress("eleConvVeto", &eleConvVeto);
    tree->SetBranchAddress("eleMissHits", &eleMissHits);
    tree->SetBranchAddress("eleIDMVAIso", &eleIDMVAIso);
    tree->SetBranchAddress("eleIDMVANoIso", &eleIDMVANoIso);
    tree->SetBranchAddress("eleIDbit", &eleIDbit);
    tree->SetBranchAddress("eleSCEta", &eleSCEta);
}

// Called once per event to construct the Electrons. A basic
// preselection is applied to all electrons. All Electrons passing
// preselection are sorted by pT and stored.
void Electron_Factory::Run_Factory() {
    electrons.clear();
    for (auto i = 0; i < nEle; i++) {
        if (elePt->at(i) < 10 || fabs(eleEta->at(i)) > 2.5) {
            continue;
        }
        auto electron = Electron(elePt->at(i), eleEta->at(i), elePhi->at(i), eleEn->at(i));
        electron.Charge = eleCharge->at(i);
        electron.D0 = eleD0->at(i);
        electron.Dz = eleDz->at(i);
        electron.ConvVeto = eleConvVeto->at(i);
        electron.MissHits = eleMissHits->at(i);
        electron.IDMVAIso = eleIDMVAIso->at(i);
        electron.IDMVANoIso = eleIDMVANoIso->at(i);
        electron.IDbit = eleIDbit->at(i);
        electron.eleSCEta = eleSCEta->at(i);
        electrons.push_back(electron);
    }

    // sort by pT
    std::sort(electrons.begin(), electrons.end(), [](Electron &p1, Electron &p2) -> bool { return p1.getPt() > p2.getPt(); });
    nGoodEle = electrons.size();
}

#endif  // INTERFACE_ELECTRON_FACTORY_H_
