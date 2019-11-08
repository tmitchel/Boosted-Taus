import numpy
import utils
import uproot
from glob import glob

bkg = [
    'Diboson', 'DYJets', 'SingleTop', 'ttbar', 'WJets',
]
hist_names = [
    'OS_pass', 'OS_fail', 'SS_pass', 'SS_fail',
    'OS_anti_pass', 'OS_anti_fail', 'SS_anti_pass', 'SS_anti_fail',
]
variance_names = ['{}_var'.format(hname) for hname in hist_names]


def update_hist_names(var):
    for i in range(len(hist_names)):
        hist_names[i] = '{}_{}'.format(var, hist_names[i])


def parse_data(input_dir, var):
    """
    Get binning information for use later and initialize QCD histograms
    with the values/variances from data.
    """
    f = uproot.open('{}/Data.root'.format(input_dir))
    histograms = {}

    for (hname, hvar) in zip(hist_names, variance_names):
        histograms[hname] = f['{}/{}'.format(hname, var)].values
        histograms[hvar] = f['{}/{}'.format(hname, var)].variances

    return histograms, f['OS_pass/'+var].edges


def parse_file(ifile, histograms, var):
    """
    Subtract backgrounds from data to get QCD template. Also, handle
    uncertainty propogation
    """
    f = uproot.open(ifile)
    for (hname, hvar) in zip(hist_names, variance_names):
        histograms[hname] = histograms[hname] - f['{}/{}'.format(hname, var)].values
        histograms[hvar] = histograms[hvar] + f['{}/{}'.format(hname, var)].variances

    return histograms


def form_histograms(histograms, binning, varname):
    """
    Parse values, variances, and binnings to produce final QCD histograms
    """
    for name, vals in histograms.iteritems():
        # warn user of any histograms with negative bin content
        if numpy.any(vals[0] < 0):
            print '\033[93m[WARNING] Negative value found in {}. Will be set to 0...\033[0m'.format(name)
    return {
        name+'_'+varname: utils.from_array(name+'_'+varname, vals[0], vals[1], binning)
        for name, vals in histograms.iteritems()
    }


def valid_name(name):
    """
    Only process files needed for the OS/SS ratio
    """
    if 'data' in name.lower():
        return False
    elif 'qcd' in name.lower():
        return False
    return True


def scale_histograms(histograms):
    """
    Calculate OS/SS ratio then apply to pass/fail and proprogate uncertainty
    """
    passing_ratio = histograms['OS_anti_pass'].sum() / histograms['SS_anti_pass'].sum()
    failing_ratio = histograms['OS_anti_fail'].sum() / histograms['SS_anti_fail'].sum()
    return {
        'OS_pass': (histograms['SS_pass'] * passing_ratio, histograms['SS_pass_var'] * passing_ratio * passing_ratio),
        'OS_fail': (histograms['SS_fail'] * failing_ratio, histograms['SS_fail_var'] * failing_ratio * failing_ratio),
    }


def build_qcd(args, write_output=False):
    """
    Build passing/failing histograms for QCD using OS/SS ratio method.
    Parameters:
        args (class): dictionary containing all options
        Fields:
            input_dir (string): path to input files
            variable (list(string)): variables to for which we need QCD histograms
        write_output (boolean): flag determining if an output ROOT file is created. File
            will be named QCD.root and placed in the input directory.
    Returns:
        filled (list(dict(string: hist))): dictionaries mapping variable names to QCD
            histograms that have been created
    """
    filelist = sorted([
        ifile for ifile in glob('{}/*.root'.format(args.input_dir)) if valid_name(ifile)
    ])
    print filelist

    filled = []
    for var in args.variable:
        histograms, binning = parse_data(args.input_dir, var)
        for ifile in filelist:
            histograms = parse_file(ifile, histograms, var)

        histograms = scale_histograms(histograms)

        formatted = form_histograms(histograms, binning, var)

        filled.append(formatted)

    if write_output:
        with uproot.recreate('{}/QCD.root'.format(args.input_dir)) as output:
            for hists in filled:
                for name, hist in hists.iteritems():
                    output[name] = hist

    return filled


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('--input-dir', '-i', required=True, dest='input_dir', help='path to input files')
    parser.add_argument('--variable', '-v', nargs='+', required=True, dest='variable', help='variable to plot')
    build_qcd(parser.parse_args(), write_output=True)
