#! /bin/bash

######## Run this script from the root folder of GitHub repository ########
if [ $# -ne 1 ]; then
    echo "Usage: ./deliverable.sh <ASSIGNMENT_NAME>"
    exit 1
fi

# Hard-coded folder name
FOLDER_NAME="codebase"
# Accept the ASSIGNMENT_NAME as argument
ASSIGNMENT_NAME=$1

# Move files to deliverable directory
mkdir ../$FOLDER_NAME
cp -a . ../$FOLDER_NAME

# Change directory to deliverable
cd ../$FOLDER_NAME

# Remove unnesesary git files
rm -rf .git
rm .gitignore
rm README.md

# Remove other assignments
for folder in $(find . -type d -name "assignment_*"); do
    if [ $folder != "./$ASSIGNMENT_NAME" ]; then
        rm -rf $folder
    fi
done

# Remove the deliverable script
rm deliverable.sh
