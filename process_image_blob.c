#include <stdio.h>
#include <math.h>
#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdbool.h>
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

struct Samesies {
    int smaller;
    int bigger;
    struct Samesies* next;
};


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
    // color blob
    
    int numRegions = 0;
    int* regions = (int*)malloc((numRegions+1)*sizeof(int));
    struct Samesies* sameRegions= (struct Samesies*)malloc(sizeof(struct Samesies));
    // give dummy head
    sameRegions->bigger = -1;
    sameRegions->smaller = -1;
    sameRegions->next = NULL;
    int numSamesies = 0;
    int first_img[size[0]][size[1]];
    int threshold = 17;

    for(int r = 0; r < size[1]; r++)
    {
        for(int c = 0; c < size[0]; c++)
        {
            if((r-1 >= 0) && (c-1 >= 0))
            {
                if(image[c][r] >= (image[c][r-1] - threshold) && image[c][r] <= (image[c][r-1] + threshold) && image[c][r] >= (image[c-1][r] - threshold) && image[c][r] <= (image[c-1][r] + threshold) && first_img[c][r-1] != first_img[c-1][r])
                {
                    numSamesies++;
                    struct Samesies* curr = sameRegions;
                    while(curr->next != NULL)
                    {
                        curr = curr->next;
                    }
                    curr->next = (struct Samesies*)malloc(sizeof(struct Samesies));
                    if(first_img[c][r-1] < first_img[c-1][r])
                    {
                        first_img[c][r] = first_img[c][r-1];
                        curr->next->smaller = first_img[c][r-1];
                        curr->next->bigger = first_img[c-1][r];
                    }
                    else
                    {
                        first_img[c][r] = first_img[c-1][r];
                        curr->next->smaller = first_img[c-1][r];
                        curr->next->bigger = first_img[c][r-1];
                    }
                    curr->next->next = NULL;
                }
                else if(image[c][r] >= (image[c][r-1] - threshold) && image[c][r] <= (image[c][r-1] + threshold))
                {
                    first_img[c][r] = first_img[c][r-1];
                }
                else if(image[c][r] >= (image[c-1][r] - threshold) && image[c][r] <= (image[c-1][r] + threshold))
                {
                    first_img[c][r] = first_img[c-1][r];
                }
                else
                {
                    first_img[c][r] = numRegions;
                    regions[numRegions] = numRegions;
                    numRegions++;
                    regions = realloc(regions, (numRegions+1)*sizeof(int));
                }
            }
            else if(c-1 >= 0)
            {
                if(image[c][r] >= (image[c-1][r] - threshold) && image[c][r] <= (image[c-1][r] + threshold))
                {
                    first_img[c][r] = first_img[c-1][r];
                }
                else
                {
                    first_img[c][r] = numRegions;
                    regions[numRegions] = numRegions;
                    numRegions++;
                    regions = realloc(regions, (numRegions+1)*sizeof(int));
                }
            }
            else if(r-1 >= 0)
            {
                if(image[c][r] >= (image[c][r-1] - threshold) && image[c][r] <= (image[c][r-1] + threshold))
                {
                    first_img[c][r] = first_img[c][r-1];
                }
                else
                {
                    first_img[c][r] = numRegions;
                    regions[numRegions] = numRegions;
                    numRegions++;
                    regions = realloc(regions, (numRegions+1)*sizeof(int));
                }
            }
            
            else
            {
                first_img[c][r] = numRegions;
                regions[numRegions] = numRegions;
                numRegions++;
                regions = realloc(regions, (numRegions+1)*sizeof(int));
            }
        }
    }

    int matchCount = 0;
    for(int r = 0; r < size[1]; r++)
    {
        for(int c = 0; c < size[0]; c++)
        {
            bool match = false;
            struct Samesies* currNode = sameRegions->next; // has dummy head
            int currValue = first_img[c][r];
            while(currNode != NULL)
            {
                if(currNode->bigger == currValue)
                {
                    second_img[c][r] = currNode->smaller;
                    currValue = currNode->smaller;
                    match = true;
                    matchCount++;
                }
                currNode = currNode->next;
            }
            if(match == false)
            {
                second_img[c][r] = first_img[c][r];
            }
        }
    }
   
    int numUniqueRegions = 1;
    int* uniqueRegions = (int*)malloc((numUniqueRegions+1)*sizeof(int));
    uniqueRegions[0] = second_img[0][0];
    for(int r = 0; r < size[1]; r++)
    {
        for(int c = 0; c < size[0]; c++)
        {
            bool match = false;
            int j = 0;
            while((match == false) && (j < numUniqueRegions))
            {
                if(second_img[c][r] == uniqueRegions[j])
                {
                    match = true;
                }
                j++;
            }
            //printf("match = %d, j = %d\n", match, j);
            if(match == false)
            {
                uniqueRegions[numUniqueRegions] = second_img[c][r];
                numUniqueRegions++;
                uniqueRegions = realloc(uniqueRegions, (numUniqueRegions+1)*sizeof(int));
            }
        }
    }

    int minRegion = DIM*DIM;
    int maxRegion = 0;
    for(int j = 0; j < numUniqueRegions; j++)
    {
        if(uniqueRegions[j] < minRegion)
        {
            minRegion = uniqueRegions[j];
        }
        if(uniqueRegions[j] > maxRegion)
        {
            maxRegion = uniqueRegions[j];
        }
    }

    double m = ((255.0-30)) / (maxRegion - minRegion);
    double b = m * -minRegion;
    
    int intensities[255];
    intensities[uniqueRegions[0]] = 0;
    for(int j = 1; j < numUniqueRegions; j++)
    {
        intensities[(int)round(m*uniqueRegions[j] + b)] = j * (double)(255-30)/(double)numUniqueRegions + 30;
    }
    for(int r = 0; r < size[1]; r++)
    {
        for(int c = 0; c < size[0]; c++)
        {
            proc_img[c][r] = intensities[(int)round(m*second_img[c][r] + b)];
        }
    }   
}


