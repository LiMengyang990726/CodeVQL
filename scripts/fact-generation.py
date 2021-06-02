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
runner = argparse.ArgumentParser(description='This script automate the process of generating program fact at specified version.')

# Add the arguments
runner.add_argument('--repo_path',
                    type=str,
                    help='the path to the program to be analyzed upon')
runner.add_argument('--output_path',
                    type=str,
                    help='the path to the analysis output')   
runner.add_argument('--cslicer_path',
                    type=str,
                    help='the path to the tool that generates program facts, install here(https://bitbucket.org/liyistc/gitslice/src/facts-dl4ql/)')            
runner.add_argument('--commit',
                    type=str,
                    help='the commit id to generate fact upon')

# Get all the input arguments and validate
args = runner.parse_args()

repo_path = args.repo_path
validatePath(repo_path)

output_path = args.output_path
validatePath(output_path)

cslicer_path = args.cslicer_path
validatePath(cslicer_path)

commit = args.commit

# Execute
# Step 1: Compile maven project
os.chdir(repo_path)
os.system("git checkout " + commit + "> /dev/null 2>&1")
os.system('mvn compile && test-compile > /dev/null 2>&1')

# Step 2: Create cslicer properties file
os.system('touch cslicer.properties')
os.system('echo "repoPath = %s/.git" >> cslicer.properties' % (repo_path))
os.system('echo "classRoot = %s/target" >> cslicer.properties' % (repo_path))

# Step 3: Run CSlicer to get fact files
start_time = time.time()
os.system('java -jar %s -c %s/cslicer.properties -e dl --ext dep > /dev/null 2>&1' % (cslicer_path, repo_path))
end_time = time.time()

# Step 4: Append version to get versionised fact file (Note: only for those inside 20-deps)
for path, _, files in os.walk(os.path.join(repo_path, ".facts/20-deps")):
    for name in files:
        file_name = os.path.join(path, name)
        with open(file_name, 'r') as fr:
            file_lines = [''.join([x.strip(), '\t' + commit, '\n']) for x in fr.readlines()]
        with open(file_name, 'w') as fw:
            fw.writelines(file_lines)
        original_file = open(file_name, 'r')
        new_combined_file = open(os.path.join(os.path.join(output_path, ".facts"), name), 'a+')
        new_combined_file.write(original_file.read())
        new_combined_file.seek(0)
        original_file.close()
        new_combined_file.close()

# Step 5: Remove intermediate files
os.remove(os.path.join(repo_path, "cslicer.properties"))
shutil.rmtree(os.path.join(repo_path, ".facts"))

time_usage = end_time - start_time
mem_usage = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss + resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
print(str(time_usage) + "\t" + str(mem_usage))
