# What is this 'paper' branch ?
This 'paper' branch is a guarantee that reviewers have access to the specific version of fimpera we used to obtain the paper's results. We may (or may not) change the output format of scripts after the paper submission in the main branch, but this 'paper' branch is garenteed to stay correct wrt the result in the paper.

Note: each time you checkout to another branch (="change branch"), you **must** run
```bash
git submodule update --init --recursive
```
to let git update the submodules.

# repoduce the paper's results
## Installation
```
git clone --recursive https://github.com/lrobidou/fimpera  # download fimpera
cd fimpera
git checkout paper  # change branch to the 'apper' branch
git submodule update --init --recursive  # update submodules
chmod +x install.sh
./install.sh
```
`fimpera` requires Cmake >= 3.13.0 and a compiler supporting **version 17 or higher** of c++ standard. For instance, g++ 11.3.1 works fine, but version above 7 should be fine as well (see https://gcc.gnu.org/projects/cxx-status.html#cxx17 for more details).

## Data
The two tara ocean sequencing datasets are 
 * Indexed: reads from ERR1726642 https://www.ebi.ac.uk/ena/browser/view/ERR1726642
 * Queried: 10^5 reads from ERR4691696: https://www.ebi.ac.uk/ena/browser/view/ERR4691696

## Running
`fimpera` consists of 2 main executables, placed in the bin folder (`fimpera_index` and `fimpera_query`). We provide another executable to reproduce the paper results. To check the output of fimpera, we compare it to a hashtable. Hashtables are very sapce-consuming and should not be used in production. Binary using them are thus placed in another folder to discourage users from using them.

The executable used in the paper is `fimpera_truth_id_diff`. It prints matrixes to the standard output. Run it and redirect the output to a file to get a log of fimpera's queries results.
```
./build/app/fimpera_truth_id_diff query.fastq kmc_output_file.txt {size of the filter} {number of bits per abundance} -K {value of k} {z_values} > fimpera.log
```
Note: it may prints warnings on stderr (stderr is likely your console if you use most terminals). This is perfectly fine, it just informs you that that a read was too short to contains any k-mers, hence it was ignored.

The matrixes are as folows:
```
    |   x x x x x x
 y  |   x x x x x x
    |   x x x x x x
    |   x x x x x x
    |   x x x x x x
    |
    |-------------->
    v       x
 x: index (without fimpera)
    or
    ctruth (constructed fp+ construction overestimation)
    or
    index z (with fimpera)
 y: ground truth
 ```

looking at `matrix[x][y]` allows you to answer "when the ground truth is y, how many time was the answer {without fimpera / with fimpera} x ?" 


Once you have a log, you must then splits the log with regard to the z value.
Keep the log file to the root folder of fimpera before running the next script:
```bash
python scripts/matrix_divider.py fimpera.log
```
This will create one file per z value, named {z}.txt. Each file contains a reponse of fimepra for one z. Each line repsesents a correct abundance, and each number on that line represents the reported abundance by fimpera. We call those intermediate representations `abundance matrixes`.

Do not move {z}.txt files before running the next script.

Finally, using those matrixes, you can compute graphs presented in the paper:
```bash
python scripts/matrix_to_metrics.py {z values}
```
This will create three graphs per z value:
- a pie graph representing the false positive rate of fimpera (`{z}_fpr_proportion.png`)
- a pie graph representing the proportion of correct abundance, excluding false positive responses (`{z}_fpr_proportion.png`)
- a mustache plot representing, for each real abundance, what abundance fimepra reported (`{z}_mustache.png`)