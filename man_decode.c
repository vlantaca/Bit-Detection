/* *********************************************************************
 * File: man_decode.c
 * Author: Michael Bennett
 * Purpose: Decode Manchester (IEEE) communication where the high side
 *          of a bit is represented by a square wave and low is
 *          relitively unchanging
 * ********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Comment out to remove DEBUG prints
#define DEBUG

#define MAX_BUF             1000000
#define SAMPLES_PER_CHECK   32
#define HIGH_MIN_AVG        300000
#define LOW_STATE           0
#define HIGH_STATE          1
#define UNKNOWN_STATE       -1
#define HALF_PERIOD_TC      215

typedef enum { false, true } bool;

int main(int argc, char **argv) {
    // Algorithm Variables
    int halfPeriodCount = 0;
    bool firstHalfPeriod = false;
    bool startEdge = false;
    int doubleState = UNKNOWN_STATE;
    int curState = LOW_STATE;
    int lastState = UNKNOWN_STATE;
    int secondLastState = UNKNOWN_STATE;
    int decoded_data[MAX_BUF];
    int bit_num = 0;

    int i, j, sample, num_samples;
    int samples[MAX_BUF];
    for (i = 0; i < MAX_BUF; ++i) {
        samples[i] = 0;
        decoded_data[i] = -1;
    }
    
    char *filename = argv[1];
    FILE *file_in = fopen(filename, "r");
    if (file_in == NULL) {
        perror("ERROR main: failed to open the input file.\n");
        exit(1);
    }

    FILE *file_out = fopen("input_abs.txt","w");
    if (file_out == NULL) {
        perror("ERROR main: failed to open the output file.\n");
        exit(1);
    }

    // Read samples from file, take abs, and place into an array
    num_samples = 0;
    for (;;) {
        if (fscanf(file_in, "%d", &sample) <= 0) break;
        samples[num_samples] = abs(sample);
        fprintf(file_out, "%d\n", abs(sample));
        num_samples++;
    }

    fclose(file_in);
    fclose(file_out);

#ifdef DEBUG
    printf("Total number of samples: %d\n", num_samples);
#endif

    // Traverse through all samples applying Manchester Decode
    for (i = 0; i < num_samples; i += SAMPLES_PER_CHECK) {
        //int avgSamplePrev = 0;
        int avgSampleNext = 0;
        //int prevSamples = 0;
        int nextSamples = 0;

        // Find average value for the prev and next set of point around the expected edge
        for (j = 0; j < SAMPLES_PER_CHECK && i+j < num_samples; j++) {
            //prevSamples += samples[i-j];
            nextSamples += samples[i+j];
        }
        //avgSamplePrev = prevSamples / SAMPLES_PER_CHECK
        avgSampleNext = nextSamples / SAMPLES_PER_CHECK;

#ifdef DEBUG
    printf("Avg for samples %d to %d: %d\n",i ,i+j , avgSampleNext);
#endif
        // Associate current bit value based on min/max values and check if it's the start bit
        if ( avgSampleNext >= HIGH_MIN_AVG ) {
            curState = HIGH_STATE;
            // Only enters this statement for the rising edge of the start signal
            if (!startEdge) {

#ifdef DEBUG
    printf("!!!!! Start between samples %d to %d\n",i ,i+j);
#endif

                startEdge = true;
                firstHalfPeriod = true;
                halfPeriodCount = 0;
            }
        } else {
            curState = LOW_STATE;
        }
        
        // When start flag is set check for data
        if (startEdge) {
            // Increment and check if half period is finished
            halfPeriodCount += SAMPLES_PER_CHECK;
            if (halfPeriodCount >= HALF_PERIOD_TC) {

#ifdef DEBUG
    printf("Half Period count: %d\n",halfPeriodCount);
    printf("doubleState: %d curState:%d lastState:%d secondLastState:%d\n", doubleState,curState, lastState, secondLastState);
#endif

                // Reset half period count
                halfPeriodCount = 0;
                
                // Check if this is the first pass after the start edge
                if (firstHalfPeriod) {
                    if (curState == HIGH_STATE) doubleState = HIGH_STATE;
                    //secondLastState = lastState;
                    //lastState = curState;
                    firstHalfPeriod = false;

#ifdef DEBUG
    printf("First Half Period- doubleState:%d curState:%d lastState:%d secondLastState:%d\n", doubleState, curState, lastState, secondLastState);
#endif

                }
                // Check for bit flip
                else if (curState != lastState && doubleState != curState) {

#ifdef DEBUG
    printf("***** %d detected between samples %d to %d\n", curState, i, i+j);
#endif                    

                    decoded_data[bit_num] = curState;
                    bit_num++;
                    secondLastState = lastState;
                    lastState = curState;
                }
                // Check for non bit flip
                else if (curState == lastState) {
                    doubleState = curState;
                    secondLastState = lastState;
                    lastState = curState;

#ifdef DEBUG
    printf("NonFlip- doubleState: %d curState:%d lastState:%d secondLastState:%d\n", doubleState, curState, lastState, secondLastState);
#endif

                }
            }
            
            // Reset input stream last three states are equivalent
            if (curState == lastState && lastState == secondLastState) {
                startEdge = false;

#ifdef DEBUG
    printf("!!!!! End of transmission detected between samples %d to %d\n", i, i+j);
    printf("!!!!! curState:%d lastState:%d secondLastState:%d\n", curState, lastState, secondLastState);
#endif

            }
        }
    }

    // Convert resulting "bits" to bytes. data is Little Endian
    int byte_val = 0;
    int power = 7;
    for (i = bit_num; i >= 0; i--) {
        byte_val += pow(2,power) * decoded_data[bit_num];
        power--;
        if (power < 0) {
            printf("0x%x\n",byte_val);
            power = 7;
            byte_val = 0;
        }
    }


    return 0;
}
