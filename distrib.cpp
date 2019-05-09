/*  distrib.cpp  ->  Distribution class

	Written by:	Roger House
	First version:	2003 Feb 19
*/

/* 
   This is part of parsim, a C++ program for simulating animal populations
   Coded by Roger House.

   Copyright (C) 2003-2007 by Roger House.
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   email: roger @ rogerfhouse.com (remove space)
*/


#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "distrib.h"
#include "random.h"


Distribution::Distribution()
{
	numSlots   = 0;
	pSlotArray = NULL;

}


bool Distribution::InitDistribution(double *pNewSlotArray, int newNumSlots)
/*
	Init a Distribution from an array:

	Copy the input array into our internal array, and make the 
	internal array cumulative.
*/
{
	// Make sure the input array is okay:

	if (!CheckDistribution(pNewSlotArray, newNumSlots))
		return false;

	// Set up data members:

	numSlots   = newNumSlots;
	delete [] pSlotArray;
	pSlotArray = new double [numSlots];

	// Copy the input array to pSlotArray, and make pSlotArray cumulative:

	for (int i = 0;  i < numSlots;  ++i)
	{
		pSlotArray[i] = pNewSlotArray[i];
		if (i > 0)
			pSlotArray[i] += pSlotArray[i-1];
	}

	return true;

}   // end Distribution ctor


Distribution::~Distribution()
{
	delete [] pSlotArray;
}


bool Distribution::CheckDistribution(double *pSlotArray, int numSlots)
/*
	Input:
		pSlotArray points to a distribution array of numSlots 
		elements.

	Action:
		It is verified that numSlots > 0 and that every element 
		of pSlotArray is non-negative, and that the sum of the 
		elements is 1.0.

	Output:
		A false return value means the array does not meet the 
		requirements stated above.

		A true return value means the array is okay.
*/
{
	if (numSlots <= 0)
		return false;

	double sum = 0.0;

	for (int i = 0;  i < numSlots;  ++i)
	{
		if (pSlotArray[i] < 0.0)
			return false;
		sum += pSlotArray[i];
	}

	if (fabs(sum - 1.0) >= 0.000001)	// Sum not 1 (within epsilon)
		return false;

	return true;

}   // end CheckDistribution


int Distribution::NumberOfSlots()
{
	return numSlots;
}


double Distribution::ProbabilityOfSlot(int slotNumber)
{
	assert(0 <= slotNumber && slotNumber < numSlots);

	return (slotNumber == 0 
		? pSlotArray[0] 
		: pSlotArray[slotNumber] - pSlotArray[slotNumber-1]);
}


double Distribution::RandomNumberFromUnitInterval()
{
	return genUniformRandomNumberInUnitInterval () ;
}


int Distribution::RandomSlot()	// Return random slot based on 
					//   distribution
/*
	Output:
		The return value is a slot number generated at random
		using the distribution.

	Notes:
		The returned slot n is an integer in the range [0, numSlots-1].

	THE ALGORITHM USED BELOW IS NOT IDEAL IN TERMS OF TIME.  IT WOULD
	BE NICE TO FIND A CONSTANT-TIME ALGORITHM RATHER THAN A LINEAR-TIME
	ONE.  BUT FOR NOW THIS WORKS FINE.  WE CAN OPTIMIZE LATER.
*/
{
	double	randValue = RandomNumberFromUnitInterval();

	for (int i = 0;  i < numSlots;  ++i)
	{
		if (randValue <= pSlotArray[i])
			return i;
	}

	// It should be impossible to reach this point, so fail:

	assert(false);

	return 0; 

}   // end RandomSlot


void Distribution::DumpDistribution(FILE *fp, char *tag)
{
	if (tag != NULL && *tag != 0)
		fprintf(fp, "%s:\n", tag);

	fprintf(fp, "  numSlots = %d\n", numSlots);
	fprintf(fp, "  pSlotArray =\n");

	for (int i = 0;  i < numSlots;  ++i)
		fprintf(fp, "    %2d:  %0.16g\n", i, pSlotArray[i]);

}   // end DumpDistribution


//  end distrib.cpp
