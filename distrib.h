/*  distrib.h  ->  Distribution class

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

#include <stdio.h>

/*
	The Distribution class is used to model probability distributions.
	Say that the gender distribution of a population is 51% females 
	and 49% males, and it is desired to randomly assign genders to 
	some sample.  This can be done as follows:

	First, set up a double array with the distribution:

		double genderDistributionValues[2] = { 0.51, 0.49 };

	Each element of the array must be non-negative, and the sum of 
	the elements must be 1.0 (within an epsilon).

	Next, instantiate a Distribution object and initialize it:

		Distribution genderDistribution;

		genderDistribution.InitDistribution(
					genderDistributionValues, 2);

	Now, calls of 

		genderDistribution.RandomSlot() 

	will return the values 0 and 1 (there are only 2 slots) to indicate 
	female and male, respectively.  Furthermore, on average, 0 will be 
	returned 51% of the time, and 1 will be returned 49% of the time.

	As a second example, consider this age distribution:

		const int MaxLifeSpanInYears = 5;

		double ageDistributionValues[MaxLifeSpanInYears] =
		{
					// year range
			0.1,		//  [ 0,  1)
			0.2,		//  [ 1,  2)
			0.3,		//  [ 2,  3)
			0.3,		//  [ 3,  4)
			0.1,		//  [ 4,  5)
		};

	Instantiate a Distribution object to model this 

		Distribution ageDistribution;

		ageDistribution.InitDistribution (ageDistributionValues, 
							MaxLifeSpanInYears);

	Now, calls of 

		ageDistribution.RandomSlot() 

	will return 0, 1, 2, 3, or 4, selecting one of the age slots.  On 
	average, 0 is returned 10% of the time, 1 is returned 20% of the 
	time, etc.

	Note that when age distributions are used, the following convention
	is recommended:

		0 is the first year of life, i.e., [0, 1)
		1 is the second year of life, i.e., [1, 2)
		etc.

	Then the slot number is equal to the age in years truncated to an 
	integer.
*/


class Distribution
{
	int	numSlots;	// No. of slots in the distribution
	double  *pSlotArray;	// Ptr to array of numSlots elements

public:
	Distribution();
	bool InitDistribution(double *pNewArray, int newNumSlots);

virtual	~Distribution();

	int    NumberOfSlots();	// Return number of slots in the distribution
	double ProbabilityOfSlot(int slotNumber);
	int    RandomSlot();	// Return random slot from distribution

static	bool   CheckDistribution(double *pSlotArray, int numSlots);
static	double RandomNumberFromUnitInterval();

	void DumpDistribution(FILE *fp, char *tag = NULL);

};  // end class Distribution


//  end distrib.h
