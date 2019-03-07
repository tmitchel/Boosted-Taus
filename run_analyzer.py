import subprocess


def format_command(exe, ext, ifile):
    output_name = ifile.replace('.root', '_output.root').split('/')[-1]
    callstring = './{} -i {}{} -o {}'.format(exe, ext, ifile, output_name)
    return callstring


def main(args):
    search = ['ls', args.input_path]
    if not args.local:
      search = ['xrdfs', 'root://cmseos.fnal.gov/'] + search

    file_list = [
        format_command(args.exe, args.ext, ifile) for ifile in subprocess.check_output(search).split('\n') if '.root' in ifile
    ]

    for callstring in file_list:
        print 'Calling.... {}'.format(callstring)
        subprocess.call(callstring, shell=True)


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser(
        description='Run an executable on all files in a directory')
    parser.add_argument('--exe', '-e', action='store',
                        dest='exe', help='name of executable')
    parser.add_argument('--input-path', '-i', action='store',
                        dest='input_path', help='path to input')
    parser.add_argument('--local', '-l', action='store_true',
                        dest='local', help='is this a local file')
    parser.add_argument('--ext', '-x', action='store', dest='ext',
                        default='root://cmsxrootd.fnal.gov/', help='reader extension')
    main(parser.parse_args())
