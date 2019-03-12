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
  std::vector<Int_t> *eleCharge, *eleChargeConsistent, *eleConvVeto, *eleMissHits, *eleEcalDrivenSeed;
  std::vector<Float_t> *eleEn, *eleSCEn, *eleEcalEn, *eleESEnP1, *eleESEnP2, *eleD0, *eleDz, *eleSIP, *elePt, *elePtError,
      *eleEta, *elePhi, *eleR9, *eleCalibPt, *eleCalibEn, *eleSCEta, *eleSCPhi, *eleSCRawEn, *eleSCEtaWidth, *eleSCPhiWidth,
      *eleHoverE, *eleEoverP, *eleEoverPout, *eleEoverPInv, *eleBrem, *eledEtaAtVtx, *eledPhiAtVtx, *eleSigmaIEtaIEtaFull5x5,
      *eleSigmaIPhiIPhiFull5x5, *eleESEffSigmaRR, *elePFChIso, *elePFPhoIso, *elePFNeuIso, *elePFPUIso, *elePFClusEcalIso,
      *elePFClusHcalIso, *eleIDMVAIso, *eleIDMVANoIso, *eleR9Full5x5, *eleTrkdxy, *eleKFHits, *eleKFChi2, *eleGSFChi2,
      *eleScale_stat_up, *eleScale_stat_dn, *eleScale_syst_up, *eleScale_syst_dn, *eleScale_gain_up, *eleScale_gain_dn,
      *eleResol_rho_up, *eleResol_rho_dn, *eleResol_phi_up, *eleResol_phi_dn;
  std::vector<std::vector<Int_t>> *eleGSFHits, *eleGSFMissHits, *eleGSFNHitsMax;
  std::vector<std::vector<Float_t>> *eleGSFPt, *eleGSFEta, *eleGSFPhi, *eleGSFCharge, *eleGSFVtxProb, *eleGSFlxyPV, *eleGSFlxyBS, *eleBCEn,
      *eleBCEta, *eleBCPhi, *eleBCS25, *eleBCS15, *eleBCSieie, *eleBCSieip, *eleBCSipip;
};

Electron_Factory::Electron_Factory(TTree *tree)
    : eleSCEn(nullptr),
      eleEcalEn(nullptr),
      eleESEnP1(nullptr),
      eleESEnP2(nullptr),
      eleD0(nullptr),
      eleDz(nullptr),
      elePt(nullptr),
      elePtError(nullptr),
      eleEta(nullptr),
      elePhi(nullptr),
      eleR9(nullptr),
      eleCalibPt(nullptr),
      eleCalibEn(nullptr),
      eleSCEta(nullptr),
      eleSCPhi(nullptr),
      eleSCRawEn(nullptr),
      eleSCEtaWidth(nullptr),
      eleSCPhiWidth(nullptr),
      eleHoverE(nullptr),
      eleEoverP(nullptr),
      eleEoverPout(nullptr),
      eleEoverPInv(nullptr),
      eleBrem(nullptr),
      eledEtaAtVtx(nullptr),
      eledPhiAtVtx(nullptr),
      eleSigmaIEtaIEtaFull5x5(nullptr),
      eleSigmaIPhiIPhiFull5x5(nullptr),
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
      eleR9Full5x5(nullptr),
      eleEcalDrivenSeed(nullptr),
      eleTrkdxy(nullptr),
      eleKFHits(nullptr),
      eleKFChi2(nullptr),
      eleGSFChi2(nullptr),
      eleGSFPt(nullptr),
      eleGSFEta(nullptr),
      eleGSFPhi(nullptr),
      eleGSFCharge(nullptr),
      eleGSFHits(nullptr),
      eleGSFMissHits(nullptr),
      eleGSFNHitsMax(nullptr),
      eleGSFVtxProb(nullptr),
      eleGSFlxyPV(nullptr),
      eleGSFlxyBS(nullptr),
      eleBCEn(nullptr),
      eleBCEta(nullptr),
      eleBCPhi(nullptr),
      eleBCS25(nullptr),
      eleBCS15(nullptr),
      eleBCSieie(nullptr),
      eleBCSieip(nullptr),
      eleBCSipip(nullptr),
      eleFiredSingleTrgs(nullptr),
      eleFiredDoubleTrgs(nullptr),
      eleFiredL1Trgs(nullptr),
      eleIDbit(nullptr),
      eleScale_stat_up(nullptr),
      eleScale_stat_dn(nullptr),
      eleScale_syst_up(nullptr),
      eleScale_syst_dn(nullptr),
      eleScale_gain_up(nullptr),
      eleScale_gain_dn(nullptr),
      eleResol_rho_up(nullptr),
      eleResol_rho_dn(nullptr),
      eleResol_phi_up(nullptr),
      eleResol_phi_dn(nullptr) {
  tree->SetBranchAddress("nTau", &nEle);
  tree->SetBranchAddress("eleSCEn", &eleSCEn);
  tree->SetBranchAddress("eleEcalEn", &eleEcalEn);
  tree->SetBranchAddress("eleESEnP1", &eleESEnP1);
  tree->SetBranchAddress("eleESEnP2", &eleESEnP2);
  tree->SetBranchAddress("eleD0", &eleD0);
  tree->SetBranchAddress("eleDz", &eleDz);
  tree->SetBranchAddress("eleSIP", &eleSIP);
  tree->SetBranchAddress("elePt", &elePt);
  tree->SetBranchAddress("elePtError", &elePtError);
  tree->SetBranchAddress("eleEta", &eleEta);
  tree->SetBranchAddress("elePhi", &elePhi);
  tree->SetBranchAddress("eleR9", &eleR9);
  tree->SetBranchAddress("eleCalibPt", &eleCalibPt);
  tree->SetBranchAddress("eleCalibEn", &eleCalibEn);
  tree->SetBranchAddress("eleSCEta", &eleSCEta);
  tree->SetBranchAddress("eleSCPhi", &eleSCPhi);
  tree->SetBranchAddress("eleSCRawEn", &eleSCRawEn);
  tree->SetBranchAddress("eleSCEtaWidth", &eleSCEtaWidth);
  tree->SetBranchAddress("eleSCPhiWidth", &eleSCPhiWidth);
  tree->SetBranchAddress("eleHoverE", &eleHoverE);
  tree->SetBranchAddress("eleEoverP", &eleEoverP);
  tree->SetBranchAddress("eleEoverPout", &eleEoverPout);
  tree->SetBranchAddress("eleEoverPInv", &eleEoverPInv);
  tree->SetBranchAddress("eleBrem", &eleBrem);
  tree->SetBranchAddress("eledEtaAtVtx", &eledEtaAtVtx);
  tree->SetBranchAddress("eledPhiAtVtx", &eledPhiAtVtx);
  tree->SetBranchAddress("eleSigmaIEtaIEtaFull5x5", &eleSigmaIEtaIEtaFull5x5);
  tree->SetBranchAddress("eleSigmaIPhiIPhiFull5x5", &eleSigmaIPhiIPhiFull5x5);
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
  tree->SetBranchAddress("eleR9Full5x5", &eleR9Full5x5);
  tree->SetBranchAddress("eleEcalDrivenSeed", &eleEcalDrivenSeed);
  tree->SetBranchAddress("eleTrkdxy", &eleTrkdxy);
  tree->SetBranchAddress("eleKFHits", &eleKFHits);
  tree->SetBranchAddress("eleKFChi2", &eleKFChi2);
  tree->SetBranchAddress("eleGSFChi2", &eleGSFChi2);
  tree->SetBranchAddress("eleGSFPt", &eleGSFPt);
  tree->SetBranchAddress("eleGSFEta", &eleGSFEta);
  tree->SetBranchAddress("eleGSFPhi", &eleGSFPhi);
  tree->SetBranchAddress("eleGSFCharge", &eleGSFCharge);
  tree->SetBranchAddress("eleGSFHits", &eleGSFHits);
  tree->SetBranchAddress("eleGSFMissHits", &eleGSFMissHits);
  tree->SetBranchAddress("eleGSFNHitsMax", &eleGSFNHitsMax);
  tree->SetBranchAddress("eleGSFVtxProb", &eleGSFVtxProb);
  tree->SetBranchAddress("eleGSFlxyPV", &eleGSFlxyPV);
  tree->SetBranchAddress("eleGSFlxyBS", &eleGSFlxyBS);
  tree->SetBranchAddress("eleBCEn", &eleBCEn);
  tree->SetBranchAddress("eleBCEta", &eleBCEta);
  tree->SetBranchAddress("eleBCPhi", &eleBCPhi);
  tree->SetBranchAddress("eleBCS25", &eleBCS25);
  tree->SetBranchAddress("eleBCS15", &eleBCS15);
  tree->SetBranchAddress("eleBCSieie", &eleBCSieie);
  tree->SetBranchAddress("eleBCSieip", &eleBCSieip);
  tree->SetBranchAddress("eleBCSipip", &eleBCSipip);
  tree->SetBranchAddress("eleFiredSingleTrgs", &eleFiredSingleTrgs);
  tree->SetBranchAddress("eleFiredDoubleTrgs", &eleFiredDoubleTrgs);
  tree->SetBranchAddress("eleFiredL1Trgs", &eleFiredL1Trgs);
  tree->SetBranchAddress("eleIDbit", &eleIDbit);
  tree->SetBranchAddress("eleScale_stat_up", &eleScale_stat_up);
  tree->SetBranchAddress("eleScale_stat_dn", &eleScale_stat_dn);
  tree->SetBranchAddress("eleScale_syst_up", &eleScale_syst_up);
  tree->SetBranchAddress("eleScale_syst_dn", &eleScale_syst_dn);
  tree->SetBranchAddress("eleScale_gain_up", &eleScale_gain_up);
  tree->SetBranchAddress("eleScale_gain_dn", &eleScale_gain_dn);
  tree->SetBranchAddress("eleResol_rho_up", &eleResol_rho_up);
  tree->SetBranchAddress("eleResol_rho_dn", &eleResol_rho_dn);
  tree->SetBranchAddress("eleResol_phi_up", &eleResol_phi_up);
  tree->SetBranchAddress("eleResol_phi_dn", &eleResol_phi_dn);
}

void Electron_Factory::Run_Factory() {
  electrons.clear();
  for (auto i = 0; i < nEle; i++) {
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
    electron.R9 = eleR9->at(i);
    electron.CalibPt = eleCalibPt->at(i);
    electron.CalibEn = eleCalibEn->at(i);
    electron.SCEta = eleSCEta->at(i);
    electron.SCPhi = eleSCPhi->at(i);
    electron.SCRawEn = eleSCRawEn->at(i);
    electron.SCEtaWidth = eleSCEtaWidth->at(i);
    electron.SCPhiWidth = eleSCPhiWidth->at(i);
    electron.HoverE = eleHoverE->at(i);
    electron.EoverP = eleEoverP->at(i);
    electron.EoverPout = eleEoverPout->at(i);
    electron.EoverPInv = eleEoverPInv->at(i);
    electron.Brem = eleBrem->at(i);
    electron.dEtaAtVtx = eledEtaAtVtx->at(i);
    electron.dPhiAtVtx = eledPhiAtVtx->at(i);
    electron.SigmaIEtaIEtaFull5x5 = eleSigmaIEtaIEtaFull5x5->at(i);
    electron.SigmaIPhiIPhiFull5x5 = eleSigmaIPhiIPhiFull5x5->at(i);
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
    electron.R9Full5x5 = eleR9Full5x5->at(i);
    electron.EcalDrivenSeed = eleEcalDrivenSeed->at(i);
    electron.Trkdxy = eleTrkdxy->at(i);
    electron.KFHits = eleKFHits->at(i);
    electron.KFChi2 = eleKFChi2->at(i);
    electron.GSFChi2 = eleGSFChi2->at(i);
    electron.GSFPt = eleGSFPt->at(i);
    electron.GSFEta = eleGSFEta->at(i);
    electron.GSFPhi = eleGSFPhi->at(i);
    electron.GSFCharge = eleGSFCharge->at(i);
    electron.GSFHits = eleGSFHits->at(i);
    electron.GSFMissHits = eleGSFMissHits->at(i);
    electron.GSFNHitsMax = eleGSFNHitsMax->at(i);
    electron.GSFVtxProb = eleGSFVtxProb->at(i);
    electron.GSFlxyPV = eleGSFlxyPV->at(i);
    electron.GSFlxyBS = eleGSFlxyBS->at(i);
    electron.BCEn = eleBCEn->at(i);
    electron.BCEta = eleBCEta->at(i);
    electron.BCPhi = eleBCPhi->at(i);
    electron.BCS25 = eleBCS25->at(i);
    electron.BCS15 = eleBCS15->at(i);
    electron.BCSieie = eleBCSieie->at(i);
    electron.BCSieip = eleBCSieip->at(i);
    electron.BCSipip = eleBCSipip->at(i);
    electron.FiredSingleTrgs = eleFiredSingleTrgs->at(i);
    electron.FiredDoubleTrgs = eleFiredDoubleTrgs->at(i);
    electron.FiredL1Trgs = eleFiredL1Trgs->at(i);
    electron.IDbit = eleIDbit->at(i);
    electron.Scale_stat_up = eleScale_stat_up->at(i);
    electron.Scale_stat_dn = eleScale_stat_dn->at(i);
    electron.Scale_syst_up = eleScale_syst_up->at(i);
    electron.Scale_syst_dn = eleScale_syst_dn->at(i);
    electron.Scale_gain_up = eleScale_gain_up->at(i);
    electron.Scale_gain_dn = eleScale_gain_dn->at(i);
    electron.Resol_rho_up = eleResol_rho_up->at(i);
    electron.Resol_rho_dn = eleResol_rho_dn->at(i);
    electron.Resol_phi_up = eleResol_phi_up->at(i);
    electron.Resol_phi_dn = eleResol_phi_dn->at(i);
    electrons.push_back(electron);
  }

  // sort by pT
  std::sort(electrons.begin(), electrons.end(), [](Electron &p1, Electron &p2) -> bool { return p1.getPt() > p2.getPt(); });
  nGoodEle = electrons.size();
}

#endif  // INTERFACE_ELECTRON_FACTORY_H_
