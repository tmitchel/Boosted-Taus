// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_ELECTRON_FACTORY_H_
#define INTERFACE_ELECTRON_FACTORY_H_

#include <algorithm>
#include <memory>
#include <vector>
#include "./electron.h"
#include "TTree.h"

typedef std::vector<Electron> VElectron;

class Electron_Factory {
 public:
  explicit Electron_Factory(TTree *);
  void Run_Factory();

  // getters
  Int_t getNTotalElectron() { return nEle; }
  Int_t getNGoodElectron() { return nGoodEle; }
  std::shared_ptr<VElectron> getElectrons() { return std::make_shared<VElectron>(electrons); }

 private:
  Int_t nEle, nGoodEle;
  VElectron electrons;
  std::vector<Short_t> *eleIDbit;
  std::vector<ULong64_t> *eleFiredSingleTrgs, *eleFiredDoubleTrgs, *eleFiredL1Trgs;
  std::vector<Int_t> *eleCharge, *eleChargeConsistent, *eleConvVeto, *eleMissHits;
  std::vector<Float_t> *eleEn, *eleSCEn, *eleEcalEn, *eleESEnP1, *eleESEnP2, *eleD0, *eleDz, *eleSIP, *elePt, *elePtError,
      *eleEta, *elePhi, *eleSCEta, *eleSCPhi, *eleESEffSigmaRR, *elePFChIso, *elePFPhoIso, *elePFNeuIso, *elePFPUIso,
      *elePFClusEcalIso, *elePFClusHcalIso, *eleIDMVAIso, *eleIDMVANoIso;
};

Electron_Factory::Electron_Factory(TTree *tree)
    : eleCharge(nullptr),
      eleChargeConsistent(nullptr),
      eleEn(nullptr),
      eleSCEn(nullptr),
      eleEcalEn(nullptr),
      eleESEnP1(nullptr),
      eleESEnP2(nullptr),
      eleD0(nullptr),
      eleDz(nullptr),
      eleSIP(nullptr),
      elePt(nullptr),
      elePtError(nullptr),
      eleEta(nullptr),
      elePhi(nullptr),
      eleSCEta(nullptr),
      eleSCPhi(nullptr),
      eleConvVeto(nullptr),
      eleMissHits(nullptr),
      eleESEffSigmaRR(nullptr),
      elePFChIso(nullptr),
      elePFPhoIso(nullptr),
      elePFNeuIso(nullptr),
      elePFPUIso(nullptr),
      elePFClusEcalIso(nullptr),
      elePFClusHcalIso(nullptr),
      eleIDMVAIso(nullptr),
      eleIDMVANoIso(nullptr),
      eleFiredSingleTrgs(nullptr),
      eleFiredDoubleTrgs(nullptr),
      eleFiredL1Trgs(nullptr),
      eleIDbit(nullptr) {
  tree->SetBranchAddress("nEle", &nEle);
  tree->SetBranchAddress("eleSCEn", &eleSCEn);
  tree->SetBranchAddress("eleEcalEn", &eleEcalEn);
  tree->SetBranchAddress("eleESEnP1", &eleESEnP1);
  tree->SetBranchAddress("eleESEnP2", &eleESEnP2);
  tree->SetBranchAddress("eleD0", &eleD0);
  tree->SetBranchAddress("eleDz", &eleDz);
  tree->SetBranchAddress("eleSIP", &eleSIP);
  tree->SetBranchAddress("elePt", &elePt);
  tree->SetBranchAddress("elePtError", &elePtError);
  tree->SetBranchAddress("eleEn", &eleEn);
  tree->SetBranchAddress("eleEta", &eleEta);
  tree->SetBranchAddress("elePhi", &elePhi);
  tree->SetBranchAddress("eleSCEta", &eleSCEta);
  tree->SetBranchAddress("eleSCPhi", &eleSCPhi);
  tree->SetBranchAddress("eleConvVeto", &eleConvVeto);
  tree->SetBranchAddress("eleMissHits", &eleMissHits);
  tree->SetBranchAddress("eleESEffSigmaRR", &eleESEffSigmaRR);
  tree->SetBranchAddress("elePFChIso", &elePFChIso);
  tree->SetBranchAddress("elePFPhoIso", &elePFPhoIso);
  tree->SetBranchAddress("elePFNeuIso", &elePFNeuIso);
  tree->SetBranchAddress("elePFPUIso", &elePFPUIso);
  tree->SetBranchAddress("elePFClusEcalIso", &elePFClusEcalIso);
  tree->SetBranchAddress("elePFClusHcalIso", &elePFClusHcalIso);
  tree->SetBranchAddress("eleIDMVAIso", &eleIDMVAIso);
  tree->SetBranchAddress("eleIDMVANoIso", &eleIDMVANoIso);
  tree->SetBranchAddress("eleFiredSingleTrgs", &eleFiredSingleTrgs);
  tree->SetBranchAddress("eleFiredDoubleTrgs", &eleFiredDoubleTrgs);
  tree->SetBranchAddress("eleFiredL1Trgs", &eleFiredL1Trgs);
  tree->SetBranchAddress("eleIDbit", &eleIDbit);
}

void Electron_Factory::Run_Factory() {
  electrons.clear();
  for (auto i = 0; i < nEle; i++) {
    if (elePt->at(i) < 10 || fabs(eleEta->at(i)) > 2.1) {
      continue;
    }
    auto electron = Electron(elePt->at(i), eleEta->at(i), elePhi->at(i), eleEn->at(i));
    electron.SCEn = eleSCEn->at(i);
    electron.EcalEn = eleEcalEn->at(i);
    electron.ESEnP1 = eleESEnP1->at(i);
    electron.ESEnP2 = eleESEnP2->at(i);
    electron.D0 = eleD0->at(i);
    electron.Dz = eleDz->at(i);
    electron.Pt = elePt->at(i);
    electron.PtError = elePtError->at(i);
    electron.Eta = eleEta->at(i);
    electron.Phi = elePhi->at(i);
    electron.SCEta = eleSCEta->at(i);
    electron.SCPhi = eleSCPhi->at(i);
    electron.ConvVeto = eleConvVeto->at(i);
    electron.MissHits = eleMissHits->at(i);
    electron.ESEffSigmaRR = eleESEffSigmaRR->at(i);
    electron.PFChIso = elePFChIso->at(i);
    electron.PFPhoIso = elePFPhoIso->at(i);
    electron.PFNeuIso = elePFNeuIso->at(i);
    electron.PFPUIso = elePFPUIso->at(i);
    electron.PFClusEcalIso = elePFClusEcalIso->at(i);
    electron.PFClusHcalIso = elePFClusHcalIso->at(i);
    electron.IDMVAIso = eleIDMVAIso->at(i);
    electron.IDMVANoIso = eleIDMVANoIso->at(i);
    electron.FiredSingleTrgs = eleFiredSingleTrgs->at(i);
    electron.FiredDoubleTrgs = eleFiredDoubleTrgs->at(i);
    electron.FiredL1Trgs = eleFiredL1Trgs->at(i);
    electron.IDbit = eleIDbit->at(i);
    electrons.push_back(electron);
  }

  // sort by pT
  std::sort(electrons.begin(), electrons.end(), [](Electron &p1, Electron &p2) -> bool { return p1.getPt() > p2.getPt(); });
  nGoodEle = electrons.size();
}

#endif  // INTERFACE_ELECTRON_FACTORY_H_
