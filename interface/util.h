// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_UTIL_H_
#define INTERFACE_UTIL_H_

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
  {"2016", 30.59},
  {"2017", 41.5}
};

std::map<std::string, double> cross_sections = {
  {"DYJetsToLL_M-50_HT-100to200", 148.},
  {"DYJetsToLL_M-50_HT-200to400", 40.94},
  {"DYJetsToLL_M-50_HT-400to600", 5.497},
  {"DYJetsToLL_M-50_HT-600to800", 1.354},
  {"DYJetsToLL_M-50_HT-800to1200", 0.625},
  {"DYJetsToLL_M-50_HT-1200to2500", 0.151},
  {"DYJetsToLL_M-50_HT-2500toInf", 0.003647},
};


#endif  // INTERFACE_UTIL_H_
