import subprocess


def format_command(exe, ifile):
    output_name = ifile.replace('.root', '_output.root')
    callstring = './{} -i {} -o {}'.format(exe, ifile, output_name)
    return callstring


def main(args):
    search = ['ls', '-u', args.input_path]
    if not args.local:
        search = ['xrdfs', args.ext] + search

    file_list = [
        format_command(args.exe, ifile) for ifile in subprocess.check_output(search) if '.root' in ifile
    ]

    for callstring in file_list:
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
                        default='root://cmseos.fnal.gov/', help='reader extension')
    main(parser.parse_args())
