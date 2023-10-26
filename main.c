#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <getopt.h>

char *var_name = "";
char *note_name = "";
bool archived = true;

int index_of_file_extension(char *filename) {
    int index = -1;
    for (int i = 0; i < strlen(filename); i++) {
        if (filename[i] == '.') {
            index = i;
        }
    }
    return index;
}

bool is_txt_file(char *filename) {
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
    int index = index_of_file_extension(filename);
    if (index == -1) {
        return false;
    }
    return true;
}

void print_help(char *program_name) {
    printf("Usage: %s [-n note_name] [-v var_name] [-a] [-r] filename\n", program_name);
    printf("Options:\n");
    printf("  -n note_name\tSet the note name\n");
    printf("  -v var_name\tSet the variable name\n");
    printf("  -a\t\tStore in archive (default)\n");
    printf("  -r\t\tStore in RAM\n");
    printf("  -h\t\tPrint this help menu\n");
}

int main(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "n:v:arh")) != -1) {
        switch (opt) {
            case 'n':
                note_name = optarg;
                break;
            case 'v':
                var_name = optarg;
                break;
            case 'a':
                archived = true;
                break;
            case 'r':
                archived = false;
                break;
            case 'h':
                print_help(argv[0]);
                exit(EXIT_SUCCESS);
            default:
                fprintf(stderr, "Unknown option: %c\n", optopt);
                print_help(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        print_help(argv[0]);
        exit(EXIT_FAILURE);
    }

    char *filename = argv[optind];

    // Parse any remaining arguments
    for (int i = optind + 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            note_name = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-v") == 0 && i + 1 < argc) {
            var_name = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-a") == 0) {
            archived = true;
        } else if (strcmp(argv[i], "-r") == 0) {
            archived = false;
        } else if (strcmp(argv[i], "-h") == 0) {
            print_help(argv[0]);
            exit(EXIT_SUCCESS);
        } else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            print_help(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // Find the last occurrence of the '/' character in the file path
    char *txt_filename = strrchr(argv[1], '/');
    if (txt_filename == NULL) {
        // The file path does not contain any '/' characters
        txt_filename = argv[1];
    } else {
        // The file path contains one or more '/' characters
        txt_filename++;
    }

    if (!is_txt_file(txt_filename)) {
        printf("Error: The file given is not in the proper format. Only '.txt' files are excepted!\n");
        return 1;
    }

    FILE *txt_fp = fopen(argv[1], "rb");
    if (txt_fp == NULL) {
        printf("Error: could not open file '%s' for reading\n", txt_filename);
        return 1;
    }

    //char trimmed_filename[256];
    //char trimmed_filename[9];
    char trimmed_filename[25]; // 20 characters max + 4 for extension + 1 for null terminator
    //int counter;

    // Copy the original filename to the trimmed filename buffer
    //strncpy(trimmed_filename, txt_filename, 8);
    strncpy(trimmed_filename, txt_filename, 24);

    if (is_txt_file(trimmed_filename)) {
        // If the filename ends with '.txt', remove the '.txt' extension
        trimmed_filename[index_of_file_extension(trimmed_filename)] = '\0';
    } else {
        // If the filename does not end with '.txt', remove everything after the last '.' character
        trimmed_filename[20] = '\0';
    }

    //printf("Original filename: %s\n", txt_filename);
    //printf("Trimmed filename: %s\n", trimmed_filename);

    if ((strlen(trimmed_filename) > 8 && var_name == NULL) || (strlen(var_name) > 8 && var_name != NULL)) {
        printf("Error: The maximum allowed length of a variable on the calculator is 8 characters and you have exceded that, so only the first 8 characters will be used instead!\n");
    }

    char output_filename[256];
    //char output_filename[24]; // 19 characters max + 4 for extension + 1 for null terminator
    strncpy(output_filename, txt_filename, index_of_file_extension(txt_filename));
    strcpy(output_filename + index_of_file_extension(txt_filename), ".8xv");

    //printf("Output filename: %s\n", output_filename);

    FILE *fp = fopen(output_filename, "wb+");
    if (fp == NULL) {
        printf("Error: could not open file for writing\n");
        return 1;
    }

    // signature offset 0x0 (0) thru 0xa (10)
    unsigned char sig[] = {0x2a, 0x2a, 0x54, 0x49, 0x38, 0x33, 0x46, 0x2a, 0x1a, 0x0a, 0x00};
    for (int i = 0; i < sizeof(sig); i++) {
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
    if (var_name != "") {
        fprintf(fp, "%.*s", 8, var_name);
        if (strlen(var_name) < 8) {
            for (int i = 0; i < 8 - strlen(var_name); i++) {
                fputc(0x00, fp);
            }
        }
    } else {
        fprintf(fp, "%.*s", 8, trimmed_filename);
        if (strlen(trimmed_filename) < 8) {
            for (int i = 0; i < 8 - strlen(trimmed_filename); i++) {
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
    for (int i = 0; i < sizeof(before_filename); i++) {
        fputc(before_filename[i], fp);
    }

    if (strlen(txt_filename) > 19 && note_name == "") {
        printf("Error: The filename given is too long to be stored as the name for the note. Only the first 19 characters will be used instead!\n");
    } else if (strlen(note_name) > 19 && note_name != "") {
        printf("Error: The note name given is too long to be stored so only the first 19 characters will be used instead!\n");
    }

    // filename offset 0x4f (79) thru 0x62 (98) padded with 0x0D
    if (note_name != "") {
        fprintf(fp, "%.*s", 20, note_name);
        if (strlen(note_name) < 20) {
            for (int i = 0; i < 20 - strlen(note_name); i++) {
                fputc(0x0D, fp);
            }
        }
    } else {
        fprintf(fp, "%.*s", 20, txt_filename);
        if (strlen(txt_filename) < 20) {
            for (int i = 0; i < 20 - strlen(txt_filename); i++) {
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

    // some weird end bytes offset 0x?+5 (?+5) thru 0x?+3 (?+3)
    unsigned char end_bytes[] = {0xff, 0x0d, 0x00};
    for (int i = 0; i < sizeof(end_bytes); i++) {
        fputc(end_bytes[i], fp);
    }

    fseek(fp, 0, SEEK_END);
    if (ftell(fp) < 0x48) {
        printf("Error: file is too small\n");
        return 1;
    }

    int var_data_size = ftell(fp) - 0x48;
    //printf("Variable data size: %d 0x%04X\n", var_data_size, var_data_size);

    fseek(fp, 0x39, SEEK_SET);
    fwrite(&var_data_size, 2, 1, fp);

    fseek(fp, 0x46, SEEK_SET);
    fwrite(&var_data_size, 2, 1, fp);

    var_data_size -= 0x2;
    fseek(fp, 0x48, SEEK_SET);
    fwrite(&var_data_size, 2, 1, fp);

    fseek(fp, 0, SEEK_END);
    if (ftell(fp) < 0x37) {
        printf("Error: file is too small\n");
        return 1;
    }

    int data_size = ftell(fp) - 0x37;
    //printf("Data size: %d 0x%04X\n", data_size, data_size);
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

    /*
    // Print the data to the console
    for (int i = 0; i < data_size; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");

    printf("sum: %i 0x%04X\n", sum, sum);
    */

    // Get the lower 16 bits of the sum using bitwise operators
    unsigned short lower_16_bits = sum & 0xFFFF;

    //printf("Lower 16 bits of sum: 0x%04X\n", lower_16_bits);

    // Set the file position indicator to the end of the file
    fseek(fp, 0, SEEK_END);

    // Write the lower 16 bits of the sum to the file
    fwrite(&lower_16_bits, sizeof(lower_16_bits), 1, fp);

    // calculate the file size of and update its value in the file
    fseek(fp, 0, SEEK_END);
    long int file_size = ftell(fp);
    //printf("File size: %ld 0x%x\n", file_size, file_size);
    //printf("File size - 0x39: %ld 0x%x\n", file_size - 0x39, file_size - 0x39);
    file_size -= 0x39;
    fseek(fp, 0x35, SEEK_SET);
    fwrite(&file_size, 2, 1, fp);
    fseek(fp, 0, SEEK_END);

    fclose(fp);
    fclose(txt_fp);
    
    return 0;
}