######################################################
#
# Prerequisite 1: Java & maven installed
# Prerequisite 2: CSlicer installed
# Prerequisite 3: GitFacts installed
#
######################################################

BUILD_DIR="target"
GIT_DIR=".git"
FACT_REL_PATH=".facts/20-deps"
GITFACTS_REL_PATH="target/release/gitfacts"
PROPERTIYFILE="cslicer.properties"
GITFACTSFILE="history.facts"
SLASH="/"
TAB=$'\t'

# Step 0: Read command line input
while [[ "$#" -gt 0 ]]; do
  case $1 in
    --repo_path) BASE_DIR="$2"; shift ;;
    --cslicer_path) CSLICER_PATH="$2"; shift ;;
    --gitfacts_path) GITFACTS_PATH="$2"; shift ;;
    *) echo "Unknown parameter passed: $1"; exit 1;;
  esac
  shift
done

# Step 1: Get current commit id
cd $BASE_DIR
CURRENTCOMMIT=$(git rev-parse HEAD)

# Step 2: Compile maven project
mvn compile

# Step 3: Create cslicer properties file
touch ${PROPERTIYFILE}
echo "repoPath = ${BASE_DIR}${SLASH}${GIT_DIR}" >> ${PROPERTIYFILE}
echo "classRoot = ${BASE_DIR}${SLASH}${BUILD_DIR}" >> ${PROPERTIYFILE}
echo "endCommit = ${CURRENTCOMMIT}" >> ${PROPERTIYFILE}

# Step 4: Run CSlicer to get fact files
java -jar ${CSLICER_PATH}  -c ${PROPERTIYFILE} -e dl --ext dep
wait $!

# Step 5: Append version to get versionised fact file
for f in "${BASE_DIR}${SLASH}${FACT_REL_PATH}"/*; do
    echo "Processing" "$f";
    sed -i '' -e "/^\/\//!s/$/$TAB$CURRENTCOMMIT/" "$f"
done

# Step 6: Run GitFacts to get git fact files
${GITFACTS_PATH}${SLASH}${GITFACTS_REL_PATH} ${BASE_DIR}
mv ${BASE_DIR}${SLASH}${GITFACTSFILE} ${BASE_DIR}${SLASH}${FACT_REL_PATH}
