/*
 * Userspace program that communicates with the top device driver
 * through ioctls
 *
 * Hang Ye
 * Columbia University
 */

 #include <stdio.h>
 #include "top.h"
 #include <sys/ioctl.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <string.h>
 #include <unistd.h>
 
 int top_fd;
 
 /* Write img_size */
 void write_img_size(unsigned char img_size) {
     top_arg_t vla;
     vla.img_size.img_size = img_size;
     if (ioctl(top_fd, TOP_WRITE_IMG_SIZE, &vla)) {
         perror("ioctl(TOP_WRITE_IMG_SIZE) failed");
         return;
     }
 }
 
 /* Write input_data */
 void write_input_data(unsigned char input_data) {
     top_arg_t vla;
     vla.input_data.input_data = input_data;
     if (ioctl(top_fd, TOP_WRITE_INPUT_DATA, &vla)) {
         perror("ioctl(TOP_WRITE_INPUT_DATA) failed");
         return;
     }
 }
 
 /* Write weight_data */
 void write_weight_data(unsigned char weight_data) {
     top_arg_t vla;
     vla.weight_data.weight_data = weight_data;
     if (ioctl(top_fd, TOP_WRITE_WEIGHT_DATA, &vla)) {
         perror("ioctl(TOP_WRITE_WEIGHT_DATA) failed");
         return;
     }
 }
 
 /* Read done signal */
 unsigned char read_done() {
     top_arg_t vla;
     if (ioctl(top_fd, TOP_READ_DONE, &vla)) {
         perror("ioctl(TOP_READ_DONE) failed");
         return 0;
     }
     return vla.done.done;
 }
 
 /* Read output_data */
 unsigned char read_output_data() {
     top_arg_t vla;
     if (ioctl(top_fd, TOP_READ_OUTPUT_DATA, &vla)) {
         perror("ioctl(TOP_READ_OUTPUT_DATA) failed");
         return 0;
     }
     return vla.output_data.output_data;
 }
 
 int main() {
     static const char filename[] = "/dev/top";
     unsigned char img_size = 4; // Example img_size
     unsigned char input_data[16];
     unsigned char weight_data[16];
     unsigned char output_data[16];
     int i;
 
     printf("Top Userspace program started\n");
 
     if ((top_fd = open(filename, O_RDWR)) == -1) {
         fprintf(stderr, "could not open %s\n", filename);
         return -1;
     }
 
     // Initialize input_data and weight_data
     for (i = 0; i < 16; i++) {
         input_data[i] = i;       // Example input data: 0, 1, 2, ...
         weight_data[i] = i + 1;  // Example weight data: 1, 2, 3, ...
     }
 
     // Write img_size
     printf("Writing img_size: %d\n", img_size);
     write_img_size(img_size);
 
     // Write input_data
     printf("Writing input_data:\n");
     for (i = 0; i < 16; i++) {
         printf("  input_data[%d] = %d\n", i, input_data[i]);
         write_input_data(input_data[i]);
     }
 
     // Write weight_data
     printf("Writing weight_data:\n");
     for (i = 0; i < 16; i++) {
         printf("  weight_data[%d] = %d\n", i, weight_data[i]);
         write_weight_data(weight_data[i]);
     }
 
     // Wait for done signal
     printf("Waiting for done signal...\n");
     while (read_done() == 0) {
         usleep(100000); // Sleep for 100ms
     }
     printf("Done signal received!\n");
 
     // Read output_data
     printf("Reading output_data:\n");
     for (i = 0; i < 16; i++) {
         output_data[i] = read_output_data();
         printf("  output_data[%d] = %d\n", i, output_data[i]);
 
         // Verify output_data
         if (output_data[i] != input_data[i] + weight_data[i]) {
             fprintf(stderr, "Error: output_data[%d] = %d (expected %d)\n",
                     i, output_data[i], input_data[i] + weight_data[i]);
         }
     }
     // for simplification, we assume output_data = input_data * weight_data
     printf("Top Userspace program terminating\n");
     return 0;
 }