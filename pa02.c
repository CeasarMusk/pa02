#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage() {
    fprintf(stderr, "Usage: ./checksum <filename> <checksum size>\n");
    fprintf(stderr, "Valid checksum sizes are 8, 16, or 32\n");
    exit(EXIT_FAILURE);
}

unsigned long calculate_checksum(FILE *file, int checksum_size) {
    unsigned long checksum = 0;
    int c;
    int count = 0;
    while ((c = fgetc(file)) != EOF) {
        checksum += c;
        count++;
    }
    // Add padding if necessary
    while (count % (checksum_size / 8) != 0) {
        checksum += 'X';
        count++;
    }
    // Mask the checksum to the appropriate size
    if (checksum_size == 8) {
        checksum &= 0xFF;
    } else if (checksum_size == 16) {
        checksum &= 0xFFFF;
    } else if (checksum_size == 32) {
        checksum &= 0xFFFFFFFF;
    }
    return checksum;
}

void print_file_content(FILE *file) {
    rewind(file); // Rewind the file to the beginning
    int c;
    int count = 0;
    while ((c = fgetc(file)) != EOF) {
        putchar(c);
        count++;
        if (count % 80 == 0) {
            putchar('\n');
        }
    }
    // If the last line has fewer than 80 characters, add a newline
    if (count % 80 != 0) {
        putchar('\n');
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage();
    }

    char *filename = argv[1];
    int checksum_size = atoi(argv[2]);
    if (checksum_size != 8 && checksum_size != 16 && checksum_size != 32) {
        fprintf(stderr, "Valid checksum sizes are 8, 16, or 32\n");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    printf("File content:\n");
    print_file_content(file);

    unsigned long checksum = calculate_checksum(file, checksum_size);

    fseek(file, 0, SEEK_END);
    int characterCnt = ftell(file);
    fclose(file);

    printf("%2d bit checksum is %8lx for all %4d chars\n", checksum_size, checksum, characterCnt);

    return 0;
}
