// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_UTIL_H_
#define INTERFACE_UTIL_H_

#include <cmath>
#include <map>
#include <string>

enum working_point {
    vloose,
    loose,
    medium,
    tight,
    vtight
};

std::map<std::string, double> lumi = {
  {"2016", 35900},
  {"2017", 41500},
  {"2018", 63670}
};

std::map<std::string, double> cross_sections = {
  {"DYJetsToLL_M-50_HT-100to200", 148.},
  {"DYJetsToLL_M-50_HT-200to400", 40.94},
  {"DYJetsToLL_M-50_HT-400to600", 5.497},
  {"DYJetsToLL_M-50_HT-600to800", 1.354},
  {"DYJetsToLL_M-50_HT-800to1200", 0.625},
  {"DYJetsToLL_M-50_HT-1200to2500", 0.151},
  {"DYJetsToLL_M-50_HT-2500toInf", 0.003647},
  {"DYJetsToLL_M-50_v1", 0.},  // don't use inclusive yet

  {"WJetsToLNu_HT-100To200", 148.},
  {"WJetsToLNu_HT-200To400", 40.94},
  {"WJetsToLNu_HT-400To600", 5.497},
  {"WJetsToLNu_HT-600To800", 1.354},
  {"WJetsToLNu_HT-800To1200", 0.625},
  {"WJetsToLNu_HT-1200To2500", 0.151},
  {"WJetsToLNu_HT-2500ToInf", 0.003647},
  {"WJetsToLNu_Inc", 0.},  // don't use inclusive yet

  {"ST_s-channel_4f_leptonDecays", 3.36},
  {"ST_t-channel_antitop_4f_inclusiveDecays", 26.23},
  {"ST_t-channel_top_4f_inclusiveDecays", 44.07},
  {"ST_tW_antitop_5f_inclusiveDecays", 35.6},
  {"ST_tW_top_5f_inclusiveDecays", 35.6},
  {"TTTo2L2Nu", 88.29},
  {"TTToHadronic", 377.96},
  {"TTToSemiLeptonic", 365.35},

  {"WWTo1L1Nu2Q_13TeV_amcatnloFXFX_madspin", 49.997},
  {"WWTo2L2Nu_NNPDF31_TuneCP5_13TeV-powheg", 12.178},
  {"WWTo4Q_NNPDF31_TuneCP5_13TeV-powheg", 51.723},
  {"WWToLNuQQ_NNPDF31_TuneCP5_13TeV-powheg", 49.997},
  {"WZTo1L1Nu2Q_13TeV_amcatnloFXFX_madspin", 10.71},
  {"WZTo1L3Nu_13TeV_amcatnloFXFX", 3.05},
  {"WZTo2L2Q_13TeV_amcatnloFXFX_madspin", 5.595},
  {"WZTo3LNu_TuneCP5_13TeV-amcatnloFXFX", 4.708},
  {"ZZTo2L2Nu_13TeV_powheg", 0.564},  // need to find this xs
  {"ZZTo2L2Q_13TeV_amcatnloFXFX_madspin", 3.22},
  {"ZZTo4L_13TeV_powheg", 1.212},

  {"JetHT_Run2017B-31Mar2018", 1.},
  {"JetHT_Run2017C-31Mar2018", 1.},
  {"JetHT_Run2017D-31Mar2018", 1.},
  {"JetHT_Run2017E-31Mar2018", 1.},
  {"JetHT_Run2017F-31Mar2018", 1.},

};

double transverse_mass(std::vector<double> p1, std::vector<double> p2) {
  return sqrt(pow(p1.at(0) + p2.at(0), 2) - pow(p1.at(1) + p2.at(1), 2) - pow(p1.at(2) + p2.at(2), 2));
}

#endif  // INTERFACE_UTIL_H_
