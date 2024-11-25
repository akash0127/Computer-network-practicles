#include <stdio.h>
#include <math.h>

#define DATA_BITS 4  // Number of original data bits

// Function to calculate the required number of parity bits
int calculateParityBits(int dataBits) {
    int r = 0;
    while ((int)pow(2, r) < (dataBits + r + 1)) {
        r++;
    }
    return r;
}

// Function to generate Hamming Code
void generateHammingCode(int data[], int dataBits, int hammingCode[], int totalBits) {
    int r = calculateParityBits(dataBits);
    int j = 0, k = 0;

    // Insert data bits and parity bit placeholders
    for (int i = 1; i <= totalBits; i++) {
        if ((i & (i - 1)) == 0) { // If i is a power of 2
            hammingCode[i - 1] = 0; // Placeholder for parity bits
        } else {
            hammingCode[i - 1] = data[j++];
        }
    }

    // Calculate parity bits
    for (int i = 0; i < r; i++) {
        int parityPos = (int)pow(2, i) - 1;
        int parity = 0;
        for (int j = parityPos; j < totalBits; j++) {
            if ((j + 1) & (parityPos + 1)) {
                parity ^= hammingCode[j];
            }
        }
        hammingCode[parityPos] = parity;
    }
}

// Function to detect and correct errors in Hamming Code
void detectAndCorrectHammingCode(int hammingCode[], int totalBits) {
    int errorPos = 0;

    // Calculate parity bits to find the error position
    for (int i = 0; i < log2(totalBits) + 1; i++) {
        int parityPos = (int)pow(2, i) - 1;
        int parity = 0;
        for (int j = parityPos; j < totalBits; j++) {
            if ((j + 1) & (parityPos + 1)) {
                parity ^= hammingCode[j];
            }
        }
        if (parity) {
            errorPos += (parityPos + 1);
        }
    }

    if (errorPos) {
        printf("Error detected at position: %d\n", errorPos);
        hammingCode[errorPos - 1] ^= 1; // Correct the error
    } else {
        printf("No error detected.\n");
    }
}

int main() {
    int data[DATA_BITS] = {1, 0, 1, 1}; // Example data
    int parityBits = calculateParityBits(DATA_BITS);
    int totalBits = DATA_BITS + parityBits;
    int hammingCode[totalBits];

    // Generate Hamming Code
    generateHammingCode(data, DATA_BITS, hammingCode, totalBits);

    printf("Generated Hamming Code: ");
    for (int i = 0; i < totalBits; i++) {
        printf("%d ", hammingCode[i]);
    }
    printf("\n");

    // Simulate an error
    hammingCode[3] ^= 1; // Flip the 4th bit
    printf("Hamming Code with error: ");
    for (int i = 0; i < totalBits; i++) {
        printf("%d ", hammingCode[i]);
    }
    printf("\n");

    // Detect and Correct Error
    detectAndCorrectHammingCode(hammingCode, totalBits);

    printf("Corrected Hamming Code: ");
    for (int i = 0; i < totalBits; i++) {
        printf("%d ", hammingCode[i]);
    }
    printf("\n");

    return 0;
}
