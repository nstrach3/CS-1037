//
// Created by Nathan on 2024-11-23.
//

/* huffman.h */
#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 100
#define ASCII_SIZE 128

// Huffman Tree Node Structure
typedef struct HuffmanNode {
    char data;  // Character
    unsigned frequency;  // Frequency of the character
    struct HuffmanNode *left, *right;  // Left and right children
} HuffmanNode;

// Min Heap Node Structure
typedef struct MinHeapNode {
    HuffmanNode* node;
    struct MinHeapNode* next;
} MinHeapNode;

// Function declarations
void initialize(void);
void encode(const char* input_file, const char* output_file);
void decode(const char* input_file, const char* output_file);
void free_memory(void);

#endif // HUFFMAN_H