#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"

// colour coding 
/* ANSI color codes for terminal output */
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"

/*----------------------------------Function Definitions-------------------------------*/

/* check the operation types */
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // argv[2] -> .BMP
    //     yes -> store in the structure (encInfo->src_image_fname = argv[2])
    //     No  -> return e_failure;
    if(strstr(argv[2], ".bmp") != NULL)
    {
        printf(".bmp is present\n");
        encInfo->src_image_fname = argv[2];

    }
    else
    {
        printf("The source file is not .bmp file");
        return e_failure;
    }
    
    // argv[3] -> .txt
    //     yes -> store it in the structure
    //            store the extn in the structure.
    //     No -> return e_failure;
    if(strstr(argv[3], ".txt") != NULL)
    {
        printf(".txt is present\n");
        encInfo->secret_fname = argv[3];

        char *extn = strrchr(argv[3], '.');
        if (extn != NULL)
        {
            strcpy(encInfo->secret_file_extn, extn);
        }

    }
    else
    {
        printf("The secret file is not .txt file");
        return e_failure;
    }

    // argv[4] is NULL
    //     NULL -> store "stego.bmp" in the structure
    //     Not a NULL -> >.BMP
    //         yes -> strore it in structure
    //         No  -> return e_failure;
    if((argv[4] == NULL))
    {
        encInfo->stego_image_fname = "stego.bmp";
    }
    else
    {
        if(strstr(argv[4], ".bmp") != NULL)
        {
            encInfo->stego_image_fname = argv[4];
        }
        else
        {
            printf("The source file is not .bmp file");
            return e_failure;
        }
    }

    //printf("INFO : SUCCESS : Read and validates successfull\n");
    return e_success;

}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // open source Image file encInfo pointer to a structure.
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");

    // Do Error handling
    // If the file cannot be opened, fopen() returns NULL.
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");

        // standard error
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // open Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");

    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        fclose(encInfo->fptr_src_image);
    	return e_failure;
    }

    // open Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");

    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
    	return e_failure;
    }

    // All files opened successfully 
    //printf("INFO : SUCCESS : Files Opened Successfull\n");
    return e_success;
}

// get BMP capacity.
Status check_capacity(EncodeInfo *encInfo) 
{

    int secret_file_size;
    int size_of_info;
    char magic_string[20];

    printf("Enter the Magic String :");
    scanf("%[^\n]",encInfo->magic_string);

    // Move secret file pointer to the end 
    fseek(encInfo->fptr_secret, 0, SEEK_END);

    // Get secret file size 
    encInfo->size_secret_file = ftell(encInfo->fptr_secret);

    // Move file pointer back to beginning 
    rewind(encInfo->fptr_secret);

    /*
     * Calculate total information to be encoded
     *
     * MAGIC_STRING
     * + size of secret extension
     * + secret extension
     * + size of secret file
     * + secret file data
     */
    size_of_info = strlen(encInfo->magic_string) + strlen(encInfo->secret_file_extn) + sizeof(long) + encInfo->size_secret_file;

    // Get BMP image capacity
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    // One byte of data requires 8 bytes of image capacity 
    if (size_of_info * 8 <= encInfo->image_capacity)
    {
        return e_success;
    }

    return e_failure;

}


uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;

    // In a BMP file, the width information is stored at byte position 18.
    // Move the file pointer to position 18.
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width of the image.
    fread(&width, sizeof(uint), 1, fptr_image);

    // Read the height of the image.
    // Height is stored immediately after width.
    fread(&height, sizeof(uint), 1, fptr_image);

    // Move the file pointer back to the beginning of the image.
    rewind(fptr_image);

    // Each pixel contains 3 bytes:
    // 1 byte  -> Red
    // 1 byte  -> Green
    // 1 byte  -> Blue
    // Image size = width * height * 3
    return width * height * 3;
}


// Get the total size of a file.
uint get_file_size(FILE *fptr)
{
    uint size;

    // Move the file pointer to the end of the file.
    fseek(fptr, 0, SEEK_END);

    // ftell() returns the current position of the file pointer.
    //Since the pointer is at the end, the position represents the file size.
    size = ftell(fptr);

    // Move the file pointer back to the beginning of the file.
    rewind(fptr);

    //Return the size of the file.
    return size;
}



// Copy the BMP header from the source image to the stego image.
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    // BMP header size is 54 bytes.
    // This buffer temporarily stores the BMP header.
    char str[54];


    // Read 54 bytes from the source image.
    // If fread() does not read successfully, return failure.
    if(fread(str, 54, 1, fptr_src_image) != 1)
    {
        return e_failure;
    }

    // Write the 54-byte BMP header into the stego image.
    // If writing fails, return failure.
    if(fwrite(str, 54, 1, fptr_stego_image) != 1)
    {
        return e_failure;
    }

    // BMP header copied successfully.
    return e_success;
}


// Encode the magic string into the image.
// The magic string is used during decoding to check whether the image contains encoded data from our program.
Status encode_magic_string(EncodeInfo *encInfo)
{
    // Find the length of the magic string.
    int size = strlen(encInfo->magic_string);

    // Encode the magic string into the source image data.
    // Each character requires 8 image bytes.
    if (encode_data_to_image(encInfo->magic_string, size, encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
    {
        return e_failure;
    }

    // Magic string encoded successfully.
    return e_success;
}


// Encode the size of the magic string.
// The size is stored before the actual magic string so that the decoder knows how many characters to read.
Status encode_magic_string_size(int size, EncodeInfo *encInfo)
{
    // Convert the address of size into a character pointer and encode its  bytes into the image
    encode_data_to_image((char *)&size, sizeof(int), encInfo->fptr_src_image, encInfo->fptr_stego_image);


    // Magic string size encoded successfully.
    return e_success;
}


// Encode data into the source image.
// One character contains 8 bits.
// Therefore, 8 image bytes are required to store one character.
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    // This buffer stores 8 image bytes.
    // These 8 bytes are used to store  one byte of secret data.
    char image_buffer[8];


    // process each byte of the input data.
    for (int i = 0; i < size; i++)
    {
        // Read 8 bytes from the source image.
        // These 8 bytes will be used to store one byte of secret data.
        fread(image_buffer, 8, 1, fptr_src_image);


        // Encode one byte of secret data into the 8 image bytes.
        if (encode_byte_to_lsb(data[i], image_buffer) != e_success)
        {
            return e_failure;
        }

        // Write the modified 8 bytes into the stego image.
        fwrite(image_buffer, 8, 1, fptr_stego_image);
    }

    // All data encoded successfully.
    return e_success;
}


// Encode the size of the secret file extension.
// Example:
// Extension = ".txt"
// Size      = 4
// The size is stored before the extension.
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    // Encode the extension size into the stego image.
    if (encode_data_to_image((char *)&size, sizeof(int), encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
    {
        printf("ERROR: Unable to encode extension size\n");

        return e_failure;
    }

    // Extension size encoded successfully.
    return e_success;
}


// Encode the secret file extension.
// Example:
// secret.txt
// Extension = ".txt"
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    // Find the size of the file extension.
    int size = strlen(file_extn);

    // Encode the extension characters into the image.
    if (encode_data_to_image((char *)file_extn, size, encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
    {
        return e_failure;
    }

    // Secret file extension encoded successfully.
    return e_success;
}


// Encode the size of the secret file.
// Example:
// If secret.txt contains 100 bytes, then 100 is stored inside the image.

// During decoding, this size tells the program how many bytes it should read.
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    // Encode the secret file size  into the stego image.
    if (encode_data_to_image((char *)&file_size, sizeof(long), encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
    {
        return e_failure;
    }

    // Secret file size encoded successfully.
    return e_success;
}


// Encode the actual secret file data  into the image.
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    // This variable stores one byte read from the secret file.
    char data;

    // Read the secret file one byte at a time.
    // Continue until the complete secret  file has been processed.
    for (long i = 0; i < encInfo->size_secret_file; i++)
    {
        // Read one byte from the secret file.
        fread(&data, 1, 1, encInfo->fptr_secret);

        // Encode that one byte into 8 bytes of the image.
        if (encode_data_to_image(&data, 1, encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
        {
            return e_failure;
        }
    }

    // Secret file data encoded successfully.
    return e_success;
}


// Encode one byte of secret data into the LSBs of 8 image bytes.
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    // A character contains 8 bits.
    // Therefore, process 8 image bytes to store one character.
    for (int i = 0; i < 8; i++)
    {
        // Clear the LSB of the image byte.
        // This makes the last bit 0.
        image_buffer[i] = image_buffer[i] & (~1);

        // Get one bit from the secret data.
        // Right shift moves the required bit  to the last position.
        // & 1 extracts that bit.
        int bit = (data >> i) & 1;

        // Store the secret bit in the LSB of the image byte.
        image_buffer[i] = image_buffer[i] | bit;
    }

    // One byte successfully encoded.
    return e_success;
}


// Copy all remaining image data from the source image to the stego image.
// This is done after all secret data has been encoded.
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    // Temporary variable used to store one byte of image data.
    char ch;

    // Read the remaining image data one byte at a time.
    // Continue until the end of the source image is reached.
    while (fread(&ch, 1, 1, fptr_src) == 1)
    {
        // Write the original image byte into the stego image.
        fwrite(&ch, 1, 1, fptr_dest);
    }

    // Remaining image data copied successfully.
    return e_success;
}


/*---------------------------------------------------------
 * Function: do_encoding
 *
 * Description:
 * This function performs the complete encoding process.
 *
 * Encoding order:
 *
 * 1. Open all required files
 * 2. Check image capacity
 * 3. Copy BMP header
 * 4. Encode magic string size
 * 5. Encode magic string
 * 6. Encode secret file extension size
 * 7. Encode secret file extension
 * 8. Encode secret file size
 * 9. Encode secret file data
 * 10. Copy remaining image data
 *---------------------------------------------------------*/

Status do_encoding(EncodeInfo *encInfo)
{
    printf(CYAN "\n=================== ENCODING ====================\n" RESET);


    // Open source image, secret file and stego image.
    if (open_files(encInfo) != e_success)
    {
        printf(RED "ERROR: Unable to open files\n" RESET);

        return e_failure;
    }

    printf(GREEN "INFO : SUCCESS : Files Opened Successfully\n" RESET);


    // Check whether the source BMP image has enough capacity to store
    // the secret file.
    if (check_capacity(encInfo) != e_success)
    {
        printf(RED "ERROR: Image capacity is not enough\n" RESET);

        return e_failure;
    }

    printf(GREEN "INFO : SUCCESS : Image Capacity Checked\n" RESET);


    // Copy the 54-byte BMP header from source image to stego image.
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
    {
        printf(RED "ERROR: BMP Header copy failed\n" RESET);

        return e_failure;
    }

    printf(GREEN "INFO : SUCCESS : BMP Header Copied\n" RESET);


    // Find the size of the magic string.
    int size;

    size = strlen(encInfo->magic_string);


    // Encode the size of the magic string into the image.
    if (encode_magic_string_size(size, encInfo) != e_success)
    {
        printf(RED "ERROR: Magic String Size encoding failed\n" RESET);

        return e_failure;
    }

    printf(GREEN "INFO : SUCCESS : Magic String Size Encoded\n" RESET);


    // Encode the actual magic string into the image.
    if (encode_magic_string(encInfo) != e_success)
    {
        printf(RED "ERROR: Magic String encoding failed\n" RESET);

        return e_failure;
    }

    printf(GREEN "INFO : SUCCESS : Magic String Copied\n" RESET);


    // Find the size of the secret file extension.
    int extn_size;

    extn_size = strlen(encInfo->secret_file_extn);


    // Encode the size of the secret file extension.
    if (encode_secret_file_extn_size(extn_size, encInfo) != e_success)
    {
        printf(RED "ERROR: Extension Size encoding failed\n" RESET);

        return e_failure;
    }

    printf(GREEN "INFO : SUCCESS : Extension Size Encoded\n" RESET);


    // Encode the actual secret file extension .txt
    if (encode_secret_file_extn(encInfo->secret_file_extn, encInfo) != e_success)
    {
        printf(RED "ERROR: Secret File Extension encoding failed\n" RESET);

        return e_failure;
    }

    printf(GREEN "INFO : SUCCESS : Secret File Extension Copied\n" RESET);


    // Encode the size of the secret file.
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) != e_success)
    {
        printf(RED "ERROR: Secret File Size encoding failed\n" RESET);

        return e_failure;
    }

    printf(GREEN "INFO : SUCCESS : Secret File Size Encoded\n" RESET);


    // Encode the actual secret file data into the BMP image
    if (encode_secret_file_data(encInfo) != e_success)
    {
        printf(RED "ERROR: Secret File Data encoding failed\n" RESET);

        return e_failure;
    }

    printf(GREEN "INFO : SUCCESS : Secret File Data Copied\n" RESET);


    // Copy all remaining image data from source image to stego image.
    // Remaining data is copied without modification.
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
    {
        printf(RED "ERROR: Remaining Image Data copy failed\n" RESET);

        return e_failure;
    }

    printf(GREEN "INFO : SUCCESS : Remaining Image Data Copied\n" RESET);


    // Encoding completed successfully.
    printf(CYAN "\n========== ENCODING COMPLETED ==========\n" RESET);

    return e_success;
}