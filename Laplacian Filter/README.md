# Laplacian Filter
This program uses a laplacian filter to compute the second derivative of the pixels in the image. Laplacian filters are used as edge detectors and can detect areas in the image in 
which the change in color is the greatest. We use laplacian filters to determine the edges of the image. 
# Usage
Simply clone the repository or copy all files into the same directory. Run the make command on your desired command line interface (or gcc command), and simply run the executable. 
Command line arguments (argv[1]) will need an image associated in this repository. './filename image.ppm'
# What I Learned
- Laplacian filters are used as edge detectors and can determine the greatest change in color in an image. 
- Threads can improve the performance of a program by splitting up the work between processes. However, too many threads can also slow down a process as well. 
