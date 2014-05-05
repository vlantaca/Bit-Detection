#include <stdio.h>
#include <stdlib.h>
#include <math.h>



//#define DEBUG


int main(int argc, char **argv) {

   /* For each argument, try to open it as a file and read it */
   int i, j;



   int sample_count = 1;
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
      int high_length = 0;

      char ch1, ch2;
      int number1, number2, number;

#ifdef DEBUG
      printf("running...\n");
#endif

      int index = 1;
      int index_of_last_high = 0;
      int index_of_last_low = 0;

      for (;;){
		
   		if (fscanf(file, "%d", &number) <= 0) break;

   		if (abs(  (double)number ) <= 300000){
            
   			if (low_length >= 190){

#ifdef DEBUG
               printf("%d.) case 1\n", index);
#endif

               low_length = 0;
   				high_length = 0;
               index_of_last_low = 0;
               index_of_last_high = 0;


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
               
   			}else{
               if (index_of_last_low == 0){
                  index_of_last_low = index;
                  continue;
               }
               low_length += index - index_of_last_low;
            }
            index_of_last_low = index;
   		}else{

#ifdef DEBUG
            printf("   %d.) > 300k, high_length = %d, low_length = %d, old_hi = %d, old_lo = %d\n", index, high_length, low_length, index_of_last_high, index_of_last_low);
#endif

   			if (high_length >= 190){

#ifdef DEBUG
               printf("%d.) case 2\n", index);
#endif
               
               high_length = 0;
   				low_length = 0;
               index_of_last_low = 0;
               index_of_last_high = 0;

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

   			}else{
               if (index_of_last_high == 0){
                  index_of_last_high = index;
                  continue;
               }
               high_length += index - index_of_last_high;
               low_length = 0;
            }

            index_of_last_high = index;
   		}

   		//printf("%zu\n", abs( (double)number ));
         index++;
      }
   }
   return 0;
}