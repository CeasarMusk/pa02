/*
I Ceasar Muscatella (ce447461) affirm that this program is entirely my own work and that I have neither developed my code with any another person, nor copied any code from any other person, nor permitted my code to be copied 
or otherwise used by any other person, nor have I copied, modified, or otherwise used programs created by others. I acknowledge that any violation of the above terms will be treated as academic dishonesty.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

void display_output(const char *data) {
    int length = strlen(data);
    for (int i = 0; i < length; i++) {
        putchar(data[i]);
        if ((i + 1) % 80 == 0) {
            putchar('\n');
        }
    }
    putchar('\n');
}

int calculate_padding(int length, int bits) {
    int multiple = (bits == 32) ? 4 : 2;
    int padding = 0;
    while (length % multiple != 0) {
        length++;
        padding++;
    }
    return (bits > 8) ? padding : 0;
}

void prepare_data_with_padding(char *output, const char *input, int bits) {
    int original_len = strlen(input);
    int padding = calculate_padding(original_len, bits);
    int new_len = original_len + padding;

    memcpy(output, input, original_len);
    memset(output + original_len, 'X', padding);
    output[new_len] = '\0';
}

int compute_checksum8(const unsigned char *data, int len) {
    int checksum = 0;
    for (int i = 0; i < len; i++) {
        checksum += data[i];
    }
    return checksum & 0xFF;
}

int compute_checksum16(const unsigned char *data, int len) {
    int checksum = 0;
    for (int i = 0; i < len; i += 2) {
        checksum += (data[i] << 8) | (data[i + 1] & 0xFF);
    }
    return checksum & 0xFFFF;
}

int compute_checksum32(const unsigned char *data, int len) {
    int checksum = 0;
    for (int i = 0; i < len; i += 4) {
        checksum += (data[i] << 24) | (data[i + 1] << 16) | (data[i + 2] << 8) | (data[i + 3] & 0xFF);
    }
    return checksum;
}

int calculate_checksum(const unsigned char *data, int len, int bits) {
    switch (bits) {
        case 8:
            return compute_checksum8(data, len);
        case 16:
            return compute_checksum16(data, len);
        case 32:
            return compute_checksum32(data, len);
        default:
            fprintf(stderr, "Valid checksum sizes are 8, 16, or 32\n");
            exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "\nPlease provide the correct parameters.\n"
                        "First parameter is the input file name, second is the checksum size.\n");
        return 1;
    }

    FILE *input_file = fopen(argv[1], "r");
    if (!input_file) {
        fprintf(stderr, "\nFile %s does not exist\n", argv[1]);
        return 1;
    }

    int checksum_bits = atoi(argv[2]);
    if (checksum_bits != 8 && checksum_bits != 16 && checksum_bits != 32) {
        fprintf(stderr, "\nValid checksum sizes are 8, 16, or 32\n");
        return 1;
    }

    char buffer[BUFFER_SIZE];
    if (!fgets(buffer, sizeof(buffer), input_file)) {
        fprintf(stderr, "Error reading file or file is empty\n");
        fclose(input_file);
        return 1;
    }
    fclose(input_file);

    char padded_data[BUFFER_SIZE * 2];
    prepare_data_with_padding(padded_data, buffer, checksum_bits);

    unsigned char *data_array = (unsigned char *)padded_data;
    int result_checksum = calculate_checksum(data_array, strlen(padded_data), checksum_bits);

    printf("\n%s\n%2d bit checksum is %08lx for all %4lu chars\n",
           padded_data, checksum_bits, result_checksum, strlen(padded_data));

    return 0;
}
