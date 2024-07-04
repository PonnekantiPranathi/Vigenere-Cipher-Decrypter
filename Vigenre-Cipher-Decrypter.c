// Ponnekanti Pranathi - 2103125
// Assignment 2 - Attacking the Vigenere's Cipher


// Including the necessary libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define ALPHABET_SIZE 26
#define TARGET_VALUE 0.068

// Function to calculate the sum of the products of pi and qi for a given shift
double Ij(const double qi[], const double pi[]) {
    double sum = 0.0;

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        sum += qi[i] * pi[i];
    }

    return sum;
}

// Function which finds the shift using the improved attack on shift cipher
int find_shift(const char *ciphertext, const double pi[]) {
    double qi[ALPHABET_SIZE];
    double min_difference = INFINITY; 
    int best_shift = 0;

    // Counting occurrences of each letter in the ciphertext
    int length = strlen(ciphertext);
    for (int i = 0; i < length; i++) {
        char c = tolower(ciphertext[i]);
        if (isalpha(c)) {
            qi[c - 'a']++;
        }
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        qi[i] /= length;
    }


// Looping over all possible shifts
    for (int shift = 0; shift < ALPHABET_SIZE; shift++) {
        double sum_of_squares = Ij(qi, pi);

        // Update the best shift if the difference is smaller
        if (fabs(sum_of_squares - TARGET_VALUE) < min_difference) {
            min_difference = fabs(sum_of_squares - TARGET_VALUE);
            best_shift = shift;
        }

        double temp = qi[0];
        for (int i = 0; i < ALPHABET_SIZE - 1; i++) {
            qi[i] = qi[i + 1];
        }
        qi[ALPHABET_SIZE - 1] = temp;
    }

    return best_shift;
}

// Function to calculate the index of coincidence for a particular stream
double index_of_coincidence(const char *stream) {
    int length = strlen(stream);
    int frequency[ALPHABET_SIZE] = {0};
    double ic = 0.0;

    // Count frequencies of each letter
    for (int i = 0; i < length; i++) {
        char c = tolower(stream[i]);
        if (isalpha(c)) {
            frequency[c - 'a']++;
        }
    }

    // Calculate the index of coincidence
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        ic += frequency[i] * (frequency[i] - 1);
    }
    ic /= length * (length - 1);

    return ic;
}



void compute_and_print_ic(char ciphertext[], int key_length, double target_ioc, int *key_lengths, int *num_found) {
    // Arrays to store streams
    char *streams_key[key_length];
    for (int i = 0; i < key_length; i++) {
        streams_key[i] = (char *)malloc((strlen(ciphertext) / key_length + 1) * sizeof(char)); // Adding 1 for null terminator
        if (streams_key[i] == NULL) {
            perror("Memory allocation failed");
            
            exit(EXIT_FAILURE);
        }
        streams_key[i][0] = '\0'; // Null-terminate the string
    }

    // Populating the streams
    int str_index = 0;
    for (int i = 0; i < strlen(ciphertext); i++) {
        if (isalpha(ciphertext[i])) {
            strncat(streams_key[str_index], &ciphertext[i], 1); // Append one character to the stream
            str_index = (str_index + 1) % key_length;
        }
    }

    printf("Index of coincidence for each stream:\n");
    // Calculate average index of coincidence
    double sum_ic = 0.0;
    for (int i = 0; i < key_length; i++) {
        double ic = index_of_coincidence(streams_key[i]);
        printf("Stream %d: %.4f\n", i + 1, ic);
        sum_ic += ic;
    }
    printf("\n");

    double average_ic;
    
    average_ic = sum_ic / key_length;
    
   
    printf("Average Index of coincidence for all streams: %.4f\n", average_ic);
    printf("\n");




    // Check if the average IOC lies in this range and storing these values to compute gcd later
    if (average_ic >= 0.065 && average_ic <= 0.085) { 
        key_lengths[*num_found] = key_length;
        (*num_found)++;
    }

    // Free memory allocated for the streams
    for (int i = 0; i < key_length; i++) {
        free(streams_key[i]);
    }
}

// Function to calculate GCD of two numbers
int gcd(int a, int b) {
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

// Main function
int main() {
    // Storing the known frequencies of the standard English text into an array 
    const double pi[ALPHABET_SIZE] = {0.082, 0.015, 0.028, 0.043, 0.127, 0.022, 0.02, 0.061, 0.07, 0.002, 0.008, 0.04, 0.024, 0.067, 0.075, 0.019, 0.001, 0.06, 0.063, 0.091, 0.028, 0.01, 0.023, 0.001, 0.02, 0.001};

    FILE *file;
    char filename[] = "ciphertext.txt";    // File name can be changed accordingly
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *ciphertext = (char *)malloc((file_size + 1) * sizeof(char));
    if (ciphertext == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return EXIT_FAILURE;
    }

    fread(ciphertext, sizeof(char), file_size, file);
    ciphertext[file_size] = '\0';

    fclose(file);

    printf("Ciphertext read from file:\n%s\n\n", ciphertext);  // Printing the ciphertext read from the file

    int key_lengths[25]; // Array to store key lengths with close IOC values
    int num_found = 0; // Number of key lengths found with close IOC values

    printf("Using the method INDEX OF COINCIDENCE - Finding the probable key lengths\n");
    printf("------------------------------------------------------------------------\n\n\n");
    for (int t = 1; t <= 25; t++) {
        printf("For key length %d:\n", t);
        printf("-------------------\n");
        compute_and_print_ic(ciphertext, t, TARGET_VALUE, key_lengths, &num_found);
    }

    // Print key lengths with close IOC values
    printf("----------------------------------------------------------------------------------------------------------------\n");
    printf("Probable Key Lengths (All average indexes closer to the value 0.065 and slightly greater than this are filtered)\n");
    for (int i = 0; i < num_found; i++) {
        printf("%d ", key_lengths[i]);
    }
    printf("\n");

    // Calculating GCD of key lengths
    int key_length = key_lengths[0];
    for (int i = 1; i < num_found; i++) {
        key_length = gcd(key_length, key_lengths[i]);
    }

    printf("-----------------------------------------------------------------------------------------------------------------\n");
    printf(" USEFUL OBSERVATION - We can observe that when the key length given as input is the correct key length, the average value of index of coincidences is approximately equal to 0.065 or slightly greater than this.\n");
    printf("                      Assume this key length as 't', we can also observe that for multiples of 't' also we get the value approximately equal to 0.065 or slightly greater than this(This is due to the logic of division of streams and index of coincidence is nothing but finding the frequency of each letter in the stream and squaring it. Multiples of 't' also divide the ciphertext into streams whose average values equals 0.065 or slightly greater than this- But all these multiples might not be the correct key lengths and they might not produce meaningful texts also - just because of the logic I have mentioned we get a similar value of average for all multiples )\n");
    printf("                      And for the key lengths not multiples of 't', we can observe that the average value is approximately equal to some 0.045 in all cases.\n");
    printf(" So now storing all the key lengths whose average values are approximately equal to 0.065 or slightly greater than this and finding the gcd of all these key lengths to get the most probable estimated key length.\n");
    printf("------------------------------------------------------------------------------------------------------------------\n");
    printf("GCD of key lengths: %d\n", key_length);

    printf("Estimated key length : %d\n", key_length);
    printf("--------------------------------------------------------------------------------------------------------------\n");


    // Calculate maximum possible length for each stream - We know that it can't be more than teh value (ciphertextlength/key_length)
    int ciphertext_length = strlen(ciphertext);
    int stream_length = ciphertext_length / key_length; 

    // If there are remaining characters after distributing evenly, increase the stream length
    if (ciphertext_length % key_length > 0) {
        stream_length++;
    }


    char streams[key_length][stream_length];

    for (int i = 0; i < key_length; i++) {
        streams[i][0] = '\0'; // Null-terminate the string
    }

   
    // Populate the streams using the key length
    int stream_index = 0;
    for (int i = 0; i < ciphertext_length; i++) {
        if (isalpha(ciphertext[i])) {
            strncat(streams[stream_index], &ciphertext[i], 1); // Append one character to the stream
            stream_index = (stream_index + 1) % key_length;
        }
    }
    


    // Printing divided stream arrays
    printf("\n\n\nDIVIDED STREAM ARRAYS:\n");
    for (int i = 0; i < key_length; i++) {
        printf("Stream %d: %s\n", i + 1, streams[i]);
    }
    printf("\n");

    // Find shifts for each stream
    int shifts[key_length];
    for (int i = 0; i < key_length; i++) {
        shifts[i] = find_shift(streams[i], pi);
    }


    // Calculate and print index of coincidence for each stream
    printf("Index of coincidence for each stream when the key length is %d:\n", key_length);
    double sum_ic = 0.0;
    for (int i = 0; i < key_length; i++) {
        double ic = index_of_coincidence(streams[i]);
        printf("Stream %d: %.4f\n", i + 1, ic);
        sum_ic += ic;
    }
    printf("\n");

    // Calculate average index of coincidence
    double average_ic;
    
    average_ic = sum_ic / key_length;
    
    
    printf("Average Index of coincidence for all streams: %.4f\n", average_ic);
    printf("\n");


    // Print shifts for each stream
    printf("SHIFTS FOR EACH STREAM USING IMPROVED ATTACK ON SHIFT CIPHER:\n");
    printf("--------------------------------------------------------------\n");
    for (int i = 0; i < key_length; i++) {
        printf("Stream %d shift: %d\n", i + 1, shifts[i]);
    }
    printf("\n");


    printf("--------------------------------------------------------------\n");
    printf("KEYWORD: ");
    for (int i = 0; i < key_length; i++) {
        char keyword_char = 'a' + shifts[i]; // Assuming lowercase letters
        printf("%c", keyword_char);
    }
    printf("\n");
    printf("--------------------------------------------------------------\n");

    // Decrypt each stream
    printf("\nDECRYPTED STREAM ARRAYS:\n");
    printf("--------------------------\n");
    
    for (int i = 0; i < key_length; i++) {
        printf("Stream %d decrypted: ", i + 1);
        for (int j = 0; j < strlen(streams[i]); j++) {
            char shifted_char = streams[i][j] - shifts[i];
            if (islower(streams[i][j])) {
                if (shifted_char < 'a') {
                    shifted_char += ALPHABET_SIZE;
                }
            } else if (isupper(streams[i][j])) {
                if (shifted_char < 'A') {
                    shifted_char += ALPHABET_SIZE;
                }
            }
            printf("%c", shifted_char);
        }
        printf("\n");
    }
    printf("\n");
    
    char msg[strlen(ciphertext)];
    int index = 0;
    // Merge streams to obtain the original message
    printf("MERGING ALL OF THE ABOVE STREAMS:\n");
    printf("--------------------------------\n");
    for (int j = 0; j < strlen(streams[0]); j++) {
        for (int i = 0; i < key_length; i++) {
            char shifted_char = streams[i][j] - shifts[i];
            if (islower(streams[i][j])) {
                if (shifted_char < 'a') {
                    shifted_char += ALPHABET_SIZE;
                }
            } else if (isupper(streams[i][j])) {
                if (shifted_char < 'A') {
                    shifted_char += ALPHABET_SIZE;
                }
            }
            if (isalpha(streams[i][j])) {
                msg[index++] = shifted_char;
                printf("%c", shifted_char);
            } else {
                msg[index++] = streams[i][j]; 
                printf("%c", streams[i][j]);
            }
        }
    }
    printf("\n");

    msg[index] = '\0'; // Null-terminate the string
/*
    printf("\n\nMESSAGE AFTER MERGING\n");
    printf("-------------------------\n");
    printf("%s\n", msg)
*/
    char final_message[strlen(ciphertext)];   // To store the final message
    int final_index = 0;

    // Comparing msg array with the ciphertext array and getting the final decrypted message
    int msg_index = 0;
    for (int i = 0; i < strlen(ciphertext); i++) {
        if (ciphertext[i] == ' ') {
            final_message[final_index++] = ' '; // Copy space directly
        } else if (isalpha(ciphertext[i])) {
            if (isalpha(msg[msg_index])) {
                final_message[final_index++] = msg[msg_index++];
            } 
        } else {
            final_message[final_index++] = ciphertext[i];
        }
    }

    final_message[final_index] = '\0'; // Null-terminate the string

    printf("\n");
    printf("FINAL DECRYPTED MESSAGE\n");
    printf("-----------------------\n");
    printf("%s\n", final_message);

    return 0;
}

    



