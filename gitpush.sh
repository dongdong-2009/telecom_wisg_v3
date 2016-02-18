#!/bin/sh
if [ $# -eq 0 ]
then 
    echo "please input commit message!"
    exit
else
    git add .
    git commit -m "$1"    
    git push -u origin debug --all
fi
