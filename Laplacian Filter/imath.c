#include <sys/time.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>

#define THREADS 4

#define filterWidth 3
#define filterHeight 3

#define RGB_MAX 255

//PPMPixel structure to store info about red, green and blue values 
typedef struct {

	 unsigned char r, g, b;
} PPMPixel;

//Structure parameter. Will be used to store info about the image, resulting image, width and height 
//of the image, the starting point of the work as well as the size of the work as well. 
struct parameter {
	
	PPMPixel *image;         
	PPMPixel *result;        
	unsigned long int w;     
	unsigned long int h;     
	unsigned long int start; 
	unsigned long int size; 
};

//So this function is the thread function that splits up the work between four separate threads. It will loop through the filter and change the red, green 
//and blue pixel values according to the values in the laplacian filter. 
void *threadfn(void *params) {
	
	//Here we cast the void argument to the structure we need in order to affect the data 
	struct parameter *paramsInfo = (struct parameter *) params;

	//Laplacian filter 
	int laplacian[filterWidth][filterHeight] =
	{
	  {-1, -1, -1},
	  {-1,  8, -1},
	  {-1, -1, -1},
	};

	//Here I set the values of the parameters structure. 
	int start = paramsInfo -> start;
	int end = paramsInfo -> start + paramsInfo -> size;
	int width = paramsInfo -> w;
	int height = paramsInfo -> h;
	PPMPixel *result = paramsInfo -> result;
	PPMPixel *image = paramsInfo -> image;
	
	//Here are the four main nested loops needed to filter the image. We start with the image height, and then the image width. We will also create red, green 
	//and blue integer values so we can store the new values in the inner most nested loop
	for(int row = start; row < end; row++){

		for(int col = 0; col < (paramsInfo -> w); col++){

			int red = 0;
			int green = 0;
			int blue = 0;

			//Here is our inner most nested loop which does most of the work. We are calculating our new x and y coordinate values for our pixel image, 
			//and then changing those values in accordance with our red, green and blue integer values 
			for(int lapWidth = 0; lapWidth < filterWidth; lapWidth++){

				for(int lapHeight = 0; lapHeight < filterHeight; lapHeight++){

					int x_coordinate = (col - filterWidth / 2 + lapWidth + width) % width;
					int y_coordinate = (row - filterHeight / 2 + lapHeight + height) % height;
					red += image[y_coordinate * width + x_coordinate].r * 
								laplacian[lapHeight][lapWidth];
					green += image[y_coordinate * width + x_coordinate].g * 
								laplacian[lapHeight][lapWidth];
					blue += image[y_coordinate * width + x_coordinate].b * 
								laplacian[lapHeight][lapWidth];
				}
			}

			//So here, if the red, green or blue values are less than zero. We simply set them to zero so that they all contain positive values. 
			if(red <= 0){ red = 0; }
			if(green <= 0){ green = 0; }
			if(blue <= 0){ blue = 0; }

			if(red > 255) red = 255;
			if(green > 255) green = 255;
			if(blue > 255) blue = 255;

			//Here we store the result in the PPMpixel result image. This will be used to write our new image. 
			result[row * width + col].r = red;
			result[row * width + col].g = green;
			result[row * width + col].b = blue; 
		}
	}

	//Exit the thread here 
	pthread_exit(NULL);
}

//This method will be used to write the image. We simply open the image file, write the necessary header data, and then the resulting image data. 
void writeImage(PPMPixel *image, char *name, unsigned long int width, unsigned long int height) {
  
	//Here we create a file pointer and open the file
	FILE *outFile = fopen(name, "w");

	//Check to make sure the output file was opened correctly 
	if(outFile == NULL){
		
		fprintf(stderr, "Error opening %s file \n", name);
		return;
  	}
  
	//Here we check the header to make sure the following first value is P6
	fprintf(outFile, "P6\n%d %d\n%d\n", ((int)width), ((int)height), RGB_MAX);

	//Write the image data.
	PPMPixel *temp = image;
	size_t sizePPMP = sizeof(PPMPixel);
  
	//Iterate over entire image data and write it.
	for(int i = 0; i <= (width * height); i++){

		if(fwrite(temp, sizePPMP, 1, outFile) <= 0)
			printf("Error writing file!\n");
		
		temp++;
	}

	//Close the file.
	fclose(outFile);

	return;
}

//This method will read the image of the file and return the image. Using fread to read the image data. As well as fscanf, which is useful 
//for reading the header data. We will then return the image in this function. 
PPMPixel *readImage(const char *filename, unsigned long int *width, unsigned long int *height) {

	//Here are just some variables we will need to read our image data. PPMPixel will store image, line for getting the next line of data in the file, 
	//size for the size of input, lLen for length of input and then maxCV for the maximum color value. 
	PPMPixel *img;
	char **line = malloc(100);
	size_t size = 0; 
	ssize_t lLen;
	int maxCV;

	//Open file
	FILE *inputFile = fopen(filename, "r");

	if(inputFile == NULL){
	  printf("Error opening file !\n");
	  exit(1);
	}

	//Read image format.
	char imgForm[3];
	fgets(imgForm, 3, inputFile);

	//Check if correct image format.
	if(strcmp(imgForm, "P6") != 0){
	  printf("Image has incorrect image format, must be P6. \n");
	  fclose(inputFile);
	  free(line);
	  exit(1);
	}

	//Move to the start of the next line.
	lLen = getline(line, &size, inputFile);
	if(lLen < 0)
	  printf("Failed getline \n");
	
	int i = 0;
	char temp[10];
	
	//For this loop we will search the header for the width and height of the max color values 
	while(i < 2){ 
	  fscanf(inputFile, "%s", temp);

	  //Skip comment lines.
	  if(strcmp(temp, "#") == 0){
	    
	    //Move to the start of the next line.
	    lLen = getline(line, &size, inputFile);
	    if(lLen < 0)
	      printf("Failed getline \n");
	  }

	  //It's not a comment line.
	  else{

	    //get width and height
	    if(i == 0){

	      //Convert temp to int for width
	      *width = strtoul(temp, NULL, 0);
	      
	      //Get height
	      fscanf(inputFile, "%lu", height);

	      //Move to the start of the next line.
	      lLen = getline(line, &size, inputFile);
	      if(lLen < 0)
			printf("Failed getline \n");
	      i++;
	    }

	    //Get max color value
	    else{
	      maxCV = atoi(temp);
	      i++;
	    } 
	  }
	}

	//Check if Max Color Value is equal to RGB_MAX. If not, display error message.
	if(maxCV != RGB_MAX){
	  printf("Error: max color value must equal %d \n", RGB_MAX);
	  fclose(inputFile);
	  free(line);
	  exit(1);
	}
	
	//Allocate memory for img. NOTE: A ppm image of w=200 and h=300 will contain 60000 triplets (i.e. for r,g,b), ---> 180,000 bytes.
	img = malloc(*width * *height * 3);

	//Read pixel data from filename into img. The pixel data is stored in scanline order from left to right (up to bottom) in 3-byte chunks (r g b values for each pixel) encoded as binary numbers.
	PPMPixel *tempP = img;

	//Move to the start of the next line. B/C after maxCV value in ppm file there's one \n.
	lLen = getline(line, &size, inputFile);
	if(lLen < 0)
	  printf("Failed getline \n");
	
	//While we are reading the image data, move to the next PPMPixel of the struct 
	while(fread(tempP, sizeof(PPMPixel), 1, inputFile))
	  tempP++;	  

	//Free line and close image input file
	free(line);
	fclose(inputFile);
	
	return img;
}

//This method creates our threads and applies our filter to our image as well. 
PPMPixel *apply_filters(PPMPixel *image, unsigned long w, unsigned long h, double *elapsedTime) {

	//Here are the variables we will need for this function. Result will store the resulting image, which this function will return. We also allocate the 
	//necessary memory for the resulting image as well. I went ahead and created a thread array, because I found that much easier to join the threads. 
	//Finally, we create our parameter array (one for each thread), and calculate the required amount of work for each thread as well. 
  	PPMPixel *result;
	result = malloc(w * h * 3);
	pthread_t thread[THREADS];
	struct parameter *parameterArray[THREADS];
	int work = h / THREADS;

	//Here we have two variables to keep track of the time. As well as we get the starting time and assign it to start. 
	struct timeval start, end;
	if(gettimeofday(&start, NULL) != 0)
		printf("Error setting start time.\n");

	//Here we allocate the memory for each parameter array for our threads. Most of this is pretty basic, because we are simply assigning our 
	//parameter variables to be most of what we already know. (width of image, height of image, etc.)
	for(int i = 0; i < THREADS; i++){

		parameterArray[i] = malloc(sizeof(struct parameter));
		parameterArray[i] -> start = (i * work);
		parameterArray[i] -> w = w;
		parameterArray[i] -> h = h;
		parameterArray[i] -> result = result;
		parameterArray[i] -> image = image;
		if(i != 3){ parameterArray[i] -> size = work; }
		else{ parameterArray[i] -> size = (h - (parameterArray[i] -> start)); } 
	}
	
	//So this loop here will create our threads. We pass through the necessary data as well, casting our parameter structure to a void argument. Also check for errors. 
	for(int i = 0; i < THREADS; i++){
		
		int s = pthread_create(&thread[i], NULL, threadfn, (void *) parameterArray[i]);
		if(s != 0)
			printf("Error creating threads.\n");
	}

	//Here we simply join our threads at the end. 
	for(int i = 0; i < THREADS; i++)
		if(pthread_join(thread[i], NULL) != 0)
			printf("Error joining threads.\n");
	
	//Finally, we store the end time in variable end, and calculate how long it took our program to run. Returning the resulting image as well. 
	if(gettimeofday(&end, NULL) != 0)
		printf("Error setting end time.\n");
	
	*elapsedTime = (end.tv_sec - start.tv_sec) * 1e6;
	*elapsedTime = (*elapsedTime + (end.tv_usec - start.tv_usec)) * 1e-6;

	return result;
}

//Main method, which is used as the driver for our program. This program will read an image, create a laplacian filter for our image, and store the resulting image 
//in a file called laplacian.ppm. It is used to create a laplacian filter for our image, given the assignment description. 
int main(int argc, char *argv[]) {
	
    //Here we have variables to store the width and height of our image, as well as the total time taken for our program to run. 
    unsigned long int w, h;
    double elapsedTime = 0.0;

    //Check to make sure arguments are two
    if(argc != 2){
      printf("Usage: ./imath filename\n");
      return 1;
    }
	
	//Create two PPMPixel structures. One to read the image, and then one to store the resulting image after applying the filter. 
    PPMPixel *img = readImage(argv[1], &w, &h);
    PPMPixel *result = apply_filters(img, w, h, &elapsedTime);

	//Resulting file image will be called laplacian.ppm. We also write the image here as well. 
    char *name = "laplacian.ppm";
    writeImage(result, name, w, h);
    
	//Here we simply print the total time taken for our program to run. As well as free the memory allocated for the image and resulting image. 
    printf("%.3f\n", elapsedTime);
    free(img);
    free(result);

    return 0;
}

