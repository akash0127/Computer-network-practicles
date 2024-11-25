#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For sleep function
#include <stdbool.h>
#include <time.h>

// Simulates frame transmission with potential loss
bool transmitFrame(int seqNum) {
    printf("Sender: Sending frame with sequence number: %d\n", seqNum);
    // Random chance of successful transmission (80% success rate)
    return rand() % 10 < 8; 
}

// Simulates acknowledgment with potential loss
bool transmitACK(int seqNum) {
    printf("Receiver: Sending ACK for sequence number: %d\n", seqNum);
    // Random chance of successful acknowledgment (80% success rate)
    return rand() % 10 < 8;
}

// Simulates the stop-and-wait protocol
void stopAndWaitProtocol() {
    int frame = 0; // Sequence number: 0 or 1
    int expectedFrame = 0;
    int maxRetries = 3;
    bool receivedACK = false;

    while (1) {
        int retries = 0;
        receivedACK = false;

        // Transmit frame until ACK is received or retries exceed max
        while (retries < maxRetries) {
            if (transmitFrame(frame)) {
                printf("Sender: Frame %d sent successfully.\n", frame);

                // Simulate waiting for ACK
                sleep(1);

                if (transmitACK(frame)) {
                    printf("Sender: ACK %d received.\n", frame);
                    receivedACK = true;
                    break;
                } else {
                    printf("Sender: ACK %d lost. Retrying...\n", frame);
                }
            } else {
                printf("Sender: Frame %d lost. Retrying...\n", frame);
            }
            retries++;
        }

        if (!receivedACK) {
            printf("Sender: Maximum retries reached. Terminating...\n");
            break;
        }

        // Receiver processes frame
        if (frame == expectedFrame) {
            printf("Receiver: Frame %d received and processed.\n", frame);
            expectedFrame = 1 - expectedFrame; // Toggle expected frame number
        } else {
            printf("Receiver: Duplicate frame %d received. Ignoring.\n", frame);
        }

        // Sender toggles sequence number
        frame = 1 - frame;

        // Simulate user exit condition
        printf("Continue transmission? (1: Yes, 0: No): ");
        int choice;
        scanf("%d", &choice);
        if (choice == 0) {
            printf("Transmission ended.\n");
            break;
        }
    }
}

int main() {
    srand(time(0)); // Initialize random seed

    printf("Simulating Stop-and-Wait Protocol with 1-bit Sliding Window...\n");
    stopAndWaitProtocol();

    return 0;
}
