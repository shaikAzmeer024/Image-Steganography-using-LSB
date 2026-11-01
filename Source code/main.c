
/*
    Name         : Shaik Azmeer
    Regd_no      : 26015_079
    Project name : Steaganography
    Title        : Image Steganography using LSB Technique in BMP Images
*/

#include <stdio.h>
#include <string.h>

#include "encode.h"
#include "decode.h"
#include "types.h"


/* ===================== Terminal Colors ===================== */

#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"


int main(int argc, char *argv[])
{
    OperationType operation;

    // Check whether the user has entered
    // at least one command-line argument.

    if (argc < 2)
    {
        printf(RED "ERROR: Invalid arguments\n\n" RESET);

        printf(YELLOW "Usage:\n" RESET);
        printf("./a.out --help\n");

        return e_failure;
    }


    // Check whether the user entered --help.
    // If --help is given, display all the
    // available commands and options.

    if (strcmp(argv[1], "--help") == 0)
    {
        printf("\n");
        printf(CYAN "========== LSB Steganography ==========\n" RESET);
        printf("\n");

        printf(BLUE "Encoding:\n" RESET);
        printf("./a.out -e source.bmp secret.txt stego.bmp\n");
        printf("./a.out -e source.bmp secret.txt\n");

        printf("\n");

        printf(BLUE "Decoding:\n" RESET);
        printf("./a.out -d stego.bmp output\n");
        printf("./a.out -d stego.bmp\n");

        printf("\n");

        printf(BLUE "Options:\n" RESET);
        printf("-e       Encode secret file into BMP image\n");
        printf("-d       Decode secret file from BMP image\n");
        printf("--help   Display help\n");

        printf("\n");
        printf(CYAN "========================================\n" RESET);

        return e_success;
    }

    // Check the operation entered by the user.
    // -e  -> Encoding
    // -d  -> Decoding
    // Other -> Unsupported operation

    operation = check_operation_type(argv);

    // ================================================================================================
    //                                         ENCODING
    // ================================================================================================
    // User command:
    // ./a.out -e source.bmp secret.txt stego.bmp
    // ./a.out -e source.bmp secret.txt

    if (operation == e_encode)
    {
        printf("\n");
        printf(BLUE "You chose encoding\n\n" RESET);


        // Check the number of arguments required
        // for encoding.
        // argc = 4:
        // ./a.out -e source.bmp secret.txt
        // argc = 5:
        // ./a.out -e source.bmp secret.txt stego.bmp

        if (argc < 4 || argc > 5)
        {
            printf(RED "ERROR: Invalid encoding arguments\n\n" RESET);

            printf(YELLOW "Usage:\n" RESET);
            printf("./a.out -e source.bmp secret.txt stego.bmp\n");

            return e_failure;
        }

        //Create an EncodeInfo structure.
        // This structure stores all the information
        // required during the encoding process.
        //  {0} initializes all structure members to 0.

        EncodeInfo encInfo = {0};

        // Read and validate the encoding arguments.
        // This function checks the source image,
        // secret file and output image names.

        if (read_and_validate_encode_args(argv, &encInfo) != e_success)
        {
            return e_failure;
        }

        // Start the encoding process.
        // The secret file is hidden inside
        // the source BMP image.

        if (do_encoding(&encInfo) != e_success)
        {
            printf(RED "Encoding unsuccessful\n" RESET);

            return e_failure;
        }

        // Display the final success message after encoding is completed successfully.
        printf("\n");
        printf(GREEN "====================================\n" RESET);
        printf(GREEN "       Encoding successful\n" RESET);
        printf(GREEN "====================================\n" RESET);
    }

    // ===========================================================================================================
    //                                            DECODING
    // ===========================================================================================================
    // User command:
    // ./a.out -d stego.bmp output
    // or
    // ./a.out -d stego.bmp

    else if (operation == e_decode)
    {
        printf("\n");
        printf(BLUE "You chose decoding\n\n" RESET);

        // Check the number of arguments required for decoding.
        // argc = 3:
        // ./a.out -d stego.bmp
        // argc = 4:
        // ./a.out -d stego.bmp output

        if (argc < 3 || argc > 4)
        {
            printf(RED "ERROR: Invalid decoding arguments\n\n" RESET);

            printf(YELLOW "Usage:\n" RESET);
            printf("./a.out -d stego.bmp output\n");

            return e_failure;
        }

        // Create a DecodeInfo structure.
        // This structure stores all the information required during the decoding process.
        // {0} initializes all structure members to 0.

        DecodeInfo decInfo = {0};

        //Read and validate the decoding arguments.
        // This function checks the stego image and output file name.

        if (read_and_validate_decode_args(argv, &decInfo) != e_success)
        {
            printf(RED "Read and validate unsuccessful\n" RESET);

            return e_failure;
        }

        // Start the decoding process.
        // The hidden data is extracted from the stego BMP image.

        if (do_decoding(&decInfo) != e_success)
        {
            printf(RED "Decoding unsuccessful\n" RESET);

            return e_failure;
        }

        // Display the final success message after decoding is completed successfully.
        printf("\n");
        printf(GREEN "====================================\n" RESET);
        printf(GREEN "       Decoding successful\n" RESET);
        printf(GREEN "====================================\n" RESET);
    }

    // ==========================================================================================
    //                             UNSUPPORTED OPERATION
    // ==========================================================================================
    // This block executes when the user enters something other than -e or -d.
    // enters something other than -e or -d.
    // ./a.out -x

    else
    {
        printf(RED "ERROR: Unsupported operation '%s'\n\n" RESET, argv[1]);

        printf(YELLOW "Use:\n" RESET);
        printf("./a.out --help\n");

        return e_failure;
    }

    // Program completed successfully.
    return e_success;
}


/*
Project Description:
This project hides a secret file inside a .bmp image using the Least Significant Bit (LSB) method and later retrieves the hidden file back without visibly changing the image.

This project supports both:
Encoding → Hiding secret data inside image
Decoding → Extracting hidden data from image

The project is implemented completely in C programming language.

1. What is Steganography?
Steganography is a technique used to hide secret information inside another file such as:
Image, Audio, Video, Text ,without making the hidden information noticeable.

In this project:

Carrier File → BMP image
Secret File → .txt, .c, .cpp, .sh
Output → Stego image containing hidden data

2. Main Objective of the Project

The main objective is:
To securely hide a secret file inside an image
To retrieve the hidden file later without data loss
To make the image appear visually unchanged

3. Technique Used – LSB (Least Significant Bit)

This project uses:
LSB Encoding Technique
Every pixel byte in a BMP image contains 8 bits.

Example:

Original byte:
10110110
The last bit is called:
Least Significant Bit (LSB)
Your program modifies only this last bit.
Example:
10110110
Change LSB to 1:
10110111
Only 1 bit changes.
This tiny change is invisible to the human eye.

4. Why BMP Image?

BMP images are used because:
BMP is uncompressed
Pixel bytes are stored directly
Easy to modify bits
No quality loss after editing
This code checks .bmp format during validation.

5. Files Used in the Project
1. main.c
Controls the entire program.
Responsibilities:
Reads command-line arguments
Detects encode/decode operation
Calls validation functions
Calls encoding or decoding functions

Functions:
check_operation()
main()

2. common.h

Contains common definitions.
Includes:
Magic string
Status enums
Operation enums
Magic string used:
#define MAGIC_STRING "#*"

3. encode.h

Contains:
Encode structure
Function declarations
Stores:
Source image info
Secret file info
Output image info

4. encode.c

Performs complete encoding operation.
Responsibilities:
Open files
Validate files
Check image capacity
Copy BMP header
Encode data into image
Create stego image

5. decode.h

Contains:
Decode structure
Decoding function declarations

6. decode.c

Performs complete decoding process.
Responsibilities:
Open stego image
Verify magic string
Decode extension
Decode secret size
Decode hidden data
Recreate original file

6. Project Workflow
Encoding Process
Secret File + BMP Image
        ↓
Read Secret Data
        ↓
Convert Secret Data into Bits
        ↓
Modify LSB of BMP Pixels
        ↓
Create Stego Image
Decoding Process
Stego Image
      ↓
Read LSB Bits
      ↓
Reconstruct Binary Data
      ↓
Recover Original Secret File


7. Encoding Process – Step by Step
Step 1: Validate Arguments

Program checks:
Source image is .bmp
Secret file exists
Output file exists
Supported secret files:
.txt
.c
.cpp
.sh
Step 2: Open Files
The program opens:
Source BMP image
Secret file
Output stego image
Using:
fopen()

Step 3: Check Image Capacity
Program calculates:
Image Capacity > Secret Data Size
BMP capacity formula:
width × height × 3
because BMP stores:
Red
Green
Blue
bytes for each pixel.
Step 4: Copy BMP Header
BMP header size
54 bytes
The header is copied directly because it contains:
Image format
Width
Height
Color information
Step 5: Encode Magic String
Magic string:
#*
Purpose:

Identifies whether image contains hidden data
Each character is stored using:
8 image bytes
Step 6: Encode File Extension
Example:
.txt
Program stores:
Extension size
Extension characters
This helps decoder recreate original file type.

Step 7: Encode Secret File Size

File size is stored as:
4-byte integer
using:
32 image bytes

Step 8: Encode Secret File Data
Each secret character:
1 byte
is hidden into:
8 image bytes

using LSB substitution.

Example:
Character:
A = 01000001
Each bit goes into one image byte LSB.
Step 9: Copy Remaining Image Data
After encoding completes:
Remaining bytes are copied unchanged.

8. Decoding Process – Step by Step
Step 1: Open Stego Image
Decoder opens encoded BMP image.
Step 2: Verify Magic String
Decoder extracts hidden magic string.
If it matches:
#*
then image contains secret data.
Step 3: Decode File Extension
Decoder extracts:
Extension size
Extension characters
Then recreates filename.

Step 4: Decode Secret File Size
Extracts original hidden file size.

Step 5: Decode Secret Data
Reads:
8 image bytes → 1 secret byte
Reconstructs original file completely.

9. Important Concepts Used
File Handling
Functions used:

fopen()
fread()
fwrite()
fseek()
ftell()
fclose()
Bitwise Operations
Used heavily in LSB encoding.
Operators:
&
|
<<
>>
~

Encoding
./a.out -e beautiful.bmp secret.txt output.bmp
Decoding
./a.out -d output.bmp decoded
10. Advantages of the Project
Data hidden securely
Image quality almost unchanged
Simple implementation
Efficient memory usage
Supports multiple file types
Fast encoding/decoding

16. Conclusion
This project successfully demonstrates how secret information can be hidden inside a BMP image using LSB steganography.
The implementation uses file handling, structures, bitwise operations, and command-line processing in C language to securely encode and decode secret files while preserving image appearance.
The project is simple, efficient, and useful for understanding real-world data hiding techniques and secure communication systems.
*/