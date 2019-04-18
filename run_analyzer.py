import subprocess
import multiprocessing


def format_command(args, ifile):
    output_name = ifile.replace('.root', '_output.root').split('/')[-1]
    print output_name
    callstring = './{} -i {}{} -o {}/{} -j {}'.format(args.exe, args.ext, ifile, args.output_dir, output_name, args.json)
    return callstring


def run_command(cmd):
    return subprocess.call(cmd, shell=True)


def main(args):
    search = ['ls', args.input_path]
    if not args.local:
      search = ['xrdfs', 'root://cmseos.fnal.gov/'] + search

    file_list = [
        format_command(args, ifile) for ifile in subprocess.check_output(search).split('\n') if '.root' in ifile
    ]

    # Use 4 cores if the machine has more than 8 total cores.
    # Otherwise, use half the available cores.
    n_processes = min(4, multiprocessing.cpu_count() / 2)
   
    pool = multiprocessing.Pool(processes=n_processes)
    r = pool.map_async(run_command, file_list)
    r.wait()


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
    parser.add_argument('--output-dir', action='store', dest='output_dir',
                        default='output', help='name of output directory')
    parser.add_argument('--json', '-j', action='store', dest='json',
                        default='configs/test.json', help='name of json config')
    main(parser.parse_args())
