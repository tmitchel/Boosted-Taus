from subprocess import call
from glob import glob
import context

files = {
    'DataEl': ['SingleElectron*'],
    'DataMu': ['SingleMuon*'],
    'DYJets': ['DYJetsToLL_M-50_*'],
    'SingleTop': ['ST_t*'],
    'ttbar': ['TTTo*'],
    'WJets': ['WJetsToLNu_*'],
    'Diboson': ['WW', 'WZ', 'ZZ'],
}


def main(args):
    with context.cd('../'): # move out of ./scripts into base directory
        # move original outputs to a new directory
        call('mkdir {}/originals'.format(args.input))
        call('mv {0}/*.root {0}/originals'.format(args.input))

        # move logs into a new directory
        call('mkdir {}/logs'.format(args.input))
        call('mv {0}/*.log {0}/logs'.format(args.input))

        for name, pattern in files.iteritems():
            # append directory path to glob
            pattern = ['{}/originals/{}'.format(args.input, p) for p in pattern]

            # check if any files exist to be hadded
            matches = 0
            for p in pattern:
                matches += len(glob(p))

            # skip if none are found
            if matches == 0:
                print '\033[93m[WARNING] No files to hadd for {}. Skipping...\033[0m'.format(name)
                continue

            # if there are files to hadd, do it
            call('hadd {}/{}.root {}'.format(args.input, name, ' '.join(pattern)))


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--input', '-i', required=True, help='path to input files')
    main(parser.parse_args())
