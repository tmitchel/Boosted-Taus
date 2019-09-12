// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_MUON_H_
#define INTERFACE_MUON_H_

#include "./object.h"
#include "TLorentzVector.h"

class Muon : public PObject {
  friend class Muon_Factory;

 public:
  Muon() : PObject() {}
  Muon(Float_t, Float_t, Float_t, Float_t);

  // getters
  Bool_t getID(int key) { return (IDbit >> key & 1) == 1; }
  Float_t getFiredTrgs() { return FiredTrgs; }
  Float_t getFiredL1Trgs() { return FiredL1Trgs; }
  Float_t getCharge() { return Charge; }
  Float_t getType() { return Type; }
  Float_t getTrkLayers() { return TrkLayers; }
  Float_t getPixelLayers() { return PixelLayers; }
  Float_t getPixelHits() { return PixelHits; }
  Float_t getMuonHits() { return MuonHits; }
  Float_t getStations() { return Stations; }
  Float_t getMatches() { return Matches; }
  Float_t getTrkQuality() { return TrkQuality; }
  Float_t getBestTrkType() { return BestTrkType; }
  Float_t getD0() { return D0; }
  Float_t getDz() { return Dz; }
  Float_t getSIP() { return SIP; }
  Float_t getChi2NDF() { return Chi2NDF; }
  Float_t getInnerD0() { return InnerD0; }
  Float_t getInnerDz() { return InnerDz; }
  Float_t getIsoTrk() { return IsoTrk; }
  Float_t getPFChIso() { return PFChIso; }
  Float_t getPFPhoIso() { return PFPhoIso; }
  Float_t getPFNeuIso() { return PFNeuIso; }
  Float_t getPFPUIso() { return PFPUIso; }
  Float_t getInnervalidFraction() { return InnervalidFraction; }
  Float_t getsegmentCompatibility() { return segmentCompatibility; }
  Float_t getchi2LocalPosition() { return chi2LocalPosition; }
  Float_t gettrkKink() { return trkKink; }
  Float_t getBestTrkPtError() { return BestTrkPtError; }
  Float_t getBestTrkPt() { return BestTrkPt; }

 private:
  ULong64_t FiredTrgs, FiredL1Trgs;
  Int_t Charge, Type, IDbit, TrkLayers, PixelLayers, PixelHits, MuonHits, Stations, Matches, TrkQuality, BestTrkType;
  Float_t D0, Dz, SIP, Chi2NDF, InnerD0, InnerDz, IsoTrk, PFChIso, PFPhoIso, PFNeuIso, PFPUIso, InnervalidFraction, segmentCompatibility,
      chi2LocalPosition, trkKink, BestTrkPtError, BestTrkPt;
};

Muon::Muon(Float_t pt, Float_t eta, Float_t phi, Float_t energy) : PObject(pt, eta, phi, energy, comp0::energy) {}

#endif  // INTERFACE_MUON_H_
