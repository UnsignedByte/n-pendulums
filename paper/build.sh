#!/bin/bash

logFile="build.log"

touch "$logFile"

echo "Initial Build"
pdflatex paper.tex > "$logFile"

echo "Building ASY"
for f in ./*.asy; do
	echo "Building $f"
	asy "$f" > "$logFile"
done

echo "Building refs"
pdflatex paper.tex > "$logFile"
echo "Final Build"
pdflatex paper.tex > "$logFile"