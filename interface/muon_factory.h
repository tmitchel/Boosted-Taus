// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_MUON_FACTORY_H_
#define INTERFACE_MUON_FACTORY_H_

#include <algorithm>
#include <memory>
#include <vector>
#include "./muon.h"
#include "TTree.h"

typedef std::vector<Muon> VMuon;

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
  std::vector<Float_t> *muPt, *muEta, *muPhi, *muEn;
  std::vector<ULong64_t> *muFiredTrgs, *muFiredL1Trgs;
  std::vector<Int_t> *muCharge, *muType, *muIDbit, *muTrkLayers, *muPixelLayers, *muPixelHits, *muMuonHits, *muStations, *muMatches, *muTrkQuality, *muBestTrkType;
  std::vector<Float_t> *muD0, *muDz, *muSIP, *muChi2NDF, *muInnerD0, *muInnerDz, *muIsoTrk, *muPFChIso, *muPFPhoIso, *muPFNeuIso, *muPFPUIso, *muInnervalidFraction, *musegmentCompatibility,
      *muchi2LocalPosition, *mutrkKink, *muBestTrkPtError, *muBestTrkPt;
};

Muon_Factory::Muon_Factory(TTree *tree)
    : muPt(nullptr),
      muEta(nullptr),
      muPhi(nullptr),
      muEn(nullptr),
      muFiredTrgs(nullptr),
      muFiredL1Trgs(nullptr),
      muCharge(nullptr),
      muType(nullptr),
      muIDbit(nullptr),
      muTrkLayers(nullptr),
      muPixelLayers(nullptr),
      muPixelHits(nullptr),
      muMuonHits(nullptr),
      muStations(nullptr),
      muMatches(nullptr),
      muTrkQuality(nullptr),
      muBestTrkType(nullptr),
      muD0(nullptr),
      muDz(nullptr),
      muSIP(nullptr),
      muChi2NDF(nullptr),
      muInnerD0(nullptr),
      muInnerDz(nullptr),
      muIsoTrk(nullptr),
      muPFChIso(nullptr),
      muPFPhoIso(nullptr),
      muPFNeuIso(nullptr),
      muPFPUIso(nullptr),
      muInnervalidFraction(nullptr),
      musegmentCompatibility(nullptr),
      muchi2LocalPosition(nullptr),
      mutrkKink(nullptr),
      muBestTrkPtError(nullptr),
      muBestTrkPt(nullptr) {
  tree->SetBranchAddress("nMu", &nMu);
  tree->SetBranchAddress("muPt", &muPt);
  tree->SetBranchAddress("muEta", &muEta);
  tree->SetBranchAddress("muPhi", &muPhi);
  tree->SetBranchAddress("muEn", &muEn);
  tree->SetBranchAddress("muFiredTrgs", &muFiredTrgs);
  tree->SetBranchAddress("muFiredL1Trgs", &muFiredL1Trgs);
  tree->SetBranchAddress("muCharge", &muCharge);
  tree->SetBranchAddress("muType", &muType);
  tree->SetBranchAddress("muIDbit", &muIDbit);
  tree->SetBranchAddress("muTrkLayers", &muTrkLayers);
  tree->SetBranchAddress("muPixelLayers", &muPixelLayers);
  tree->SetBranchAddress("muPixelHits", &muPixelHits);
  tree->SetBranchAddress("muMuonHits", &muMuonHits);
  tree->SetBranchAddress("muStations", &muStations);
  tree->SetBranchAddress("muMatches", &muMatches);
  tree->SetBranchAddress("muTrkQuality", &muTrkQuality);
  tree->SetBranchAddress("muBestTrkType", &muBestTrkType);
  tree->SetBranchAddress("muD0", &muD0);
  tree->SetBranchAddress("muDz", &muDz);
  tree->SetBranchAddress("muSIP", &muSIP);
  tree->SetBranchAddress("muChi2NDF", &muChi2NDF);
  tree->SetBranchAddress("muInnerD0", &muInnerD0);
  tree->SetBranchAddress("muInnerDz", &muInnerDz);
  tree->SetBranchAddress("muIsoTrk", &muIsoTrk);
  tree->SetBranchAddress("muPFChIso", &muPFChIso);
  tree->SetBranchAddress("muPFPhoIso", &muPFPhoIso);
  tree->SetBranchAddress("muPFNeuIso", &muPFNeuIso);
  tree->SetBranchAddress("muPFPUIso", &muPFPUIso);
  tree->SetBranchAddress("muInnervalidFraction", &muInnervalidFraction);
  tree->SetBranchAddress("musegmentCompatibility", &musegmentCompatibility);
  tree->SetBranchAddress("muchi2LocalPosition", &muchi2LocalPosition);
  tree->SetBranchAddress("mutrkKink", &mutrkKink);
  tree->SetBranchAddress("muBestTrkPtError", &muBestTrkPtError);
  tree->SetBranchAddress("muBestTrkPt", &muBestTrkPt);
}

void Muon_Factory::Run_Factory() {
  muons.clear();
  for (auto i = 0; i < nMu; i++) {
    if (muPt->at(i) < 20) {
      continue;
    }
    auto muon = Muon(muPt->at(i), muEta->at(i), muPhi->at(i), muEn->at(i));
    muon.FiredTrgs = muFiredTrgs->at(i);
    muon.FiredL1Trgs = muFiredL1Trgs->at(i);
    muon.Charge = muCharge->at(i);
    muon.Type = muType->at(i);
    muon.IDbit = muIDbit->at(i);
    muon.TrkLayers = muTrkLayers->at(i);
    muon.PixelLayers = muPixelLayers->at(i);
    muon.PixelHits = muPixelHits->at(i);
    muon.MuonHits = muMuonHits->at(i);
    muon.Stations = muStations->at(i);
    muon.Matches = muMatches->at(i);
    muon.TrkQuality = muTrkQuality->at(i);
    muon.BestTrkType = muBestTrkType->at(i);
    muon.D0 = muD0->at(i);
    muon.Dz = muDz->at(i);
    muon.SIP = muSIP->at(i);
    muon.Chi2NDF = muChi2NDF->at(i);
    muon.InnerD0 = muInnerD0->at(i);
    muon.InnerDz = muInnerDz->at(i);
    muon.IsoTrk = muIsoTrk->at(i);
    muon.PFChIso = muPFChIso->at(i);
    muon.PFPhoIso = muPFPhoIso->at(i);
    muon.PFNeuIso = muPFNeuIso->at(i);
    muon.PFPUIso = muPFPUIso->at(i);
    muon.InnervalidFraction = muInnervalidFraction->at(i);
    muon.segmentCompatibility = musegmentCompatibility->at(i);
    muon.chi2LocalPosition = muchi2LocalPosition->at(i);
    muon.trkKink = mutrkKink->at(i);
    muon.BestTrkPtError = muBestTrkPtError->at(i);
    muon.BestTrkPt = muBestTrkPt->at(i);
    muons.push_back(muon);
  }

  // sort by pT
  std::sort(muons.begin(), muons.end(), [](Muon &p1, Muon &p2) -> bool { return p1.getPt() > p2.getPt(); });
  nGoodMu = muons.size();
}

#endif  // INTERFACE_MUON_FACTORY_H_
