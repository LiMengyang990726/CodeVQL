QUERY_FILE_PATH_PREFIX=$PWD
RUN_COMMAND_PATH="/Users/limengyang/Workspaces/FinalYearProject/codeqltosouffle/automationtools/run.sh"
GITFACTS_PATH="/Users/limengyang/Workspaces/FinalYearProject/ext-gitfacts"
CODEQLTOSOUFFLE_PATH="/Users/limengyang/Workspaces/FinalYearProject/codeqltosouffle"

OUTPUT_PATH_PREFIX="/Users/limengyang/Workspaces/FinalYearProject/experiementResult"
REPO_PATH_PREFIX="/Users/limengyang/Workspaces/FinalYearProject"

SLASH="/"

for f in $(find . -name '*.txt'); do 
    REPO_NAME="$(cut -d'/' -f2 <<<${f:1})"
    LEN_COUNT="$(cut -d'/' -f3 <<<${f:1})"
    FILE_NAME="$(cut -d'/' -f4 <<<${f:1})"
    DIR_NAME="$(cut -d'.' -f1 <<<$FILE_NAME)${LEN_COUNT: -1}"

    REPO_PATH="$REPO_PATH_PREFIX$SLASH$REPO_NAME" # third last param done
    OUTPUT_PATH="$OUTPUT_PATH_PREFIX$SLASH$REPO_NAME$SLASH$DIR_NAME" # second last param done
    QUERY_FILE_PATH="$QUERY_FILE_PATH_PREFIX${f:1}" # last param done

    RUN_COMMAND="/usr/bin/time -lp $RUN_COMMAND_PATH \ 
        --gitfacts_path $GITFACTS_PATH \ 
        --codeqltosouffle_path $CODEQLTOSOUFFLE_PATH \ 
        --repo_path $REPO_PATH \ 
        --output_path $OUTPUT_PATH \ 
        --query_file_path $QUERY_FILE_PATH"
    echo "$RUN_COMMAND\n"
    wait $!
done
