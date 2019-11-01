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
    'SingleElectron_Run2017B-31Mar2018',
    'SingleElectron_Run2017C-31Mar2018',
    'SingleElectron_Run2017D-31Mar2018',
    'SingleElectron_Run2017E-31Mar2018',
    'SingleElectron_Run2017F-31Mar2018',
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

def name_filter(search, name):
    if not '.root' in name:
        return False
    if search not in name.split('/')[-1]:
        return False

    return True


def main(args):
    search = ['xrdfs', 'root://cmseos.fnal.gov/', 'ls', '-u', args.input]
    file_dict = {
        sfile: [ifile for ifile in check_output(search).split('\n') if name_filter(sfile, ifile)]
        for sfile in files
    }

    out_search = ['xrdfs', 'root://cmseos.fnal.gov/', 'ls', '-u', args.output]
    existing = [ifile.split('/')[-1].replace('.root', '') for ifile in check_output(out_search).split('\n')]

    sep = ' '
    for fname, ifiles in file_dict.iteritems():
        if fname in existing:
            print '\033[93m[WARNING] File {} already exists. Skipping...\033[0m'.format(fname)
            continue

        if len(ifile) == 0:
            print '\033[91m[ERROR] No files were found matching name {}. Skipping...\033[0m'.format(fname)
            continue

        new_name = '{}.root'.format(fname)
        call('hadd {} {}'.format(new_name, sep.join(ifiles)), shell=True)
        call('xrdcp {} root://cmseos.fnal.gov/{}'.format(new_name, args.output), shell=True)
        call('rm {}'.format(new_name), shell=True)

if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--input', '-i', required=True, help='path to input files')
    parser.add_argument('--output', '-o', required=True, help='path to output files')
    main(parser.parse_args())
