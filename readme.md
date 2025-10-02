# Image Compression using Singly Linked Lists

## Project Overview

This project presents a robust solution for grayscale image compression built on core Data Structures and Algorithms principles. It uses a Run-Length Encoding (RLE) scheme with singly linked lists to encode only the black segments of an image, delivering significant storage efficiency. The implementation serves as a stable platform for complex image manipulation, fully leveraging data structure operations.

## Key Features & CV Validation

| Feature             | Technical Implementation                                                                                                 | CV Claim Supported                                                                                   |
|---------------------|-------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------|
| Efficient Encoding  | Uses a singly linked list per row to store only the start and end indices of black pixel runs.                          | Engineered a grayscale image compression system using linked list encoding.                          |
| Data Transformation | Constructor algorithm analyzes O(N^2) pixel data, identifies contiguous runs, converts dense data to sparse format.      | Developed algorithms for pixel grouping and storage transformation.                                  |
| Image Manipulation  | Stable AND, XOR, and INVERT methods decompress rows, apply boolean logic, and re-compress the result.                   | Applied boolean operations for image transformation and validation.                                  |
| Memory Management   | Explicit cleanup_row and destructor logic prevent memory leaks, ensuring efficient dynamic memory usage.                 | Ensuring accuracy while minimizing memory overhead.                                                  |

## Core Concepts and Data Structures

- **Run-Length Encoding (RLE):**
  - Stores consecutive identical values as a single value and its count (or range).
- **Array of Singly Linked Lists:**
  - A vector holds the head pointers for each image row.
  - Each row's data is stored in a singly linked list of `[start, end]` index nodes.

## File Format

The system expects input as a space-separated sequence, starting with the dimensions `<width> <height>`, followed by pixel data rows.

**Example Input**

16 16  
1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1  
1 1 1 1 1 0 0 0 1 1 1 1 1 1 1 1  
...(16 rows total)


- `0` represents a black pixel (encoded in the linked list).
- `1` represents a white pixel (not encoded, effectively compressing storage).

