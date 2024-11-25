#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For sleep function
#include <stdbool.h>
#include <time.h>

#define MAX_FRAMES 10 // Total number of frames
#define WINDOW_SIZE 4 // Sliding window size

// Simulates frame transmission with potential loss
bool transmitFrame(int frameNum) {
    printf("Sender: Sending frame %d\n", frameNum);
    // Random chance of successful transmission (80% success rate)
    return rand() % 10 < 8;
}

// Simulates acknowledgment with potential loss
bool transmitACK(int frameNum) {
    printf("Receiver: Sending ACK for frame %d\n", frameNum);
    // Random chance of successful acknowledgment (80% success rate)
    return rand() % 10 < 8;
}

// Selective Repeat Protocol
void selectiveRepeatProtocol() {
    int frames[MAX_FRAMES];
    for (int i = 0; i < MAX_FRAMES; i++) {
        frames[i] = i; // Initialize frames with sequence numbers
    }

    bool ackReceived[MAX_FRAMES] = {0}; // Track ACKs for frames
    int base = 0;                       // First frame in the window
    int nextSeqNum = 0;                 // Next frame to be sent
    int totalFrames = MAX_FRAMES;

    while (base < totalFrames) {
        // Send frames within the window
        for (int i = nextSeqNum; i < base + WINDOW_SIZE && i < totalFrames; i++) {
            if (!ackReceived[i] && transmitFrame(frames[i])) {
                printf("Sender: Frame %d sent successfully.\n", frames[i]);
            } else if (!ackReceived[i]) {
                printf("Sender: Frame %d lost during transmission.\n", frames[i]);
            }
            nextSeqNum++;
        }

        // Wait for acknowledgments
        sleep(2); // Simulate waiting time

        for (int i = base; i < base + WINDOW_SIZE && i < totalFrames; i++) {
            if (!ackReceived[i]) {
                if (transmitACK(frames[i])) {
                    printf("Sender: ACK for frame %d received.\n", frames[i]);
                    ackReceived[i] = true;
                } else {
                    printf("Sender: ACK for frame %d lost.\n", frames[i]);
                }
            }
        }

        // Slide the window
        while (ackReceived[base] && base < totalFrames) {
            printf("Receiver: Frame %d delivered to application layer.\n", frames[base]);
            base++;
        }

        // Retransmit only the unacknowledged frames in the window
        nextSeqNum = base;
    }

    printf("All frames sent, acknowledged, and delivered successfully.\n");
}

int main() {
    srand(time(0)); // Initialize random seed

    printf("Simulating Selective Repeat Protocol...\n");
    selectiveRepeatProtocol();

    return 0;
}
