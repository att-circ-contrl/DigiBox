#!/bin/bash

# This copies comment blocks immediately preceding function and variable
# declarations. It's a kludge, but a useful kludge.
helpers/document-perl.pl *pl > digibox-funcs.txt

# This is the end of the file.
