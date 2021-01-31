######################################################
#
# Prerequisite: GitFacts installed
#
# Run process step 1
#
######################################################

GITFACTSFILE="history.facts"
FACTS_OUTPUT_REL_PATH_1=".facts"
FACTS_OUTPUT_REL_PATH_2="20-deps"
GITFACTS_REL_PATH="target/release/gitfacts"
SLASH="/"

# Step 0: Read command line input
while [[ "$#" -gt 0 ]]; do
  case $1 in
    --repo_path) REPO_PATH="$2"; shift ;;
    --gitfacts_path) GITFACTS_PATH="$2"; shift ;;
    --output_path) OUTPUT_PATH="$2"; shift ;;
    *) echo "Unknown parameter passed: $1"; exit 1;;
  esac
  shift
done

# Step 1: Generate git facts and move to output path
mkdir -p ${OUTPUT_PATH}${SLASH}${FACTS_OUTPUT_REL_PATH_1}
mkdir -p ${OUTPUT_PATH}${SLASH}${FACTS_OUTPUT_REL_PATH_1}${SLASH}${FACTS_OUTPUT_REL_PATH_2}
touch ${OUTPUT_PATH}${SLASH}${FACTS_OUTPUT_REL_PATH_1}${SLASH}${FACTS_OUTPUT_REL_PATH_2}${SLASH}${GITFACTSFILE}
${GITFACTS_PATH}${SLASH}${GITFACTS_REL_PATH} ${REPO_PATH} -o ${OUTPUT_PATH}${SLASH}${FACTS_OUTPUT_REL_PATH_1}${SLASH}${FACTS_OUTPUT_REL_PATH_2}${SLASH}${GITFACTSFILE}

# Sample run
# ./gitfact-generation.sh --repo_path /Users/limengyang/Workspaces/FinalYearProject/docker-maven-plugin --gitfacts_path /Users/limengyang/Workspaces/FinalYearProject/ext-gitfacts --output_path /Users/limengyang/Desktop/test
