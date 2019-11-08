import os
import subprocess
import multiprocessing


def format_command(args, ifile):
    if not os.path.exists(args.output_dir):
        os.mkdir(args.output_dir)
    output_name = ifile.replace('.root', '_output.root').split('/')[-1]
    print output_name
    if args.local:
        args.ext = ''
    callstring = './{0} -i {1}{2} -o {3}/{4} -j {5} -t {6}'.format(
        args.exe, args.ext, ifile, args.output_dir, output_name, args.json, args.treename)
    if args.verbose:
        callstring += ' -v'
    if 'JetHT_Run' in ifile or 'muon' in ifile.lower() or 'electron' in ifile.lower() or 'data_output' in ifile.lower():
        callstring += ' --data'
    return callstring


def run_command(cmd, q, parallel=False):
    code = subprocess.call(cmd, shell=True)
    message = ''
    if code != 0:
        message = '\033[91m[ERROR] returned non-zero exit code while running {}\033[0m'.format(cmd)
    else:
        message = '\033[92m[SUCCESS] {} completed successfully \033[0m'.format(cmd)

    # print message
    print message

    # write to log or queue depending on if multiprocessing
    file_message = message[5:-5] # strip colors off message for file
    q.put(file_message) if parallel else q.write(file_message + '\n')
    return None


def listener(q, fname):
    '''listens for messages on the q, writes to file. '''

    with open(fname, 'w') as f:
        while 1:
            m = q.get()
            if m == 'kill':
                f.write('killed')
                break
            f.write(str(m) + '\n')
            f.flush()


def main(args):
    commands = []
    if args.local:
        file_list = [os.path.join(args.input_path, f) for f in os.listdir(args.input_path)
                     if os.path.isfile(os.path.join(args.input_path, f))]
        commands = [format_command(args, ifile) for ifile in file_list]
    else:
        search = ['xrdfs', 'root://cmseos.fnal.gov/', 'ls', args.input_path]
        commands = [
            format_command(args, ifile) for ifile in subprocess.check_output(search).split('\n') if '.root' in ifile
        ]

    if args.parallel:
        manager = multiprocessing.Manager()
        q = manager.Queue()

        # Use 5 cores if the machine has more than 10 total cores.
        # Otherwise, use half the available cores.
        n_processes = min(5, multiprocessing.cpu_count() / 2)

        pool = multiprocessing.Pool(processes=n_processes)
        watcher = pool.apply_async(listener, (q,'{}/runninglog.txt'.format(args.output_dir)))

        jobs = []
        for command in commands:
            job = pool.apply_async(run_command, (command, q, True))
            jobs.append(job)

        for job in jobs:
            job.get()

        q.put('kill')
        pool.close()
        pool.join()
    else:
        with open('{}/runninglog.txt'.format(args.output_dir), 'w') as ifile:
            [run_command(command, ifile, False) for command in commands]


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
    parser.add_argument('--treename', '-t', action='store', dest='treename',
                        default='ggNtuplizer/EventTree', help='name of tree')
    parser.add_argument('--output-dir', action='store', dest='output_dir',
                        default='output', help='name of output directory')
    parser.add_argument('--json', '-j', action='store', dest='json',
                        default='configs/test.json', help='name of json config')
    parser.add_argument('--verbose', action='store_true', dest='verbose')
    parser.add_argument('--parallel', action='store_true', dest='parallel', help='run in multiprocess')

    main(parser.parse_args())
