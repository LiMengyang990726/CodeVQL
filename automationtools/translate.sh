######################################################
#
# Prerequisite 1: Souffle installed
#
######################################################

OLDPWD=$PWD
FACT_REL_PATH=".facts/20-deps"
SOUFFLE_FILE_REL_PATH="rules"
QUERY_OUTPUT_REL_PATH="output"
TRANSLATOR_FILE="./codeqltosouffle"
VERSION_DL_FILE="./Version.dl"
MAIN_DL_FILE="./main.dl"
SLASH="/"

# Step 0: Read command line input
while [[ "$#" -gt 0 ]]; do
  case $1 in
    --repo_path) REPO_PATH="$2"; shift ;;
    --query_file_path) QUERY_FILE_PATH="$2"; shift ;;
    --codeqltosouffle_path) CODEQLTOSOUFFLE_PATH="$2"; shift ;;
    *) echo "Unknown parameter passed: $1"; exit 1;;
  esac
  shift
done

FACT_PATH="${REPO_PATH}${SLASH}${FACT_REL_PATH}"
QUERY_OUTPUT_PATH="${REPO_PATH}${SLASH}${QUERY_OUTPUT_REL_PATH}"

# Step 1: Run translation
cd $CODEQLTOSOUFFLE_PATH
mkdir -p ${SOUFFLE_FILE_REL_PATH}
$TRANSLATOR_FILE $QUERY_FILE_PATH
mv ${SOUFFLE_FILE_REL_PATH} ${REPO_PATH}

# Step 2: Run souffle
cd $REPO_PATH
mkdir -p ${QUERY_OUTPUT_REL_PATH}
cd "$REPO_PATH${SLASH}${SOUFFLE_FILE_REL_PATH}"

for i in $(find . -type f -iname "SelectedVersion*from.dl"); do
    souffle -F ${FACT_PATH} -D ${QUERY_OUTPUT_PATH} $i
    wait $!
    rm -rf $i
done

for i in $(find . -type f -iname "SelectedVersion*base.dl"); do
    souffle -F ${FACT_PATH} -D ${QUERY_OUTPUT_PATH} $i
    wait $!
    rm -rf $i
done

for i in $(find . -type f -iname "SelectedVersion*.dl"); do
    souffle -F ${FACT_PATH} -D ${QUERY_OUTPUT_PATH} $i
    wait $!
    rm -rf $i
done

souffle -F ${FACT_PATH} -D ${QUERY_OUTPUT_PATH} ${VERSION_DL_FILE}
souffle -F ${FACT_PATH} -D ${QUERY_OUTPUT_PATH} ${MAIN_DL_FILE}
