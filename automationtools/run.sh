######################################################
#
# Prerequisite: All the required parameters are ready
#
######################################################

FACTS_OUTPUT_REL_PATH=".facts/20-deps"
OUTPUT_REL_PATH="output"
SOUFFLE_FILE_REL_PATH="rules"
MAIN_DL_FILE="main.dl"

GITFACT_GENERATION_REL_PATH="automationtools/gitfact-generation.sh"
TRANSLATE_REL_PATH="automationtools/translate.sh"
MULTIVERSION_FACT_GENERATION_REL_PATH="automationtools/multiversion-fact-generation.sh"
SLASH="/"

# Step 0: Read command line input
while [[ "$#" -gt 0 ]]; do
  case $1 in
    --repo_path) REPO_PATH="$2"; shift ;;
    --gitfacts_path) GITFACTS_PATH="$2"; shift ;;
    --output_path) OUTPUT_PATH="$2"; shift ;;
    --query_file_path) QUERY_FILE_PATH="$2"; shift ;;
    --codeqltosouffle_path) CODEQLTOSOUFFLE_PATH="$2"; shift ;;
    --cslicer_path) CSLICER_PATH="$2"; shift ;;
    *) echo "Unknown parameter passed: $1"; exit 1;;
  esac
  shift
done

# Step 1: Run the file in sequence
mkdir -p ${OUTPUT_PATH}
cd ${OUTPUT_PATH}

${CODEQLTOSOUFFLE_PATH}${SLASH}${GITFACT_GENERATION_REL_PATH} --repo_path ${REPO_PATH} --gitfacts_path ${GITFACTS_PATH} --output_path ${OUTPUT_PATH}
wait $!
${CODEQLTOSOUFFLE_PATH}${SLASH}${TRANSLATE_REL_PATH} --query_file_path ${QUERY_FILE_PATH} --codeqltosouffle_path ${CODEQLTOSOUFFLE_PATH} --output_path ${OUTPUT_PATH}
wait $!
${CODEQLTOSOUFFLE_PATH}${SLASH}${MULTIVERSION_FACT_GENERATION_REL_PATH} --repo_path ${REPO_PATH}  --cslicer_path ${CSLICER_PATH} --codeqltosouffle_path ${CODEQLTOSOUFFLE_PATH} --output_path ${OUTPUT_PATH}
wait $!

mkdir ${OUTPUT_PATH}${SLASH}${OUTPUT_REL_PATH}
souffle -F ${OUTPUT_PATH}${SLASH}${FACTS_OUTPUT_REL_PATH} -D ${OUTPUT_PATH}${SLASH}${OUTPUT_REL_PATH} ${OUTPUT_PATH}${SLASH}${SOUFFLE_FILE_REL_PATH}${SLASH}${MAIN_DL_FILE}
wait $!

# Sample run
# ./run.sh --repo_path /Users/limengyang/Workspaces/FinalYearProject/docker-maven-plugin \
#          --gitfacts_path /Users/limengyang/Workspaces/FinalYearProject/ext-gitfacts \
#          --output_path /Users/limengyang/Desktop/test \
#          --query_file_path /Users/limengyang/Workspaces/FinalYearProject/codeqltosouffle/automationtools/callQuery.txt \
#          --codeqltosouffle_path /Users/limengyang/Workspaces/FinalYearProject/codeqltosouffle \
#          --cslicer_path /Users/limengyang/Workspaces/FinalYearProject/gitslice/target/cslicer-1.0.0-jar-with-dependencies.jar
