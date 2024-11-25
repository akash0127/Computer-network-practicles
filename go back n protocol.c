#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

// Go-Back-N Protocol
void goBackNProtocol() {
    int frames[MAX_FRAMES];
    for (int i = 0; i < MAX_FRAMES; i++) {
        frames[i] = i; // Initialize frames with sequence numbers
    }

    int base = 0;     // First frame in the window
    int nextSeqNum = 0; // Next frame to be sent
    int totalFrames = MAX_FRAMES;

    while (base < totalFrames) {
        // Send frames within the window
        for (int i = nextSeqNum; i < base + WINDOW_SIZE && i < totalFrames; i++) {
            if (transmitFrame(frames[i])) {
                printf("Sender: Frame %d sent successfully.\n", frames[i]);
            } else {
                printf("Sender: Frame %d lost during transmission.\n", frames[i]);
            }
            nextSeqNum++;
        }

        // Wait for acknowledgments
        sleep(2); // Simulate waiting time
        bool ackReceived = false;

        for (int i = base; i < nextSeqNum; i++) {
            if (transmitACK(frames[i])) {
                printf("Sender: ACK for frame %d received.\n", frames[i]);
                base++; // Slide the window forward
                ackReceived = true;
            } else {
                printf("Sender: ACK for frame %d lost.\n", frames[i]);
                break; // If an ACK is lost, go back to the first unacknowledged frame
            }
        }

        // If no ACK was received, retransmit from the base
        if (!ackReceived) {
            printf("Sender: Retransmitting from frame %d.\n", base);
            nextSeqNum = base; // Reset the next frame to be sent
        }
    }

    printf("All frames sent and acknowledged successfully.\n");
}

int main() {
    srand(time(0)); // Initialize random seed

    printf("Simulating Go-Back-N Protocol...\n");
    goBackNProtocol();

    return 0;
}
