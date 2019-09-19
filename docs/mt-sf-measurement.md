# Measuring the Z->tautau cross section using boosted taus

## Mu-Tau Channel

### Skim Selection

- PFMet > 75
- N(0.4 < deltaR(tau, mu) < 0.8) > 0
    - muon pT > 30
    - abs(muon eta) < 2.4
    - tau pT > 20
    - abs(tau eta) < 2.3
- MT < 40

### Analysis Selection

- N(electron) == 0
    - electron pT > 10
    - abs(electron eta) < 2.4
- N(jets) > 1
    - jet pT > 200
    - abs(jet eta) < 2.4
- PFMet > 200
- N(b-jets) == 0

##### Signal Region
- muon pT > 55
- muon PFIso / pT < 0.25 (using medium isolation instead for now)
- tau passes VLoose MVA Isolation
- tau passes VLoose MVA anti-electron
- tau passes Tight MVA anti-muon

##### Zmumu Control Region
- N(muon) == 2
    - leading muon pT > 55
    - sub-leading muon pT > 10
    - abs(muon eta) < 2.4
    - muon PFIso / pT < 0.25 (using medium isolation instead for now)
- 60 < dimuon mass < 120
- 0.05 < DeltaR(muon1, muon2) < 1.5
