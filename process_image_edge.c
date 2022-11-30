#include <stdio.h>
#include <math.h>
#include <X11/Xlib.h>

#define DIM 512

/******************************************************************/
/* This structure contains the coordinates of a box drawn with    */
/* the left mouse button on the image window.                     */
/* roi.x , roi.y  - left upper corner's coordinates               */
/* roi.width , roi.height - width and height of the box           */
/******************************************************************/
extern XRectangle roi;


/******************************************************************/
/* Main processing routine. This is called upon pressing the      */
/* Process button of the interface.                               */
/* image  - the original greyscale image                          */
/* size   - the actual size of the image                          */
/* proc_image - the image representation resulting from the       */
/*              processing. This will be displayed upon return    */
/*              from this function.                               */
/******************************************************************/
void vertical_edge(unsigned char image[DIM][DIM], int size[2], int conv_img[size[0]][size[1]], int* min, int* max)
{
    for(int r = 1; r < size[1] - 1; r++)
    {
        for(int c = 1; c < size[0] - 1; c++)
        {
            conv_img[c][r] = 0;
            conv_img[c][r] -= image[c-1][r-1];
            conv_img[c][r] -= image[c-1][r];
            conv_img[c][r] -= image[c-1][r+1];
            conv_img[c][r] += image[c+1][r-1];
            conv_img[c][r] += image[c+1][r];
            conv_img[c][r] += image[c+1][r+1];
            if(conv_img[c][r] < 0)
                conv_img[c][r] *= -1;
            if(conv_img[c][r] < *min)
                *min = conv_img[c][r];
            if(conv_img[c][r] > *max)
                *max = conv_img[c][r];
        }
    }
}
void horizontal_edge(unsigned char image[DIM][DIM], int size[2], int conv_img[size[0]][size[1]], int* min, int* max)
{
    for(int r = 1; r < size[1] - 1; r++)
    {
        for(int c = 1; c < size[0] - 1; c++)
        {
            conv_img[c][r] = 0;
            conv_img[c][r] -= image[c-1][r-1];
            conv_img[c][r] -= image[c][r-1];
            conv_img[c][r] -= image[c+1][r-1];
            conv_img[c][r] += image[c-1][r+1];
            conv_img[c][r] += image[c][r+1];
            conv_img[c][r] += image[c+1][r+1];
            if(conv_img[c][r] < 0)
                conv_img[c][r] *= -1;
            if(conv_img[c][r] < *min)
                *min = conv_img[c][r];
            if(conv_img[c][r] > *max)
                *max = conv_img[c][r];
        }
    }
}
void maj_diag_edge(unsigned char image[DIM][DIM], int size[2], int conv_img[size[0]][size[1]], int* min, int* max)
{
    for(int r = 1; r < size[1] - 1; r++)
    {
        for(int c = 1; c < size[0] - 1; c++)
        {
            conv_img[c][r] = 0;
            conv_img[c][r] -= image[c][r-1];
            conv_img[c][r] -= image[c+1][r-1];
            conv_img[c][r] -= image[c+1][r];
            conv_img[c][r] += image[c-1][r];
            conv_img[c][r] += image[c-1][r+1];
            conv_img[c][r] += image[c][r+1];
            if(conv_img[c][r] < 0)
                conv_img[c][r] *= -1;
            if(conv_img[c][r] < *min)
                *min = conv_img[c][r];
            if(conv_img[c][r] > *max)
                *max = conv_img[c][r];
        }
    }
}
void min_diag_edge(unsigned char image[DIM][DIM], int size[2], int conv_img[size[0]][size[1]], int* min, int* max)
{
    for(int r = 1; r < size[1] - 1; r++)
    {
        for(int c = 1; c < size[0] - 1; c++)
        {
            conv_img[c][r] = 0;
            conv_img[c][r] -= image[c-1][r-1];
            conv_img[c][r] -= image[c][r-1];
            conv_img[c][r] -= image[c-1][r];
            conv_img[c][r] += image[c+1][r];
            conv_img[c][r] += image[c][r+1];
            conv_img[c][r] += image[c+1][r+1];
            if(conv_img[c][r] < 0)
                conv_img[c][r] *= -1;
            if(conv_img[c][r] < *min)
                *min = conv_img[c][r];
            if(conv_img[c][r] > *max)
                *max = conv_img[c][r];
        }
    }
}
void process_image(image, size, proc_img)
unsigned char image[DIM][DIM];
int size[2];
unsigned char proc_img[DIM][DIM];
{    
    int min = 255;
    int max = 0;
    int conv_img[size[0]][size[1]];

    vertical_edge(image, size, conv_img, &min, &max);
    // horizontal_edge(image, size, conv_img, &min, &max);
    // maj_diag_edge(image, size, conv_img, &min, &max);
    // min_diag_edge(image, size, conv_img, &min, &max);

    double m = (255.0 - 0.0) / (max - min);
    double b = m * -min;
    for(int r = 1; r < size[1] - 1; r++)
    {
        for(int c = 1; c < size[0] - 1; c++)
        {
            proc_img[c-1][r-1] = m * conv_img[c][r] + b;
        }
    }
    
    size[1] -= 2;
    size[0] -= 2;
}
