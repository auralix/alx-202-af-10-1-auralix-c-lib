/**
  ******************************************************************************
  * @file alxInterpLin.c
  * @brief Auralix C Library - ALX Linear Interpolation Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxInterpLin.h"


//******************************************************************************
// Constructor
//******************************************************************************
void AlxInterpLin_Ctor
(
	AlxInterpLin* me,
	float *xPointArr,		
	float *yPointArr,		
	uint32_t numOfArrPoints,
	bool isXpointArrRising
)
{
	// Ctor
	
	// Parameters
	me->xPointArr = xPointArr;
	me->yPointArr = yPointArr;
	me->numOfArrPoints = numOfArrPoints;
	me->isXpointArrRising = isXpointArrRising;
	
	// Variables
	
	// Info
	me->wasCtorCalled = true;

	// Check lenght and dataX numbers sequence 
	ALX_INTERP_LIN_ASSERT( me->numOfArrPoints > 1);
	
	if(me->isXpointArrRising) {
		for (uint32_t i = 0; i < numOfArrPoints - 1; i++) ALX_INTERP_LIN_ASSERT(*(me->xPointArr + i + 1) > *(me->xPointArr + i)); // Xn+1 > Xn - Every number in the array should be greater than the previus!
	}
	else
	{
		for (uint32_t i = 0; i < numOfArrPoints - 1; i++) ALX_INTERP_LIN_ASSERT(*(me->xPointArr + i + 1) < *(me->xPointArr + i));  // Xn+1 < Xn - Every number in the array should be lesser than the previus!
	}
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxInterpLin_GetY_WithStatus(AlxInterpLin* me, float x, float* yPtr)
{
	uint32_t index0 = 0; 
	uint32_t index1 = 0;
	float x0 = 0;
	float x1 = 0;
	float y0 = 0;
	float y1 = 0;

	if (me->isXpointArrRising)	// X array values are rising
	{
		if (x < *(me->xPointArr)) // If x is smaller than dataX[0], clip y at minimum dataY[0] and return AlxStatus 
		{
			*yPtr = *(me->yPointArr);  
			return Alx_ErrOutOfRangeMin;				// Out of Range Min
		}		 
		if (x > *(me->xPointArr + me->numOfArrPoints - 1)) // If x is bigger than dataX[last element], clip y at maximum dataY[last element]	
		{
			*yPtr = *(me->yPointArr + me->numOfArrPoints - 1);   	
			return Alx_ErrOutOfRangeMax;				// Out of Range Max
		}
		
		// Search for the indexes of the numbers surrounding inserted x  
		for(uint32_t i = 0 ; i < me->numOfArrPoints ; i++)
		{
			if (x < *(me->xPointArr + i)) // When this is true it  means that we found the first number that is larger than X (X1). Save and break from for loop
			{
				index1 = i;
				index0 = i - 1;
				break;
			}
		}
	}
	else						// X array values are falling
	{
		if (x < *(me->xPointArr + me->numOfArrPoints - 1)) // If x is smaller than dataX[last element], clip y at minimum dataY[last element]
		{
			*yPtr = *(me->yPointArr + me->numOfArrPoints - 1);	  	
			return Alx_ErrOutOfRangeMin;				// Out of Range Min
		}
		
		if (x > *(me->xPointArr)) // If x is larger than dataX[0], clip y at maximum dataY[0]
		{
			*yPtr =  *(me->yPointArr);  								
			return Alx_ErrOutOfRangeMax; 				// Out of Range Max
		}
		
		// Search for the indexes of the numbers surrounding inserted x 
		for (uint32_t i = 0; i < me->numOfArrPoints; i++)
		{
			if (x > *(me->xPointArr + i)) // When this is true it  means that we found the first number that is smaller than X (X1). Save and break from for loop
			{
				index1 = i;
				index0 = i - 1;
				break;
			}
		}
	}
	// Create points x0 and x1 from indexes
	x0 = *(me->xPointArr + index0);
	y0 = *(me->yPointArr + index0);
	x1 = *(me->xPointArr + index1);
	y1 = *(me->yPointArr + index1);
	
	// Linear Interpolation 
	*yPtr = y0 + (x - x0) * ((y1 - y0) / (x1 - x0));   // Where x0, x1 are nearest values of input x,  y0 y1 are nearest values to output y
	 
	return Alx_Ok;
}
float AlxInterpLin_GetY(AlxInterpLin* me, float x)
{
	float y;
	AlxInterpLin_GetY_WithStatus(me, x, &y);
	return y;
}