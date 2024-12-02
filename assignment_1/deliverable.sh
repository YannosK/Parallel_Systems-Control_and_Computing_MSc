#! /bin/bash

######## Run this script from the root folder of GitHub repository ########

FOLDER_NAME="codebase"
ASSIGNMENT_NAME="assignment_1"

# Move files to deliverable directory
mkdir ../$FOLDER_NAME
cp -a . ../$FOLDER_NAME

# Change directory to deliverable
cd ../$FOLDER_NAME

# Remove unnesesary git files
rm -rf .git
rm .gitignore
rm README.md

rm $ASSIGNMENT_NAME/deliverable.sh
