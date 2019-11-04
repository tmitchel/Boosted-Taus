// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_MUON_FACTORY_H_
#define INTERFACE_MUON_FACTORY_H_

#include <algorithm>
#include <memory>
#include <vector>
#include "TTree.h"
#include "./util.h"
#include "TLorentzVector.h"

class Muon;
typedef std::vector<Muon> VMuon;

class Muon {
  friend class Muon_Factory;

 public:
    Muon(Float_t _pt, Float_t _eta, Float_t _phi, Float_t _en) { this->p4.SetPtEtaPhiE(_pt, _eta, _phi, _en); }
    Muon() { this->p4.SetPtEtaPhiE(0, 0, 0, 0); }
  // getters
  TLorentzVector getP4() { return p4; }
  Float_t getPt() { return p4.Pt(); }
  Float_t getEta() { return p4.Eta(); }
  Float_t getPhi() { return p4.Phi(); }
  Float_t getMass() { return p4.M(); }
  Bool_t getID(working_point key) { return (IDbit >> key & 1) == 1; }
  Float_t getCharge() { return Charge; }
  Float_t getType() { return Type; }
  Float_t getMuonHits() { return MuonHits; }
  Float_t getTrkQuality() { return TrkQuality; }
  Float_t getIsoTrk() { return muIsoTrk; }
  Float_t getD0() { return D0; }
  Float_t getDz() { return Dz; }
  Float_t getPFChIso() { return muPFChIso; }
  Float_t getPFNeuIso() { return muPFNeuIso; }
  Float_t getPFPhoIso() { return muPFPhoIso; }
  Float_t getPFPUIso() { return muPFPUIso; }

 private:
  TLorentzVector p4;
  Int_t Charge, Type, IDbit, MuonHits, TrkQuality;
  Float_t D0, Dz, muIsoTrk, muPFChIso, muPFNeuIso, muPFPhoIso, muPFPUIso;
};

class Muon_Factory {
   public:
    explicit Muon_Factory(TTree *);
    void Run_Factory();

    // getters
    Int_t getNTotalMuon() { return nMu; }
    Int_t getNGoodMuon() { return nGoodMu; }
    std::shared_ptr<VMuon> getMuons() { return std::make_shared<VMuon>(muons); }

   private:
    Int_t nMu, nGoodMu;
    VMuon muons;
    std::vector<Float_t> *muPt, *muEta, *muPhi, *muEn, *muD0, *muDz, *muIsoTrk, *muPFChIso, *muPFNeuIso, *muPFPhoIso, *muPFPUIso;
    std::vector<Int_t> *muCharge, *muType, *muIDbit, *muMuonHits, *muTrkQuality;
};

Muon_Factory::Muon_Factory(TTree *tree)
    : muPt(nullptr),
      muEta(nullptr),
      muPhi(nullptr),
      muEn(nullptr),
      muCharge(nullptr),
      muType(nullptr),
      muIDbit(nullptr),
      muMuonHits(nullptr),
      muTrkQuality(nullptr),
      muD0(nullptr),
      muDz(nullptr),
      muIsoTrk(nullptr),
      muPFChIso(nullptr),
      muPFNeuIso(nullptr),
      muPFPhoIso(nullptr),
      muPFPUIso(nullptr) {
    tree->SetBranchAddress("nMu", &nMu);
    tree->SetBranchAddress("muPt", &muPt);
    tree->SetBranchAddress("muEta", &muEta);
    tree->SetBranchAddress("muPhi", &muPhi);
    tree->SetBranchAddress("muEn", &muEn);
    tree->SetBranchAddress("muCharge", &muCharge);
    tree->SetBranchAddress("muType", &muType);
    tree->SetBranchAddress("muIDbit", &muIDbit);
    tree->SetBranchAddress("muMuonHits", &muMuonHits);
    tree->SetBranchAddress("muTrkQuality", &muTrkQuality);
    tree->SetBranchAddress("muD0", &muD0);
    tree->SetBranchAddress("muDz", &muDz);
    tree->SetBranchAddress("muIsoTrk", &muIsoTrk);
    tree->SetBranchAddress("muPFChIso", &muPFChIso);
    tree->SetBranchAddress("muPFNeuIso", &muPFNeuIso);
    tree->SetBranchAddress("muPFPhoIso", &muPFPhoIso);
    tree->SetBranchAddress("muPFPUIso", &muPFPUIso);
}

void Muon_Factory::Run_Factory() {
    muons.clear();
    for (auto i = 0; i < nMu; i++) {
        if (muPt->at(i) < 10 || fabs(muEta->at(i)) > 2.4) {
            continue;
        }
        auto muon = Muon(muPt->at(i), muEta->at(i), muPhi->at(i), muEn->at(i));
        muon.Charge = muCharge->at(i);
        muon.Type = muType->at(i);
        muon.IDbit = muIDbit->at(i);
        muon.MuonHits = muMuonHits->at(i);
        muon.TrkQuality = muTrkQuality->at(i);
        muon.muIsoTrk = muIsoTrk->at(i);
        muon.D0 = muD0->at(i);
        muon.Dz = muDz->at(i);
        muon.muPFChIso = muPFChIso->at(i);
        muon.muPFNeuIso = muPFNeuIso->at(i);
        muon.muPFPhoIso = muPFPhoIso->at(i);
        muon.muPFPUIso = muPFPUIso->at(i);
        muons.push_back(muon);
    }

    // sort by pT
    std::sort(muons.begin(), muons.end(), [](Muon &p1, Muon &p2) -> bool { return p1.getPt() > p2.getPt(); });
    nGoodMu = muons.size();
}



#endif  // INTERFACE_MUON_FACTORY_H_
