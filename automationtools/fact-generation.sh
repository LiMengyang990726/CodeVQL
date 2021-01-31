######################################################
#
# Prerequisite: CSlicer installed
#
######################################################

BUILD_DIR="target"
GIT_DIR=".git"
FACT_REL_PATH=".facts/20-deps"
PROPERTIYFILE="cslicer.properties"
SLASH="/"
TAB=$'\t'

# Step 0: Read command line input
while [[ "$#" -gt 0 ]]; do
  case $1 in
    --repo_path) REPO_PATH="$2"; shift ;;
    --cslicer_path) CSLICER_PATH="$2"; shift ;;
    --output_path) OUTPUT_PATH="$2"; shift ;;
    --commit) COMMIT="$2"; shift ;;
    *) echo "Unknown parameter passed: $1"; exit 1;;
  esac
  shift
done

# Step 1: Compile maven project
cd ${REPO_PATH}
mvn compile

# Step 2: Create cslicer properties file
touch ${PROPERTIYFILE}
echo "repoPath = ${REPO_PATH}${SLASH}${GIT_DIR}" >> ${PROPERTIYFILE}
echo "classRoot = ${REPO_PATH}${SLASH}${BUILD_DIR}" >> ${PROPERTIYFILE}
echo "endCommit = ${COMMIT}" >> ${PROPERTIYFILE}

# Step 3: Run CSlicer to get fact files
java -jar ${CSLICER_PATH}  -c ${PROPERTIYFILE} -e dl --ext dep
wait $!

# Step 4: Append version to get versionised fact file
for f in "${REPO_PATH}${SLASH}${FACT_REL_PATH}"/*; do
    echo "Processing" "$f";
    sed -i '' -e "/^\/\//!s/$/$TAB$COMMIT/" "$f"
    F_BASENAME=$(basename $f)
    if test -f "${OUTPUT_PATH}${SLASH}${FACT_REL_PATH}${SLASH}${F_BASENAME}"; then
      cat $f >> ${OUTPUT_PATH}${SLASH}${FACT_REL_PATH}${SLASH}${F_BASENAME}
    else
      mv $f ${OUTPUT_PATH}${SLASH}${FACT_REL_PATH}
    fi
done
wait $!

# Step 5: Move the versionised fact files to the output dir
rm -rf ${PROPERTIYFILE}
rm -rf ${REPO_PATH}${SLASH}${FACT_REL_PATH}

# Sample run
# ./fact-generation.sh --repo_path /Users/limengyang/Workspaces/FinalYearProject/docker-maven-plugin --cslicer_path /Users/limengyang/Workspaces/FinalYearProject/gitslice/target/cslicer-1.0.0-jar-with-dependencies.jar --output_path /Users/limengyang/Desktop/test --commit e7fefdaddd99294d5842ef8c5b16070ce230d29e