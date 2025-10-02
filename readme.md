# Image Compression using Singly Linked Lists

## Project Overview

This project presents a robust solution for grayscale image compression. Using Run-Length Encoding (RLE) and singly linked lists, it stores only the black pixel runs, delivering significant storage efficiency. The algorithms convert dense O(N^2) pixel data into a sparse, compact format, support safe logical image operations, and robustly manage memory to prevent leaks.

## Key Features

| Feature             | Description                                                                                         |
|---------------------|-----------------------------------------------------------------------------------------------------|
| Efficient Encoding  | Compresses each image row into singly linked lists, storing only start and end indices of black runs.|
| Data Transformation | Converts dense pixel grids into sparse, compact linked list structures.                              |
| Image Manipulation  | Supports AND, XOR, INVERT; decompresses, applies boolean logic, then recompresses the result.        |
| Memory Management   | Explicit cleanup and destructor routines prevent leaks and optimise dynamic memory usage.             |

## Core Concepts and Data Structures

- **Run-Length Encoding (RLE):**
  - Stores consecutive identical values as a single record with its range.
- **Array of Singly Linked Lists:**
  - A vector tracks head pointers for each row; each row’s data consists of `[start, end]` index nodes.

## File Format

The system expects input as a space-separated sequence, beginning with `<width> <height>`, followed by pixel data rows.

**Example Input**

16 16  
1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1  
1 1 1 1 1 0 0 0 1 1 1 1 1 1 1 1  
...(16 rows total)


- `0` represents a black pixel (encoded in the linked list).
- `1` represents a white pixel (not encoded, effectively compressing storage).

