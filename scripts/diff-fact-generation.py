import argparse
import os
import sys
import shutil
import resource
import time

def validatePath(path):
    if ((not os.path.isdir(path)) and (not os.path.isfile(path))):
        print('The path or file ' + path + ' specified does not exist')
        sys.exit()

# Create the parser
runner = argparse.ArgumentParser(description='This script automate the process of generating git facts.')

# Add the arguments
runner.add_argument('--repo_path',
                    type=str,
                    help='the path to the program to be analyzed upon')
runner.add_argument('--cslicer_path',
                    type=str,
                    help='the path to the tool that generates program facts, install here(https://bitbucket.org/liyistc/gitslice/src/facts-dl4ql/)')            
runner.add_argument('--output_path',
                    type=str,
                    help='the path to the analysis output')   

# Get all the input arguments and validate
args = runner.parse_args()

repo_path = args.repo_path
validatePath(repo_path)

cslicer_path = args.cslicer_path
validatePath(cslicer_path)

output_path = args.output_path
validatePath(output_path)

# Execute
interMsgFileName = os.path.join(output_path, "rules/interMsgForPipeline")
# Step 1: Read each line inside the inter message file created by the translator
if os.stat(interMsgFileName).st_size != 0:
    file = open(interMsgFileName, 'r')
    lines = file.readlines()
    count = 0
    start_time = time.time()
    for line in lines:
        results = line.strip().split("\t")
        if len(results) <= 2:
            continue
        else:
            startCommit = results[0]
            endCommit = results[1]
            # Step 2: Generate the properties file and Run gitslice
            os.chdir(repo_path)
            os.system('mvn compile && test-compile > /dev/null 2>&1')
            os.system('touch diff-%d.properties' % (count))
            os.system('echo "repoPath = %s/.git" >> diff-%d.properties' % (repo_path, count))
            os.system('echo "startCommit = %s" >> diff-%d.properties' % (startCommit, count))
            os.system('echo "endCommit = %s" >> diff-%d.properties' % (endCommit, count))
            os.system('echo "classRoot = %s/target" >> diff-%d.properties' % (repo_path, count))
            os.system('java -jar %s -c %s/diff-%d.properties -e dl --ext dep diff > /dev/null 2>&1' % (cslicer_path, repo_path, count))
            # Step 3: Move the needed files to the designated location
            for fact_type in results[2:]:
                fact_type += ".facts"
                fact_file = os.path.join(os.path.join(repo_path, ".facts/30-diffs"), fact_type)
                original_file = open(fact_file, 'r')
                new_combined_file = open(os.path.join(os.path.join(output_path, ".facts"), fact_type), 'a+')
                new_combined_file.write(original_file.read())
                new_combined_file.seek(0)
                original_file.close()
                new_combined_file.close()
            # Step 4: Remove intermediate files 
            os.remove(os.path.join(repo_path, 'diff-%d.properties' % (count)))
            shutil.rmtree(os.path.join(repo_path, ".facts"))
            count += 1
    end_time = time.time()

# Step 5: Count statistics
time_usage = end_time - start_time
mem_usage = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss + resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
print(str(time_usage) + "\t" + str(mem_usage))

