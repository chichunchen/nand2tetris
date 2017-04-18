## JackAnalyzer
The first part is to tokenize the whole program and ignore tabs, spaces, and comments.
The second part is to compile the jack language to xml file.

### Usage
I use code::block on win7 for this project, so the easiest way is to use the same
enviornment to get the same results.

### Notes
`fgetc` works well on windows and can handle the newline in a simple way, while the newline is different
on Linux or MacOS, therefore, the tokenizer should be fix if were to run on those platforms.
