import pprint
from subprocess import call, check_output

files = [
    'DYJetsToLL_M-50_HT-100to200',
    'DYJetsToLL_M-50_HT-200to400',
    'DYJetsToLL_M-50_HT-400to600',
    'DYJetsToLL_M-50_HT-600to800',
    'DYJetsToLL_M-50_HT-800to1200',
    'DYJetsToLL_M-50_HT-1200to2500',
    'DYJetsToLL_M-50_HT-2500toInf',
    'DYJetsToLL_M-50_v',
    'ST_t-channel_antitop',
    'ST_t-channel_top',
    'ST_tW_antitop',
    'ST_tW_top',
    'SingleMuon_Run2017B-31Mar2018',
    'SingleMuon_Run2017C-31Mar2018',
    'SingleMuon_Run2017D-31Mar2018',
    'SingleMuon_Run2017E-31Mar2018',
    'SingleMuon_Run2017F-31Mar2018',
    'TTTo2L2Nu',
    'TTToHadronic',
    'TTToSemiLeptonic',
    'WJetsToLNu_HT-100To200',
    'WJetsToLNu_HT-200To400',
    'WJetsToLNu_HT-400To600',
    'WJetsToLNu_HT-600To800',
    'WJetsToLNu_HT-800To1200',
    'WJetsToLNu_HT-1200To2500',
    'WJetsToLNu_HT-2500ToInf',
    'WJetsToLNu_Inc',
    'WW',
    'WZ',
    'ZZ',
]


def main(args):
    search = ['xrdfs', 'root://cmseos.fnal.gov/', 'ls', args.input]
    file_dict = {
        sfile: [ifile for ifile in check_output(search).split('\n') if '.root' in ifile and ifile in sfile]
        for sfile in files
    }
    pprint.pprint(file_dict)

if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--input', '-i', required=True, help='path to input files')
    parser.add_argument('--output', '-o', required=True, help='path to output files')
    main(parser.parse_args())
