# Image Steganography using C

## Technologies Used

- C Programming

- File Handling

- Bitwise Operations

- BMP Image Processing

- Structures and Pointers

- Command Line Arguments


## Description

Image Steganography is a technique used to hide secret information inside an image without visibly affecting the image quality. This project implements Image 
Steganography using the Least Significant Bit (LSB) technique in C programming.

The application securely hides secret files such as `.txt`, `.c`, `.cpp`, and `.sh` files inside BMP images and later extracts the hidden data successfully. The project uses bitwise operations, file handling, structures, and command-line arguments to perform encoding and decoding operations efficiently.

The main objective of this project is to provide secure data hiding inside digital images while preserving the original appearance of the image.

## Applications

- Secure Communication

- Digital Watermarking

- Data Protection

- Hidden Information Transfer

- Copyright Protection

- Military Communication Systems

## Features

- Encode secret files into BMP images

- Decode hidden files from stego images

- Uses Least Significant Bit (LSB) technique

- Supports multiple secret file formats

- Automatic recovery of secret file extension

- Preserves original image quality

- Capacity checking before encoding

- Efficient encoding and decoding process

- Command-line based execution

## Project Structure

.
├── main.c
├── encode.c
├── encode.h
├── decode.c
├── decode.h
├── common.h
├── beautiful.bmp
├── secret.txt
└── README.md

## Working Principle
### Encoding Process

->Read the source BMP image

->Read the secret file

->Convert secret data into binary bits

->Store secret bits inside image LSBs

->Generate the output stego image

### Decoding Process

->Read the stego image

->Extract hidden bits from image bytes

->Reconstruct secret data

->Restore the original secret file

## LSB Technique

The Least Significant Bit (LSB) technique stores secret data inside the least significant bits of image bytes. Since only the last bit is modified, the image appearance remains almost unchanged to the human eye.

Example
Original Byte : 10110110

Add bit = 1 to lsb

Modified Byte : 10110111

Only the last bit changes.

## Compilation

gcc *.c

## Encoding Command

./a.out -e beautiful.bmp secret.txt output.bmp

Parameters :

-e → Encode operation

beautiful.bmp → Source BMP image

secret.txt → Secret file to hide

output.bmp → Output stego image

## Decoding Command  

./a.out -d output.bmp decoded_file

Parameters :

-d → Decode operation

output.bmp → Stego image

decoded_file → Output secret filename

## Conclusion

This project successfully demonstrates Image Steganography using the Least Significant Bit (LSB) technique in C programming. The application securely hides and retrieves secret files inside BMP images without noticeably affecting image quality. The project helped in understanding concepts such as bitwise operations, file handling, structures, and BMP image processing while implementing secure data hiding techniques.

- SHAIK AZMEER
