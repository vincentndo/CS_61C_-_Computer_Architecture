// CS 61C Fall 2015 Project 4

// include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif

// include OpenMP
#if !defined(_MSC_VER)
#include <pthread.h>
#endif
#include <omp.h>

#include "calcDepthOptimized.h"
#include "calcDepthNaive.h"

/* DO NOT CHANGE ANYTHING ABOVE THIS LINE. */

float displacement(int dx, int dy)
	{
	float squaredDisplacement = dx * dx + dy * dy;
	
	return squaredDisplacement;
	}

void calcDepthOptimized(float *depth, float *left, float *right, int imageWidth, int imageHeight, int featureWidth, int featureHeight, int maximumDisplacement)
{
	// calcDepthNaive(depth, left, right, imageWidth, imageHeight, featureWidth, featureHeight, maximumDisplacement);
	
	/* The two outer for loops iterate through each pixel */
	
	#pragma omp parallel for
	for (int y = 0; y < imageHeight; y++)
	{		
		for (int x = 0; x < imageWidth; x++)
		{	
			/* Set the depth to 0 if looking at edge of the image where a feature box cannot fit. */
			if ((y < featureHeight) || (y >= imageHeight - featureHeight) || (x < featureWidth) || (x >= imageWidth - featureWidth))
			{
				depth[y * imageWidth + x] = 0;
				continue;
			}

			float minimumSquaredDifference = -1;
			int minimumDy = 0;
			int minimumDx = 0;

			/* Iterate through all feature boxes that fit inside the maximum displacement box. 
			   centered around the current pixel. */
			for (int dy = -maximumDisplacement; dy <= maximumDisplacement; dy++)
			{
				for (int dx = -maximumDisplacement; dx <= maximumDisplacement; dx++)
				{
					/* Skip feature boxes that dont fit in the displacement box. */
					if (y + dy - featureHeight < 0 || y + dy + featureHeight >= imageHeight || x + dx - featureWidth < 0 || x + dx + featureWidth >= imageWidth)
					{
						continue;
					}

					float squaredDifference = 0;

					/** ORIGINAL **/
					
					/* Sum the squared difference within a box of +/- featureHeight and +/- featureWidth. */
					// for (int boxY = -featureHeight; boxY <= featureHeight; boxY++)
					// {						
					// 	for (int boxX = -featureWidth; boxX <= featureWidth; boxX++)
					// 	{
					// 		int leftX = x + boxX;
					// 		int leftY = y + boxY;
					// 		int rightX = x + dx + boxX;
					// 		int rightY = y + dy + boxY;

					// 		float difference = left[leftY * imageWidth + leftX] - right[rightY * imageWidth + rightX];
					// 		squaredDifference += difference * difference;
					// 	}
					// }


					__m128 sum = _mm_setzero_ps();
					//__m128i t1,t2,t;
					for (int boxY = -featureHeight; boxY <= featureHeight; boxY+=1)
					{						
						for (int boxX = -featureWidth; boxX < (2*featureWidth)/4*4-featureWidth; boxX+=4)
						{
							int leftX = x + boxX;
							int leftY = y + boxY;
							int rightX = x + dx + boxX;
							int rightY = y + dy + boxY;

							__m128 difference = _mm_loadu_ps(& left[leftY * imageWidth + leftX]) - _mm_loadu_ps(& right[rightY * imageWidth + rightX]);
							sum = _mm_add_ps(sum,_mm_mul_ps(difference,difference));
						}
					}

					float temp[4] = {0,0,0,0};

					_mm_storeu_ps( temp, sum);

					for (int boxY = -featureHeight; boxY <= featureHeight; boxY++)
					{						
						for (int boxX = (2*featureWidth)/4*4-featureWidth; boxX <= featureWidth; boxX++)
						{
							int leftX11 = x + boxX;
							int leftY11 = y + boxY;
							int rightX11 = x + dx + boxX;
							int rightY11 = y + dy + boxY;

							float difference11 = left[leftY11 * imageWidth + leftX11] - right[rightY11 * imageWidth + rightX11];
							squaredDifference += difference11 * difference11;
						}
					}

					squaredDifference += temp[0] + temp[1] + temp[2] + temp[3];

					// for (int boxY = -featureHeight; boxY < (2*featureHeight)/4*4-featureHeight; boxY+=4)
					// {						
					// 	for (int boxX = -featureWidth; boxX < (2*featureWidth)/4*4-featureWidth; boxX+=4)
					// 	{
					// 		int leftX0 = x + boxX;
					// 		int leftY0 = y + boxY;
					// 		int rightX0 = x + dx + boxX;
					// 		int rightY0 = y + dy + boxY;

					// 		int leftX1 = x + boxX+1;
					// 		int leftY1 = y + boxY+1;
					// 		int rightX1 = x + dx + boxX+1;
					// 		int rightY1 = y + dy + boxY+1;

					// 		int leftX2 = x + boxX+2;
					// 		int leftY2 = y + boxY+2;
					// 		int rightX2 = x + dx + boxX+2;
					// 		int rightY2 = y + dy + boxY+2;

					// 		int leftX3 = x + boxX+3;
					// 		int leftY3 = y + boxY+3;
					// 		int rightX3 = x + dx + boxX+3;
					// 		int rightY3 = y + dy + boxY+3;

					// 		float difference0 = left[leftY0 * imageWidth + leftX0] - right[rightY0 * imageWidth + rightX0];
					// 		squaredDifference += difference0 * difference0;

					// 		float difference1 = left[leftY1 * imageWidth + leftX1] - right[rightY1 * imageWidth + rightX1];
					// 		squaredDifference += difference1 * difference1;

					// 		float difference2 = left[leftY2 * imageWidth + leftX2] - right[rightY2 * imageWidth + rightX2];
					// 		squaredDifference += difference2 * difference2;

					// 		float difference3 = left[leftY3 * imageWidth + leftX3] - right[rightY3 * imageWidth + rightX3];
					// 		squaredDifference += difference3 * difference3;
					// 	}
					// }

					// for (int boxY = (2*featureHeight)/4*4-featureHeight; boxY <= featureHeight; boxY++)
					// {						
					// 	for (int boxX = -featureWidth; boxX < (2*featureWidth)/4*4-featureWidth; boxX++)
					// 	{
					// 		int leftX11 = x + boxX;
					// 		int leftY11 = y + boxY;
					// 		int rightX11 = x + dx + boxX;
					// 		int rightY11 = y + dy + boxY;

					// 		float difference11 = left[leftY11 * imageWidth + leftX11] - right[rightY11 * imageWidth + rightX11];
					// 		squaredDifference += difference11 * difference11;
					// 	}
					// }

					// for (int boxY = -featureHeight; boxY < (2*featureHeight)/4*4-featureHeight; boxY++)
					// {						
					// 	for (int boxX = (2*featureWidth)/4*4-featureWidth; boxX <= featureWidth; boxX++)
					// 	{
					// 		int leftX22 = x + boxX;
					// 		int leftY22 = y + boxY;
					// 		int rightX22 = x + dx + boxX;
					// 		int rightY22 = y + dy + boxY;

					// 		float difference22 = left[leftY22 * imageWidth + leftX22] - right[rightY22 * imageWidth + rightX22];
					// 		squaredDifference += difference22 * difference22;
					// 	}
					// }

					// for (int boxY = (2*featureHeight)/4*4-featureHeight; boxY <= featureHeight; boxY++)
					// {						
					// 	for (int boxX = (2*featureWidth)/4*4-featureWidth; boxX <= featureWidth; boxX++)
					// 	{
					// 		int leftX33 = x + boxX;
					// 		int leftY33 = y + boxY;
					// 		int rightX33 = x + dx + boxX;
					// 		int rightY33 = y + dy + boxY;

					// 		float difference33 = left[leftY33 * imageWidth + leftX33] - right[rightY33 * imageWidth + rightX33];
					// 		squaredDifference += difference33 * difference33;
					// 	}
					// }


					/** UNROLL 4 TIMES **/

					// for (int boxY = -featureHeight; boxY < (2*featureHeight)/4*4-featureHeight; boxY+=4)
					// {						
					// 	for (int boxX = -featureWidth; boxX <= featureWidth; boxX+=1)
					// 	{
					// 		int leftX = x + boxX;
					// 		int leftY0 = y + boxY;
					// 		int rightX = x + dx + boxX;
					// 		int rightY0 = y + dy + boxY;

					// 		//int leftX1 = x + boxX+1;
					// 		int leftY1 = y + boxY+1;
					// 		//int rightX1 = x + dx + boxX+1;
					// 		int rightY1 = y + dy + boxY+1;

					// 		//int leftX2 = x + boxX+2;
					// 		int leftY2 = y + boxY+2;
					// 		//int rightX2 = x + dx + boxX+2;
					// 		int rightY2 = y + dy + boxY+2;

					// 		//int leftX3 = x + boxX+3;
					// 		int leftY3 = y + boxY+3;
					// 		//int rightX3 = x + dx + boxX+3;
					// 		int rightY3 = y + dy + boxY+3;

					// 		float difference0 = left[leftY0 * imageWidth + leftX] - right[rightY0 * imageWidth + rightX];
					// 		squaredDifference += difference0 * difference0;

					// 		float difference1 = left[leftY1 * imageWidth + leftX] - right[rightY1 * imageWidth + rightX];
					// 		squaredDifference += difference1 * difference1;

					// 		float difference2 = left[leftY2 * imageWidth + leftX] - right[rightY2 * imageWidth + rightX];
					// 		squaredDifference += difference2 * difference2;

					// 		float difference3 = left[leftY3 * imageWidth + leftX] - right[rightY3 * imageWidth + rightX];
					// 		squaredDifference += difference3 * difference3;
					// 	}
					// }

					// for (int boxY = (2*featureHeight)/4*4-featureHeight; boxY <= featureHeight; boxY++)
					// {						
					// 	for (int boxX = -featureWidth; boxX <= featureWidth; boxX++)
					// 	{
					// 		int leftX11 = x + boxX;
					// 		int leftY11 = y + boxY;
					// 		int rightX11 = x + dx + boxX;
					// 		int rightY11 = y + dy + boxY;

					// 		float difference11 = left[leftY11 * imageWidth + leftX11] - right[rightY11 * imageWidth + rightX11];
					// 		squaredDifference += difference11 * difference11;
					// 	}
					// }


					/** UNROLL 8 TIMES **/

					// for (int boxY = -featureHeight; boxY < (2*featureHeight)/8*8-featureHeight; boxY+=8)
					// {						
					// 	for (int boxX = -featureWidth; boxX <= featureWidth; boxX+=1)
					// 	{
					// 		int leftX = x + boxX;
					// 		int leftY0 = y + boxY;
					// 		int rightX = x + dx + boxX;
					// 		int rightY0 = y + dy + boxY;

					// 		//int leftX1 = x + boxX+1;
					// 		int leftY1 = y + boxY+1;
					// 		//int rightX1 = x + dx + boxX+1;
					// 		int rightY1 = y + dy + boxY+1;

					// 		//int leftX2 = x + boxX+2;
					// 		int leftY2 = y + boxY+2;
					// 		//int rightX2 = x + dx + boxX+2;
					// 		int rightY2 = y + dy + boxY+2;

					// 		//int leftX3 = x + boxX+3;
					// 		int leftY3 = y + boxY+3;
					// 		//int rightX3 = x + dx + boxX+3;
					// 		int rightY3 = y + dy + boxY+3;

					// 		//int leftX4 = x + boxX+4;
					// 		int leftY4 = y + boxY+4;
					// 		//int rightX4 = x + dx + boxX+4;
					// 		int rightY4 = y + dy + boxY+4;

					// 		//int leftX5 = x + boxX+5;
					// 		int leftY5 = y + boxY+5;
					// 		//int rightX5 = x + dx + boxX+5;
					// 		int rightY5 = y + dy + boxY+5;

					// 		//int leftX6 = x + boxX+6;
					// 		int leftY6 = y + boxY+6;
					// 		//int rightX6 = x + dx + boxX+6;
					// 		int rightY6 = y + dy + boxY+6;

					// 		//int leftX7 = x + boxX+7;
					// 		int leftY7 = y + boxY+7;
					// 		//int rightX7 = x + dx + boxX+7;
					// 		int rightY7 = y + dy + boxY+7;

					// 		float difference0 = left[leftY0 * imageWidth + leftX] - right[rightY0 * imageWidth + rightX];
					// 		squaredDifference += difference0 * difference0;

					// 		float difference1 = left[leftY1 * imageWidth + leftX] - right[rightY1 * imageWidth + rightX];
					// 		squaredDifference += difference1 * difference1;

					// 		float difference2 = left[leftY2 * imageWidth + leftX] - right[rightY2 * imageWidth + rightX];
					// 		squaredDifference += difference2 * difference2;

					// 		float difference3 = left[leftY3 * imageWidth + leftX] - right[rightY3 * imageWidth + rightX];
					// 		squaredDifference += difference3 * difference3;

					// 		float difference4 = left[leftY4 * imageWidth + leftX] - right[rightY4 * imageWidth + rightX];
					// 		squaredDifference += difference4 * difference4;

					// 		float difference5 = left[leftY5 * imageWidth + leftX] - right[rightY5 * imageWidth + rightX];
					// 		squaredDifference += difference5 * difference5;

					// 		float difference6 = left[leftY6 * imageWidth + leftX] - right[rightY6 * imageWidth + rightX];
					// 		squaredDifference += difference6 * difference6;

					// 		float difference7 = left[leftY7 * imageWidth + leftX] - right[rightY7 * imageWidth + rightX];
					// 		squaredDifference += difference7 * difference7;
					// 	}
					// }

					// for (int boxY = (2*featureHeight)/8*8-featureHeight; boxY <= featureHeight; boxY++)
					// {						
					// 	for (int boxX = -featureWidth; boxX <= featureWidth; boxX++)
					// 	{
					// 		int leftX11 = x + boxX;
					// 		int leftY11 = y + boxY;
					// 		int rightX11 = x + dx + boxX;
					// 		int rightY11 = y + dy + boxY;

					// 		float difference11 = left[leftY11 * imageWidth + leftX11] - right[rightY11 * imageWidth + rightX11];
					// 		squaredDifference += difference11 * difference11;
					// 	}
					// }


					/* 
					Check if you need to update minimum square difference. 
					This is when either it has not been set yet, the current
					squared displacement is equal to the min and but the new
					displacement is less, or the current squared difference
					is less than the min square difference.
					*/
					if ((minimumSquaredDifference == -1) || ((minimumSquaredDifference == squaredDifference) && (displacementNaive(dx, dy) < displacementNaive(minimumDx, minimumDy))) || (minimumSquaredDifference > squaredDifference))
					{
						minimumSquaredDifference = squaredDifference;
						minimumDx = dx;
						minimumDy = dy;
					}
				}
			}

			/* 
			Set the value in the depth map. 
			If max displacement is equal to 0, the depth value is just 0.
			*/
			if (minimumSquaredDifference != -1)
			{
				if (maximumDisplacement == 0)
				{
					depth[y * imageWidth + x] = 0;
				}
				else
				{
					depth[y * imageWidth + x] = displacementNaive(minimumDx, minimumDy);
				}
			}
			else
			{
				depth[y * imageWidth + x] = 0;
			}
		}
	}
}
