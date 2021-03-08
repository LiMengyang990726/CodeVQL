######################################################
#
# Prerequisite: Get unique commit executable file exists
#
# Run process step 3
#
######################################################

SOUFFLE_FILE_REL_PATH="rules"
FACTS_OUTPUT_REL_PATH=".facts/20-deps"
GET_UNIQUE_COMMITS_FROM_FACTS_FILE_REL_PATH="automationtools/get-unique-commits-from-facts-file"
FACT_GENERATION_REL_PATH="automationtools/fact-generation.sh"
VERSION_DL_FILE="./Version.dl"
VERSION_COMB_FACTS_FILE="VersionComb.facts"
SLASH="/"

# Util Function for Generating Progress Bar
generate_progress_bar() {
  total=$1
  curr=$2
  output=""

  for ((i=0;i<$curr;i++))
  do 
    output="$output#"
  done

  for ((i=$curr;i<$total;i++))
  do
    output="$output."
  done
  percentage=$(bc <<< "scale=2;$curr/$total")
  percentage=$(bc <<< "scale=2;$percentage*100")
  output="$output[$percentage%]\r"
  echo "progress bar is called"
  echo -ne "$output"
}

# Step 0: Read command line input
while [[ "$#" -gt 0 ]]; do
  case $1 in
    --repo_path) REPO_PATH="$2"; shift ;;
    --cslicer_path) CSLICER_PATH="$2"; shift ;;
    --codeqltosouffle_path) CODEQLTOSOUFFLE_PATH="$2"; shift ;;
    --output_path) OUTPUT_PATH="$2"; shift ;;
    *) echo "Unknown parameter passed: $1"; exit 1;;
  esac
  shift
done

# Step 1: Execute version related souffle files to generate facts in the required sequence
cd ${OUTPUT_PATH}${SLASH}${SOUFFLE_FILE_REL_PATH}

for i in $(find . -type f -iname "SelectedVersion*from.dl"); do
    souffle -F ${OUTPUT_PATH}${SLASH}${FACTS_OUTPUT_REL_PATH} $i
    wait $!
    rm -rf $i
    wait $!
done

for i in $(find . -type f -iname "SelectedVersion*base.dl"); do
    souffle -F ${OUTPUT_PATH}${SLASH}${FACTS_OUTPUT_REL_PATH} $i
    wait $!
    rm -rf $i
    wait $!
done

for i in $(find . -type f -iname "SelectedVersion*.dl"); do
    souffle -F ${OUTPUT_PATH}${SLASH}${FACTS_OUTPUT_REL_PATH} $i
    wait $!
    rm -rf $i
    wait $!
done

souffle -F ${OUTPUT_PATH}${SLASH}${FACTS_OUTPUT_REL_PATH} ${VERSION_DL_FILE}

# Step 2: Check out each of the unique commit
cd ${REPO_PATH}
ORIGINAL_COMMIT=$(git rev-parse HEAD)
LENGTH=$(wc -l /Users/limengyang/Workspaces/FinalYearProject/codeqltosouffle/automationtools/run.py)
LENGTH=($LENGTH)
LENGTH=${LENGTH[0]}
CURR_COUNT=1
for commit in $(${CODEQLTOSOUFFLE_PATH}${SLASH}${GET_UNIQUE_COMMITS_FROM_FACTS_FILE_REL_PATH} ${OUTPUT_PATH}${SLASH}${FACTS_OUTPUT_REL_PATH}${SLASH}${VERSION_COMB_FACTS_FILE}); do
    generate_progress_bar $LENGTH $CURR_COUNT
    wait $!
    git checkout ${commit}
    ${CODEQLTOSOUFFLE_PATH}${SLASH}${FACT_GENERATION_REL_PATH} --repo_path ${REPO_PATH} --cslicer_path ${CSLICER_PATH} --output_path ${OUTPUT_PATH} --commit ${commit}
    wait $!
    ((CURR_COUNT++))
done
git checkout ${ORIGINAL_COMMIT}

# Sample run
# ./multiversion-fact-generation.sh --repo_path /Users/limengyang/Workspaces/FinalYearProject/docker-maven-plugin  --cslicer_path /Users/limengyang/Workspaces/FinalYearProject/gitslice/target/cslicer-1.0.0-jar-with-dependencies.jar --codeqltosouffle_path /Users/limengyang/Workspaces/FinalYearProject/codeqltosouffle --output_path /Users/limengyang/Desktop/test
