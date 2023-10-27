#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <getopt.h>

char *var_name = NULL;
char *note_name = NULL;
char *output_file = NULL;
bool archived = true;

int index_of_file_extension(char *filename) {
    // This function returns the index of the last '.' character in the filename,
    // signaling the start of the file extension
    int index = -1;
    for (int i = 0; i < (int)strlen(filename); i++) {
        if (filename[i] == '.') {
            // If the current character is a '.', save its index
            index = i;
        }
    }
    return index;
}

bool is_txt_file(char *filename) {
    // This function returns true if the file is a text file and false otherwise
    int index = index_of_file_extension(filename);
    if (index == -1) {
        return false;
    }
    if (strlen(filename) - index == 4) {
        if (filename[index + 1] == 't' && filename[index + 2] == 'x' && filename[index + 3] == 't') {
            return true;
        }
    }
    return false;
}

bool file_has_an_extension(char *filename) {
    // This function returns true if the file has an extension and false otherwise
    int index = index_of_file_extension(filename);
    if (index == -1) {
        return false;
    }
    return true;
}

void print_help(char *program_name) {
    printf("Usage: %s [-o output_file] [-n note_name] [-v var_name] [-a] [-r] filename\n", program_name);
    printf("Options:\n");
    printf("  -o output_file\tSpecify the output file\n");
    printf("  -v var_name\t\tSet the variable name\n");
    printf("  -v var_name\t\tSet the variable name\n");
    printf("  -a\t\t\tStore in archive (default)\n");
    printf("  -r\t\t\tStore in RAM\n");
    printf("  -h\t\t\tPrint this help menu\n");
}

int main(int argc, char **argv) {
    // Parse the command line arguments
    int opt;
    while ((opt = getopt(argc, argv, "o:n:v:arh")) != -1) {
        switch (opt) {
            case 'o': // if -o is given, save the output file name
                output_file = optarg;
                break;
            case 'n': // if -n is given, save the note name
                note_name = optarg;
                break;
            case 'v': // if -v is given, save the variable name
                var_name = optarg;
                break;
            case 'a': // if -a is given, set archived to true
                archived = true;
                break;
            case 'r': // if -r is given, set archived to false
                archived = false;
                break;
            case 'h': // if -h is given, print help menu
                print_help(argv[0]);
                exit(EXIT_SUCCESS);
            default: // if an unknown option is given, print help menu
                fprintf(stderr, "Unknown option: %c\n", optopt);
                print_help(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        print_help(argv[0]);
        exit(EXIT_FAILURE);
    }

    char *txt_filepath = argv[optind];

    // Parse any remaining arguments
    for (int i = optind + 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) { // if -o is given, save the output file name
            output_file = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) { // if -n is given, save the note name
            note_name = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-v") == 0 && i + 1 < argc) { // if -v is given, save the variable name
            var_name = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-a") == 0) { // if -a is given, set archived to true
            archived = true;
        } else if (strcmp(argv[i], "-r") == 0) { // if -r is given, set archived to false
            archived = false;
        } else if (strcmp(argv[i], "-h") == 0) { // if -h is given, print help menu
            print_help(argv[0]);
            exit(EXIT_SUCCESS);
        } else { // if an unknown argument is given, print help menu
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            print_help(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // Find the last occurrence of the '/' character in the file path
    char *txt_filename = strrchr(txt_filepath, '/');
    if (txt_filename == NULL) {
        // The file path does not contain any '/' characters
        txt_filename = argv[1];
    } else {
        // The file path contains one or more '/' characters
        txt_filename++;
    }

    // Check if file is a suported format
    if (!is_txt_file(txt_filename)) {
        printf("Error: The file given is not in the proper format. Only '.txt' files are excepted!\n");
        exit(EXIT_FAILURE);
    }

    // Open the input file for reading
    FILE *txt_fp = fopen(txt_filepath, "rb");
    if (txt_fp == NULL) {
        printf("Error: could not open file '%s' for reading\n", txt_filename);
        exit(EXIT_FAILURE);
    }

    char trimmed_filename[25]; // 20 characters max + 4 for extension + 1 for null terminator

    // Copy the original filename to the trimmed filename buffer
    strncpy(trimmed_filename, txt_filename, 24);

    if (is_txt_file(trimmed_filename)) {
        // If the filename ends with '.txt', remove the '.txt' extension
        trimmed_filename[index_of_file_extension(trimmed_filename)] = '\0';
    } else {
        // If the filename does not end with '.txt', remove everything after the last '.' character
        // This will occur when filename is longer than 20 characters
        trimmed_filename[20] = '\0';
    }

    // Print message if variable name is too long, more than 8 characters
    if ((strlen(trimmed_filename) > 8 && var_name == NULL) || (var_name != NULL && strlen(var_name) > 8)) {
        printf("Error: The maximum allowed length of a variable on the calculator is 8 characters and you have exceded that, so only the first 8 characters will be used instead!\n");
    }

    FILE *fp;
    if (output_file == NULL) { // If no output file is given, use the input file name
        // Calculate output file name with extension
        char output_filename[256];
        strncpy(output_filename, txt_filename, index_of_file_extension(txt_filename));
        strcpy(output_filename + index_of_file_extension(txt_filename), ".8xv");

        // Open the output file for writing
        fp = fopen(output_filename, "wb+");
        if (fp == NULL) {
            printf("Error: could not open file for writing\n");
            exit(EXIT_FAILURE);
        }
    } else { // If output file is given, use that
        // Open the output file for writing
        fp = fopen(output_file, "wb+");
        if (fp == NULL) {
            printf("Error: could not open file for writing\n");
            exit(EXIT_FAILURE);
        }
    }

    // signature offset 0x0 (0) thru 0xa (10)
    unsigned char sig[] = {0x2a, 0x2a, 0x54, 0x49, 0x38, 0x33, 0x46, 0x2a, 0x1a, 0x0a, 0x00};
    for (unsigned long i = 0; i < sizeof(sig); i++) {
        fputc(sig[i], fp);
    }

    // comment offset 0xb (11) thru 0x34 (52)
    for (int i = 0; i < 42; i++) {
        fputc(0x00, fp);
    }

    // offset 0x35 (53) should have a value that is 0x39 (57) bytes less than the file size
    fputc(0x00, fp);
    fputc(0x00, fp);

    // data section
    // starting offset 0x37 (55)
    // ending offset 0x47 (71)
    // section size 0x11 (17) bytes long

    // offset 0x37 (55) has value of 0x0B (11) or 0x0D (13)
    fputc(0x0D, fp);
    fputc(0x00, fp);

    // length of variable data offset 0x39 (57) thru 0x3a (58)
    fputc(0x00, fp);
    fputc(0x00, fp);

    // variable type ID offset 0x3b (59) has value 0x15
    fputc(0x15, fp);

    // variable name offset 0x3c (60) thru 0x43 (67) padded with 0x00
    if (var_name != NULL) { // if variable name is given, use that instead of trimmed filename
        fprintf(fp, "%.*s", 8, var_name);
        if (strlen(var_name) < 8) {
            for (unsigned long i = 0; i < 8 - strlen(var_name); i++) {
                fputc(0x00, fp);
            }
        }
    } else { // otherwise use trimmed filename
        fprintf(fp, "%.*s", 8, trimmed_filename);
        if (strlen(trimmed_filename) < 8) {
            for (unsigned long i = 0; i < 8 - strlen(trimmed_filename); i++) {
                fputc(0x00, fp);
            }
        }
    }

    // version offset 0x44 (68) has value 0x00
    fputc(0x00, fp);

    // flag offset 0x45 (69) has value 0x80 (archived) or 0x00 (not archived)
    if (archived) {
        fputc(0x80, fp); // archived
    } else {
        fputc(0x00, fp); // ram
    }

    // length of variable data offset 0x46 (70) thru 0x47 (71)
    fputc(0x00, fp);
    fputc(0x00, fp);

    // offset 0x48 (72) thru 0x49 (73) is something
    fputc(0x00, fp);
    fputc(0x00, fp);
    
    // MATEO offset 0x4a (74) thru 0x4e (78)
    unsigned char before_filename[] = {0x4d, 0x41, 0x54, 0x45, 0x4f};
    for (unsigned long i = 0; i < sizeof(before_filename); i++) {
        fputc(before_filename[i], fp);
    }

    // Print message if filename or note name is too long, more than 19 characters
    if (strlen(txt_filename) > 19 && note_name == NULL) {
        printf("Error: The filename given is too long to be stored as the name for the note. Only the first 19 characters will be used instead!\n");
    } else if (note_name != NULL && strlen(note_name) > 19) {
        printf("Error: The note name given is too long to be stored so only the first 19 characters will be used instead!\n");
    }

    // filename offset 0x4f (79) thru 0x62 (98) padded with 0x0D
    if (note_name != NULL) { // if note name is given, use that instead of filename
        fprintf(fp, "%.*s", 20, note_name);
        if (strlen(note_name) < 20) {
            for (unsigned long i = 0; i < 20 - strlen(note_name); i++) {
                fputc(0x0D, fp);
            }
        }
    } else { // otherwise use filename
        fprintf(fp, "%.*s", 20, txt_filename);
        if (strlen(txt_filename) < 20) {
            for (unsigned long i = 0; i < 20 - strlen(txt_filename); i++) {
                fputc(0x0D, fp);
            }
        }
    }

    // text content offset 0x63 (99) thru 0x? (?)
    int c;
    while ((c = fgetc(txt_fp)) != EOF) {
        // calculator only accepts carriage returns, not newlines, so check and remove newlines
        if (c == 0x0A) continue; // skip newlines

        fputc(c, fp);
    }

    /*
    for (int i = 0; i < 0x10; i++) {
        fputc(i, fp);
    }
    */

    // calculate the size of the variable data and update its value in the file
    int var_data_size = ftell(fp) - 0x48;

    // update variable data size in the file at place 1
    fseek(fp, 0x39, SEEK_SET);
    fwrite(&var_data_size, 2, 1, fp);
    // update variable data size in the file at place 2
    fseek(fp, 0x46, SEEK_SET);
    fwrite(&var_data_size, 2, 1, fp);

    // update variable data size minus 0x2 in the file at place 3
    var_data_size -= 0x2;
    fseek(fp, 0x48, SEEK_SET);
    fwrite(&var_data_size, 2, 1, fp);

    // save file data to calculate checksum
    int data_size = ftell(fp) - 0x37;
    unsigned char data[data_size];

    // file checksum offset 0x? (?) thru 0x?+2 (?+2)
    fseek(fp, 0x37, SEEK_SET);
    // Read the data section into the buffer
    fread(data, 1, data_size, fp);

    // Calculate the sum of all the bytes in the data section
    int sum = 0;
    for (int i = 0; i < data_size; i++) {
        sum += data[i];
    }

    fseek(fp, 0, SEEK_END);

    // Get the lower 16 bits of the sum using bitwise operators
    unsigned short lower_16_bits = sum & 0xFFFF;

    // Set the file position indicator to the end of the file
    fseek(fp, 0, SEEK_END);

    // Write the lower 16 bits of the sum to the file
    fwrite(&lower_16_bits, sizeof(lower_16_bits), 1, fp);

    // calculate the file size of and update its value in the file
    fseek(fp, 0, SEEK_END);
    long int file_size = ftell(fp);
    file_size -= 0x39;
    fseek(fp, 0x35, SEEK_SET);
    fwrite(&file_size, 2, 1, fp);
    fseek(fp, 0, SEEK_END);

    // Close the files
    fclose(fp);
    fclose(txt_fp);
    
    return 0;
}
