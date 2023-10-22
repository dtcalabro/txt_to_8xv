# txt_to_8xv
This program is intented to convert text files to 8xv files so they can be used with the Text Editor CE application for compatible TI calculators. My main motivation for this project was to make a way to convert the text files on other operating systems, like MacOS, because previously the only way was to use a program built only for Windows.

## Building
To compile the program, run the following command ...
```bash
make
```
... if compilation was successful, you should see that the txt_to_8xv executable has been created.

## Usage
To use the program, simply run the executable with the text file you would like to be converted as the first argument.
### Example
Examples may look like the following ...
```bash
txt_to_8xv file.txt
```
OR
```bash
txt_to_8xv /path/to/file.txt
```
... which would produce the file "file.8xv" which can then be uploaded to the calculator.