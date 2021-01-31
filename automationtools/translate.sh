######################################################
#
# Prerequisite: CodeQLToSouffle is build
#
# Run process step 2
#
######################################################

SOUFFLE_FILE_REL_PATH="rules"
TRANSLATOR_FILE="./codeqltosouffle"
SLASH="/"

# Step 0: Read command line input
while [[ "$#" -gt 0 ]]; do
  case $1 in
    --query_file_path) QUERY_FILE_PATH="$2"; shift ;;
    --codeqltosouffle_path) CODEQLTOSOUFFLE_PATH="$2"; shift ;;
    --output_path) OUTPUT_PATH="$2"; shift ;;
    *) echo "Unknown parameter passed: $1"; exit 1;;
  esac
  shift
done

# Step 1: Run translation
cd ${CODEQLTOSOUFFLE_PATH}
mkdir -p ${SOUFFLE_FILE_REL_PATH}
${TRANSLATOR_FILE} ${QUERY_FILE_PATH}
mv ${CODEQLTOSOUFFLE_PATH}${SLASH}${SOUFFLE_FILE_REL_PATH} ${OUTPUT_PATH}${SLASH}${SOUFFLE_FILE_REL_PATH}

# Sample run
# ./translate.sh --query_file_path /Users/limengyang/Workspaces/FinalYearProject/automationtools/callQuery.txt --codeqltosouffle_path /Users/limengyang/Workspaces/FinalYearProject/codeqltosouffle --output_path /Users/limengyang/Desktop/test