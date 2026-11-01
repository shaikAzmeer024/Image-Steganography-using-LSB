#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"


#define MAX_FILE_SUFFIX 5
#define MAX_MAGIC_STRING 30


/* Structure for decoding */
typedef struct _DecodeInfo
{
    /* Stego image */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Output file */
    char *output_fname;
    FILE *fptr_output;

    /* Secret file extension */
    char secret_file_extn[MAX_FILE_SUFFIX];
    int secret_file_extn_size;

    /* Secret file size */
    long size_secret_file;

    /* Magic string */
    char magic_string[MAX_MAGIC_STRING];
    int magic_string_size;

} DecodeInfo;


/* Function declarations */

/* Read and validate arguments */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Open files */
Status open_decode_files(DecodeInfo *decInfo);

/* Skip BMP header */
Status skip_bmp_header(FILE *fptr_stego_image);

/* Decode one byte */
char decode_byte_from_lsb(char *image_buffer);

/* Decode data from image */
Status decode_data_from_image(char *data, int size, FILE *fptr_stego_image);

/* Decode magic string */
Status decode_magic_string_size(DecodeInfo *decInfo);
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode secret file extension */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Complete decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Close files */
void close_decode_files(DecodeInfo *decInfo);


#endif