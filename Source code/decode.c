#include <stdio.h>
#include <string.h>

#include "decode.h"
#include "types.h"


/* ===================== ANSI Terminal Colors ===================== */

#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"


// Read and validate the command-line arguments required for decoding.
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Check whether the input stego image has .bmp extension.
    if (strstr(argv[2], ".bmp") != NULL)
    {
        decInfo->stego_image_fname = argv[2];
    }
    else
    {
        printf(RED "ERROR: Stego image should be .bmp file\n" RESET);
        return e_failure;
    }

    // Get the output file name.
    // If the user gives an output name, use that name.
    // Otherwise use "output" as default name.
    if (argv[3] != NULL)
    {
        decInfo->output_fname = argv[3];
    }
    else
    {
        decInfo->output_fname = "output";
    }

    printf(GREEN "INFO : SUCCESS : Decode arguments validated\n" RESET);

    return e_success;
}


// Open the stego image in read-binary mode.
Status open_decode_files(DecodeInfo *decInfo)
{
    // Open the stego image for reading.
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");


    // Check whether the file was opened successfully.
    if (decInfo->fptr_stego_image == NULL)
    {
        printf(RED "ERROR: Unable to open stego image\n" RESET);

        return e_failure;
    }

    printf(GREEN "INFO : SUCCESS : Stego image opened\n" RESET);

    return e_success;
}


// Skip the 54-byte BMP header.
// The encoded secret data starts after the BMP header.
Status skip_bmp_header(FILE *fptr_stego_image)
{
    // Move the file pointer to byte 54.
    if (fseek(fptr_stego_image, 54, SEEK_SET) != 0)
    {
        printf(RED "ERROR: Unable to skip BMP header\n" RESET);

        return e_failure;
    }

    printf(GREEN "INFO : SUCCESS : BMP Header skipped\n" RESET);

    return e_success;
}


// Decode one character from 8 image bytes.
// One bit is stored in the LSB of each image byte.
char decode_byte_from_lsb(char *image_buffer)
{
    char data = 0;

    // Read one bit from each of the 8 image bytes.
    for (int i = 0; i < 8; i++)
    {
        // Get the LSB of the image byte.
        int bit = image_buffer[i] & 1;

        // Store the bit in the correct position of the decoded character.
        data = data | (bit << i);
    }

    return data;
}


// Decode data from the stego image. Each character requires 8 image bytes.
Status decode_data_from_image(char *data, int size, FILE *fptr_stego_image)
{
    char image_buffer[8];

    // Decode each character one by one.
    for (int i = 0; i < size; i++)
    {
        // Read 8 image bytes.
        if (fread(image_buffer, 8, 1, fptr_stego_image) != 1)
        {
            printf(RED "ERROR: Unable to read image data\n" RESET);

            return e_failure;
        }

        // Decode one character from the 8 image bytes.
        data[i] = decode_byte_from_lsb(image_buffer);
    }

    return e_success;
}


// Decode the size of the magic string.
// The size is stored before the actual magic string.
Status decode_magic_string_size(DecodeInfo *decInfo)
{
    int size;
    
    //Decode the magic string size from the stego image.
    if (decode_data_from_image((char *)&size, sizeof(int), decInfo->fptr_stego_image) != e_success)
    {
        printf(RED "ERROR: Unable to decode magic string size\n" RESET);

        return e_failure;
    }

    // Check whether the decoded size is valid.
    if (size <= 0 || size >= MAX_MAGIC_STRING)
    {
        printf(RED "ERROR: Invalid magic string size\n" RESET);

        return e_failure;
    }

    // Store the magic string size inside DecodeInfo structure.
    decInfo->magic_string_size = size;

    printf(CYAN "Magic String Size : %d\n" RESET, size);

    return e_success;
}


// Decode the magic string from the image and compare it with the string entered by the user.
Status decode_magic_string(DecodeInfo *decInfo)
{
    int size = decInfo->magic_string_size;
    char magic_string[30];

    // Decode the magic string from the stego image.
    if (decode_data_from_image( decInfo->magic_string, size, decInfo->fptr_stego_image) != e_success)
    {
        printf(RED "ERROR: Unable to decode magic string\n" RESET);

        return e_failure;
    }

    // Add NULL character at the end to make it a valid C string.
    decInfo->magic_string[size] = '\0';


    // Ask the user to enter the magic string.
    printf(YELLOW "Enter the Magic String : " RESET);

    scanf("%29s", magic_string);


    // Compare the user-entered magic string with the decoded magic string.
    if (strcmp(magic_string, decInfo->magic_string) != 0)
    {
        printf(RED "ERROR: Magic String does not match\n" RESET);

        return e_failure;
    }

    printf(GREEN "Magic String matched successfully\n" RESET);

    return e_success;
}


// Decode the size of the secret file extension.
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    // Decode the extension size from the stego image.
    if (decode_data_from_image((char *)&decInfo->secret_file_extn_size, sizeof(int), decInfo->fptr_stego_image) != e_success)
    {
        printf(RED "ERROR: Unable to decode extension size\n" RESET);

        return e_failure;
    }

    // Check whether the extension size is valid.
    if (decInfo->secret_file_extn_size <= 0 || decInfo->secret_file_extn_size >= MAX_FILE_SUFFIX)
    {
        printf(RED "ERROR: Invalid extension size\n" RESET);

        return e_failure;
    }

    printf(CYAN "Secret File Extension Size : %d\n" RESET, decInfo->secret_file_extn_size);

    return e_success;
}


// Decode the secret file extension.
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    int size = decInfo->secret_file_extn_size;

    // Decode the extension from the stego image.
    if (decode_data_from_image( decInfo->secret_file_extn, size, decInfo->fptr_stego_image) != e_success)
    {
        printf(RED "ERROR: Unable to decode file extension\n" RESET);

        return e_failure;
    }

    // Add NULL character at the end of the extension string.
    decInfo->secret_file_extn[size] = '\0';

    printf(CYAN "Secret File Extension : %s\n" RESET, decInfo->secret_file_extn);

    return e_success;
}


// Decode the size of the secret file.
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    // Decode the secret file size from the stego image.
    if (decode_data_from_image((char *)&decInfo->size_secret_file, sizeof(long), decInfo->fptr_stego_image) != e_success)
    {
        printf(RED "ERROR: Unable to decode secret file size\n" RESET);

        return e_failure;
    }

    // Check whether the decoded file size is valid.
    if (decInfo->size_secret_file < 0)
    {
        printf(RED "ERROR: Invalid secret file size\n" RESET);

        return e_failure;
    }

    printf(CYAN "Secret File Size : %ld bytes\n" RESET, decInfo->size_secret_file);

    return e_success;
}


// Decode the actual secret file data and write it into the output file.
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char data;

    // Decode the secret file one byte at a time.
    for (long i = 0; i < decInfo->size_secret_file; i++)
    {
        // Decode one byte from the image.
        if (decode_data_from_image( &data, 1, decInfo->fptr_stego_image) != e_success)
        {
            return e_failure;
        }

        // Write the decoded byte into the output file.
        if (fwrite(&data, 1, 1, decInfo->fptr_output) != 1)
        {
            printf(RED "ERROR: Unable to write output file\n" RESET);

            return e_failure;
        }
    }

    printf(GREEN "INFO : SUCCESS : Secret file data decoded\n" RESET);

    return e_success;
}


// Close all files used during decoding.
void close_decode_files(DecodeInfo *decInfo)
{
    // Close the stego image file.
    if (decInfo->fptr_stego_image != NULL)
    {
        fclose(decInfo->fptr_stego_image);
        decInfo->fptr_stego_image = NULL;
    }

    // Close the output file.
    if (decInfo->fptr_output != NULL)
    {
        fclose(decInfo->fptr_output);
        decInfo->fptr_output = NULL;
    }
}


// Complete decoding process.
// This function calls all the decoding functions in the correct order.
Status do_decoding(DecodeInfo *decInfo)
{
    char output_file[100];

    // Open the stego image.
    if (open_decode_files(decInfo) != e_success)
    {
        return e_failure;
    }

    // Skip the 54-byte BMP header.
    if (skip_bmp_header(decInfo->fptr_stego_image) != e_success)
    {
        close_decode_files(decInfo);
        return e_failure;
    }

    // Decode the magic string size.
    if (decode_magic_string_size(decInfo) != e_success)
    {
        close_decode_files(decInfo);
        return e_failure;
    }

    // Decode the magic string and check whether it matches.
    if (decode_magic_string(decInfo) != e_success)
    {
        close_decode_files(decInfo);
        return e_failure;
    }

    // Decode the secret file extension size.
    if (decode_secret_file_extn_size(decInfo) != e_success)
    {
        close_decode_files(decInfo);
        return e_failure;
    }

    // Decode the secret file extension.
    if (decode_secret_file_extn(decInfo) != e_success)
    {
        close_decode_files(decInfo);
        return e_failure;
    }

    // Decode the secret file size.
    if (decode_secret_file_size(decInfo) != e_success)
    {
        close_decode_files(decInfo);
        return e_failure;
    }


    // Create the output file name.
    // Example:
    // output + .txt = output.txt
    strcpy(output_file, decInfo->output_fname);
    strcat(output_file, decInfo->secret_file_extn);


    // Open the output file in write-binary mode.
    decInfo->fptr_output = fopen(output_file, "wb");


    //Check whether the output file was created successfully.
    if (decInfo->fptr_output == NULL)
    {
        printf(RED "ERROR: Unable to create output file\n" RESET);

        close_decode_files(decInfo);
        return e_failure;
    }

    printf(GREEN "INFO : SUCCESS : Output file created : %s\n" RESET, output_file);


    // Decode the actual secret file data and write it into the output file.
    if (decode_secret_file_data(decInfo) != e_success)
    {
        close_decode_files(decInfo);
        return e_failure;
    }

    // Close all opened files.
    close_decode_files(decInfo);


    // Display final decoding success message.
    printf(GREEN "INFO : SUCCESS : Decoding completed\n" RESET);

    return e_success;
}