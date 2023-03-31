# Words Shares

Second-year Algorithmics project for Computer Science degree at University of Rouen(France).

# HELP

## USAGE  : 
`./ws [OPTION] ... FILES`

Print a list of words shared by text files.

## Program information :

- -h,--help	Print help message and exit.

## Input Control

- -i, --initial=VALUE	Set the maximal number of significant initial letters for words to VALUE. 0 means no limitation. Default is 63.

- -p, --punctuation-like-space	Interpret punctuation characters as spaces.

- -u, --uppercasing	Convert words to uppercase.

## Output Control

- -s, --same-numbers	Also print all the words that have the same number of shares as the last word printed.

- -t, --top=VALUE	Set the maximal number of words to print to VALUE. 0 means no limitation. Default is 10.

The number of FILES that can be taken into account is between 2 and 63.
