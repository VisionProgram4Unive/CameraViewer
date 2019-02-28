/*
 * =====================================================================================
 *
 *       Filename:  histogram_equalization.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2018년 03월 27일 02시 56분 59초
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Morning Willow (), axiuus@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "histogram_equalization.h"

int histogram_equalization(unsigned char * input,
							unsigned char * output,
							unsigned int width, unsigned int height)
{
	/*variables*/
	unsigned int histogram[256] = {0, };
	unsigned int equaled_histogram[256] = {0, };
	unsigned int sumofhisto = 0;
	const int numofpix = width * height;
	const double scale_factor = 255./(double)numofpix;
 
	/*get histogram*/
	for(int i=0;i<numofpix;i++){
		histogram[input[i]]++;
	} 

	/*CDF*/
	for(int i=0;i<256;i++){
		sumofhisto += histogram[i];
		equaled_histogram[i] = scale_factor*sumofhisto + 0.5;
	}

	/*write new image file*/
	for(int i=0;i<numofpix;i++)
		output[i] = equaled_histogram[input[i]];

	return 0;
}
