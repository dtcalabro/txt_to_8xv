# txt_to_8xv
This program is intented to convert text files to 8xv files so they can be used with the Text Editor CE application for compatible TI calculators. My main motivation for this project was to make a way to convert the text files on other operating systems such as MacOS, because previously the only way was to use a program only built for Windows.

## Building
To compile the program, simply run `make`. If compilation was successful, you should see that the txt_to_8xv executable has been created.

## Usage
To use the program, simply run the executable with the text file you would like to be converted as the  argument. The following additional options are available as well:
| Option      | Argument    | Description                |
|    :---:    |    :----:   | :---                       |
| -n          | note_name   | Set note name              |
| -v          | var_name    | Set variable name          |
| -a          | N/A         | Store in archive (default) |
| -r          | N/A         | Store in RAM               |
| -h          | N/A         | Print help menu            |

### Example
Example usage may look like the following:
| Command                                 | File     | Note Name | Var Name | Location |
| :---                                    |  :---:   |   :---:   |   :---:  |   :---:  |
| txt_to_8xv file.txt                     | file.8xv | file.txt  | file     | Archive  |
| txt_to_8xv /path/to/file.txt            | file.8xv | file.txt  | file     | Archive  |
| txt_to_8xv file.txt -n file             | file.8xv | file      | file     | Archive  |
| txt_to_8xv file.txt -v file1            | file.8xv | file.txt  | file1    | Archive  |
| txt_to_8xv file.txt -n file -v file1    | file.8xv | file      | file1    | Archive  |
| txt_to_8xv file.txt -n file -v file1 -a | file.8xv | file      | file1    | Archive  |
| txt_to_8xv file.txt -n file -v file1 -r | file.8xv | file      | file1    | RAM      |
| txt_to_8xv file.txt -n file -r          | file.8xv | file      | file     | RAM      |
| txt_to_8xv file.txt -n file -a          | file.8xv | file      | file     | Archive  |
| txt_to_8xv file.txt -a                  | file.8xv | file.txt  | file     | Archive  |
| txt_to_8xv file.txt -r                  | file.8xv | file.txt  | file     | RAM      |