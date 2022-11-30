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

double conv_img[DIM][DIM];
int second_img[DIM][DIM];

/******************************************************************/
/* Main processing routine. This is called upon pressing the      */
/* Process button of the interface.                               */
/* image  - the original greyscale image                          */
/* size   - the actual size of the image                          */
/* proc_image - the image representation resulting from the       */
/*              processing. This will be displayed upon return    */
/*              from this function.                               */
/******************************************************************/    
 
void process_image(image, size, proc_img)
unsigned char image[DIM][DIM];
int size[2];
unsigned char proc_img[DIM][DIM];
{   
    // template matching 
    
    // find average of image
    double img_avg = 0;
    for(int r = 0; r < size[1]; r++)
    {
        for(int c = 0; c < size[0]; c++)
        {
            img_avg += image[c][r];
        }
    }
    img_avg /= (size[0] * size[1]);

    // find standard deviation of image
    double img_std = 0;
    for(int r = 0; r < size[1]; r++)
    {
        for(int c = 0; c < size[0]; c++)
        {
            img_std += ((double)image[c][r] - img_avg) * ((double)image[c][r] - img_avg);
        }
    }
    img_std = sqrt(img_std);
    printf("img_std = %d\n", img_std);
    if(roi.height % 2 != 1)
    {
        roi.height += 1;
    }
    if(roi.height % 2 != 1)
    {
        roi.height += 1;
    }
    
    // find average of template
    double temp_avg = 0;
    for(int r = roi.y; r < roi.y + roi.height; r++)
    {
        for(int c = roi.x; c < roi.x + roi.width; c++)
        {
            temp_avg += image[c][r];
        }
    }
    temp_avg /= (roi.width * roi.height);

    // find standard deviation of template
    double temp_std = 0;
    for(int r = roi.y; r < roi.y + roi.height; r++)
    {
        for(int c = roi.x; c < roi.x + roi.width; c++)
        {
            temp_std += ((double)image[c][r] - temp_avg) * ((double)image[c][r] - temp_avg);
        }
    }
    temp_std = sqrt(temp_std);
    printf("temp_std = %d\n", temp_std);

    // convolution
    double min = 255;
    double max = 0;
    for(int r = roi.height/2; r < size[1] - roi.height/2; r++)
    {
        for(int c = roi.width/2; c < size[0] - roi.width/2; c++)
        {
            conv_img[c][r] = 0;
            for(int j = -roi.height/2; j < roi.height/2; j++)
            {
                for(int k = -roi.width/2; k < roi.width/2; k++)
                {   
                    conv_img[c][r] += ((double)image[c + k][r + j] - (double)img_avg) * ((double)image[roi.x + roi.width/2 + k][roi.y + roi.height/2 + j] - (double)temp_avg);
                }
            }
            conv_img[c][r] /= (img_std * temp_std);
            if(conv_img[c][r] < min)
            {
                min = conv_img[c][r];
            }
            if(conv_img[c][r] > max)
            {
                max = conv_img[c][r];
            }
        }
    }

    // normalization
    double m = (255.0 - 0.0) / (max - min);
    double b = m * -min;
    for(int r = roi.height/2; r < size[1] - roi.height/2; r++)
    {
        for(int c = roi.width/2; c < size[0] - roi.width/2; c++)
        {
            proc_img[c-roi.width/2][r-roi.height/2] = m * conv_img[c][r] + b;
        }
    }

    // did not compute convolution at boundaries
    size[1] -= roi.height/2 * 2;
    size[0] -= roi.width/2 * 2;
}


