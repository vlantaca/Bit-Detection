#include <stdio.h>
#include <stdlib.h>


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv) {

   /* For each argument, try to open it as a file and read it */
   int i, j;

   int flag1 = 0;
   int flag2 = 0;
   int flag3 = 0;
   int flag4 = 0;
   int flag5 = 0;
   int high = 0;
   int high_length = 0;

   int sample_count = 0;
   int START_DETECTED = 0;
   for(i=1; i<argc; i++){
      char *filename = argv[i];
      FILE *file = fopen(filename, "r"); /* read mode */
      if (file == NULL){
         perror("Error while opening the file.\n");
         exit(1);
      }

      int manchester1 = 0; // first half of manchester
      int manchester2 = 0; // second half of manchester



      /*

	  current_half = 0 iff we are analyzing the first half of the
	  manchester bit.

	  current_half = 0 iff we are analyzing the second half of the
	  manchester bit

      */
      int current_half = 0;
      int low_length = 0;


      char ch1, ch2;
      int number1, number2, number;

      printf("running...\n");

      int index = 0;

      for (;;){
		
		if (fscanf(file, "%d", &number) <= 0) break;

		if (abs(  (double)number ) > 300000){
			if (low_length > 190){
				high_length = 0;
				if (current_half == 0){
					manchester1 = 0;
					current_half = 1;
				}else{
					manchester2 = 0;
					current_half = 0;

					/*
					 since we processed the second half of a
					 manchester bit, we can now print out the bit
					*/
						if (manchester1 == 1 && manchester2 == 0){
							printf("%d.) 0 detected\n", index);
							manchester1 = 2;
							manchester2 = 2;
						}else if(manchester1 == 0 && manchester2 == 1){
							printf("%d.) 1 detected\n", index);
							manchester1 = 2;
							manchester2 = 2;
						}
				}
				low_length = 0;
			}

			high_length += sample_count;
			sample_count = 0;

		}else{
			if (high_length > 190){
				low_length = 0;
				if (START_DETECTED == 0){
					START_DETECTED = 1;
					printf("%d.) START_BIT_DETECTED\n", index);
				}else if (START_DETECTED == 1){
					if (current_half == 0){
						manchester1 = 1;
						current_half = 1;
					}else{
						manchester2 = 1;
						current_half = 0;

						/*
						 since we processed the second half of a
						 manchester bit, we can now print out the bit
						*/
						if (manchester1 == 1 && manchester2 == 0){
							printf("%d.) 0 detected\n", index);
							manchester1 = 2;
							manchester2 = 2;
						}else if(manchester1 == 0 && manchester2 == 1){
							printf("%d.) 1 detected\n", index);
							manchester1 = 2;
							manchester2 = 2;
						}
					}
				}
				high_length = 0;
			}
			low_length += sample_count;
			sample_count = 0;
			index++;
		}

		//printf("%zu\n", abs( (double)number ));

		sample_count++;
      }
   }
   return 0;
}