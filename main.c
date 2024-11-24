

/* huffman_impl.c */
#include "huffman.h"

// Global variables
static HuffmanNode* root = NULL;
static char* codes[ASCII_SIZE];
static int freq[ASCII_SIZE];

// Helper function to create a new Huffman Tree Node
HuffmanNode* create_node(char data, unsigned frequency) {
    HuffmanNode* node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    if (node == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(1);
    }
    
    node->data = data;
    node->frequency = frequency;
    node->left = node->right = NULL;
    return node;
}

// Initialize frequency array and codes
void initialize(void) {
    memset(freq, 0, sizeof(freq));
    memset(codes, 0, sizeof(codes));
    root = NULL;
}

// Calculate frequency of characters in input file
static void calculate_frequency(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        exit(1);
    }

    int c;
    while ((c = fgetc(file)) != EOF) {
        if (c < ASCII_SIZE) {
            freq[c]++;
        }
    }
    fclose(file);
}

// Insert node into priority queue
static MinHeapNode* insert_queue(MinHeapNode* head, HuffmanNode* tree_node) {
    MinHeapNode* new_node = (MinHeapNode*)malloc(sizeof(MinHeapNode));
    new_node->node = tree_node;
    new_node->next = NULL;

    if (head == NULL || tree_node->frequency < head->node->frequency) {
        new_node->next = head;
        return new_node;
    }

    MinHeapNode* current = head;
    while (current->next != NULL && 
           current->next->node->frequency < tree_node->frequency) {
        current = current->next;
    }

    new_node->next = current->next;
    current->next = new_node;
    return head;
}

// Build Huffman Tree
static HuffmanNode* build_huffman_tree(void) {
    MinHeapNode* head = NULL;

    // Create initial nodes for characters with non-zero frequency
    for (int i = 0; i < ASCII_SIZE; i++) {
        if (freq[i] != 0) {
            head = insert_queue(head, create_node(i, freq[i]));
        }
    }

    // Build tree by combining nodes
    while (head != NULL && head->next != NULL) {
        HuffmanNode* left = head->node;
        head = head->next;
        HuffmanNode* right = head->node;
        head = head->next;

        HuffmanNode* parent = create_node('*', left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;

        head = insert_queue(head, parent);
    }

    HuffmanNode* tree_root = (head != NULL) ? head->node : NULL;
    
    // Free the queue
    while (head != NULL) {
        MinHeapNode* temp = head;
        head = head->next;
        free(temp);
    }

    return tree_root;
}

// Generate Huffman codes recursively
static void generate_codes(HuffmanNode* root, char* code, int top) {
    static char temp_code[MAX_TREE_HT];

    if (root->left) {
        temp_code[top] = '0';
        generate_codes(root->left, temp_code, top + 1);
    }

    if (root->right) {
        temp_code[top] = '1';
        generate_codes(root->right, temp_code, top + 1);
    }

    if (!root->left && !root->right) {
        temp_code[top] = '\0';
        codes[root->data] = strdup(temp_code);
    }
}

// Write compressed data
static void write_compressed(FILE* input, FILE* output, HuffmanNode* tree) {
    // Write frequency table
    fwrite(freq, sizeof(int), ASCII_SIZE, output);

    // Buffer for bits
    unsigned char buf = 0;
    int bit_count = 0;
    int c;

    while ((c = fgetc(input)) != EOF) {
        char* code = codes[c];
        for (int i = 0; code[i]; i++) {
            buf = (buf << 1) | (code[i] - '0');
            bit_count++;

            if (bit_count == 8) {
                fputc(buf, output);
                buf = 0;
                bit_count = 0;
            }
        }
    }

    // Write remaining bits
    if (bit_count > 0) {
        buf = buf << (8 - bit_count);
        fputc(buf, output);
    }
}

// Encode function
void encode(const char* input_file, const char* output_file) {
    calculate_frequency(input_file);
    
    root = build_huffman_tree();
    if (root == NULL) {
        fprintf(stderr, "Error: Empty input file\n");
        return;
    }

    char temp_code[MAX_TREE_HT];
    generate_codes(root, temp_code, 0);

    FILE* input = fopen(input_file, "r");
    FILE* output = fopen(output_file, "wb");

    if (!input || !output) {
        fprintf(stderr, "Error opening files\n");
        return;
    }

    write_compressed(input, output, root);

    fclose(input);
    fclose(output);
}

// Reconstruct Huffman tree from frequency table
static HuffmanNode* reconstruct_tree(void) {
    root = build_huffman_tree();
    return root;
}

// Decode function
void decode(const char* input_file, const char* output_file) {
    FILE* input = fopen(input_file, "rb");
    FILE* output = fopen(output_file, "w");

    if (!input || !output) {
        fprintf(stderr, "Error opening files\n");
        return;
    }

    // Read frequency table
    fread(freq, sizeof(int), ASCII_SIZE, input);
    
    // Reconstruct the Huffman tree
    root = reconstruct_tree();
    if (root == NULL) {
        fprintf(stderr, "Error: Invalid compressed file\n");
        return;
    }

    // Decode the file
    HuffmanNode* current = root;
    unsigned char buf;
    while (fread(&buf, 1, 1, input) == 1) {
        for (int i = 7; i >= 0; i--) {
            int bit = (buf >> i) & 1;
            current = bit ? current->right : current->left;

            if (!current->left && !current->right) {
                fputc(current->data, output);
                current = root;
            }
        }
    }

    fclose(input);
    fclose(output);
}

// Free memory function
void free_memory(void) {
    // Free Huffman codes
    for (int i = 0; i < ASCII_SIZE; i++) {
        if (codes[i] != NULL) {
            free(codes[i]);
            codes[i] = NULL;
        }
    }
}

    // Helper function to free tree nodes recursively
void free_tree(HuffmanNode* node) {
        if (node == NULL) return;
        free_tree(node->left);
        free_tree(node->right);
        free(node);
    // Free the Huffman tree
        free_tree(root);
        root = NULL;
}


