/*  parsim.cpp  ->  Simulating bird populations for Zach Peery
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
#include <stdio.h>
#include <search.h>
#include <assert.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <string>

#include "distrib.h"
#include "critter.h"
#include "random.h"

using namespace std;

/*
	WARNING:  This program evolved (or grew like a weed?) over a 
	period of years.  It is quite possible that some of the global 
	variables and even some of the functions found below are not 
	currently used.  Attempts were made to comment changes, but there 
	are no guarantees.  

	One major change is that a parameter file was added.  Thus many 
	of the global variables below are initialized to zero because 
	their values will be determined by the contents of the parameter 
	file.
*/

// We define a CritterMap where we keep all living critters:

CritterMap	TheCritterMap;

// We define a map where we keep our sample

CritterMap	SampleMap;

bool	postBreeding = true;		// Change this to false for prebreeding

int	numYearsToSimulate = 0;

// !!! Important note:  If the value of MaxNumYearsToSample is changed, then
// !!! parmTable must be changed so that it has exactly MaxNumYearsToSample
// !!! entries of the form numBirdsPerSampleXX.  See parmTable.

const int MinNumYearsToSample =  1;	// Lower bound on numYearsToSample parm
const int MaxNumYearsToSample = 25;	// Upperbound on numYearsToSample parm

int	numYearsToSample  = 0;

int	numBirdsPerSample[MaxNumYearsToSample] = { 0 };	// No. birds to sample each year

bool	stopSampling;			// Used when we can't get a full sample

int	initialPopulation = 0;		// We start with this many critters

int	minFinalPop = 0;		// Only experiments with final popula-
int	maxFinalPop = 0;		//   tions in this range are of interest

int	numExperimentsToReportOn = 0;	// Changed by command line parameter
int	maxExperimentsToRun      = 0;	// Never run more experiments than this

int	MaxLifeSpanInYears = 0;		// No critter lives longer than this


// !!! Important note:  If the value of MaxMaxLifeSpanInYears is changed, then
// !!! parmTable must be changed so that it has exactly MaxMaxLifeSpanInYears
// !!! entries of the form InitialPopAgeDistributionValuesXX.  See parmTable.

const int MinMaxLifeSpanInYears =  3;	// Lower bound on MaxLifeSpanInYears parm
const int MaxMaxLifeSpanInYears = 20;	// Upperbound on MaxLifeSpanInYears parm


// Here are the new parameters for introducing some randomness

bool	useRandomValues = true;		// Change this to false for determinsitic behavior

double	meanForMeanProbOfOneChild          = 0.0; // These two used to
double	stdDevForMeanProbOfOneChild        = 0.0; // compute mean below

double  meanForProbOfOneChild              = -1000.0;  // Computed dynamically
double	stdDevForProbOfOneChild            = 0.0;

double	meanForProbOfDyingFirstYear        = 0.0;
double	stdDevForProbOfDyingFirstYear      = 0.0;
double	meanForProbOfDyingAfterFirstYear   = 0.0;
double	stdDevForProbOfDyingAfterFirstYear = 0.0;

double	meanForProbOfImmFirstYear          = 0.0;
double	stdDevForProbOfImmFirstYear        = 0.0;
double	meanForProbOfImmAfterFirstYear     = 0.0;
double	stdDevForProbOfImmAfterFirstYear   = 0.0;

double	meanForProbOfEmFirstYear           = 0.0;
double	stdDevForProbOfEmFirstYear         = 0.0;
double	meanForProbOfEmAfterFirstYear      = 0.0;
double	stdDevForProbOfEmAfterFirstYear    = 0.0;

int	maxNumChildrenPerBirth             = 1 ;

double	meanForMeanForNumChildrenPerBirth   = 0.0;  // These two used to
double	stdDevForMeanForNumChildrenPerBirth = 0.0;  // compute mean below

double	meanForNumChildrenPerBirth         = -1000.0;  // Computed dynamically
double	stdDevForNumChildrenPerBirth       = 0.0;


// The distribution arrays must meet these requirements:  All elements
// are nonnegative, and the sum of the elements is 1.0.

double InitialPopAgeDistributionValues[MaxMaxLifeSpanInYears] =
{
			// Probability that age is in year range
        0.00,           //      [ 0, 1]
	0.00,           //      [ 1, 2]
	0.00,           //      [ 2, 3]
	0.00,           //      [ 3, 4]
	0.00,           //      [ 4, 5]
	0.00,           //      [ 5, 6]
	0.00,           //      [ 6, 7]
	0.00,           //      [ 7, 8]
	0.00,           //      [ 8, 9]
	0.00,           //      [ 9,10]
	0.00,           //      [10,11]
	0.00,           //      [11,12]
	0.00,           //      [12,13]
	0.00,           //      [13,14]
	0.00,           //      [14,15]
	0.00,           //      [15,16]
	0.00,           //      [16,17]
	0.00,           //      [17,18]
	0.00,           //      [18,19]
	0.00,           //      [19,20]
};


double InitialPopGenderDistributionValues[2] =
{
	0.0,	// female
	0.0,	// male
};


// The following gender distribution is used when new critters are born:

double NativeBornGenderDistributionValues[2] =
{
	0.0,	// female
	0.0,	// male
};


// Note that the following is NOT a distibution.  In particular, it is
// not required that the elements sum to 1.0.  How to use the array:
// Given a year y, the value ProbabilityOfDying[y] is the probability
// that a given critter dies at age y.

double ProbabilityOfDying[MaxMaxLifeSpanInYears] =
{
			// Probability that critter in age range dies
	0.00,           //      [ 0, 1]
	0.00,           //      [ 1, 2]
	0.00,           //      [ 2, 3]
	0.00,           //      [ 3, 4]
	0.00,           //      [ 4, 5]
	0.00,           //      [ 5, 6]
	0.00,           //      [ 6, 7]
	0.00,           //      [ 7, 8]
	0.00,           //      [ 8, 9]
	0.00,           //      [ 9,10]
	0.00,           //      [10,11]
	0.00,           //      [11,12]
	0.00,           //      [12,13]
	0.00,           //      [13,14]
	0.00,           //      [14,15]
	0.00,           //      [15,16]
	0.00,           //      [16,17]
	0.00,           //      [17,18]
	0.00,           //      [18,19]
	0.00,           //      [19,20]
};


int MarriageableAge = 0;	// A critter must have age >= MarriageableAge
				//   in order to marry

// Note that the following is NOT a distibution.  In particular, it is
// not required that the elements sum to 1.0.

double ImmigrationRateByAge[MaxMaxLifeSpanInYears] =
{
	0.0,           //      [ 0, 1]
	0.0,           //      [ 1, 2]
	0.0,           //      [ 2, 3]
	0.0,           //      [ 3, 4]
	0.0,           //      [ 4, 5]
	0.0,           //      [ 5, 6]
	0.0,           //      [ 6, 7]
	0.0,           //      [ 7, 8]
	0.0,           //      [ 8, 9]
	0.0,           //      [ 9,10]
	0.0,           //      [10,11]
	0.0,           //      [11,12]
	0.0,           //      [12,13]
	0.0,           //      [13,14]
	0.0,           //      [14,15]
	0.0,           //      [15,16]
	0.0,           //      [16,17]
	0.0,           //      [17,18]
	0.0,           //      [18,19]
	0.0,           //      [19,20]		// Not used (see comment in
						// AddCrittersWhichImmigrate
};


double ImmigrationGenderDistributionValues[2] =
{
	0.0,	// female
	0.0,	// male
};


// Note that the following is NOT a distibution.  In particular, it is
// not required that the elements sum to 1.0.  How to use the array:
// Given a year y, the value ProbabilityOfEmigratingAtAge[y] is the
// probability that a given critter emigrates at age y.

double ProbabilityOfEmigratingAtAge[MaxMaxLifeSpanInYears] =
{
			// Probability that critter in age range will emigrate
	0.0,           //      [ 0, 1]
	0.0,           //      [ 1, 2]
	0.0,           //      [ 2, 3]
	0.0,           //      [ 3, 4]
	0.0,           //      [ 4, 5]
	0.0,           //      [ 5, 6]
	0.0,           //      [ 6, 7]
	0.0,           //      [ 7, 8]
	0.0,           //      [ 8, 9]
	0.0,           //      [ 9,10]
	0.0,           //      [10,11]
	0.0,           //      [11,12]
	0.0,           //      [12,13]
	0.0,           //      [13,14]
	0.0,           //      [14,15]
	0.0,           //      [15,16]
	0.0,           //      [16,17]
	0.0,           //      [17,18]
	0.0,           //      [18,19]
	0.0,           //      [19,20]
};


// Note that the following is NOT a distibution.  In particular, it is
// not required that the elements sum to 1.0.  How to use the array:
// Given a gender and age, the probability of emigration =
// ProbabilityOfEmigratingForGender[g] TIMES ProbabilityOfEmigratingAtAge. Set
// ProbabilityOfEmigratingForGender eqaul to 1.0 to make the probability of
// emigration equal to the rows in the above array.

double ProbabilityOfEmigratingForGender[2] =
{
	0.0,	// female
	0.0,	// male
};


// Distribution objects for all the age and gender distributions:

Distribution InitialPopAgeDistribution;
Distribution InitialPopGenderDistribution;
Distribution NativeBornGenderDistribution;
Distribution ImmigrationGenderDistribution;


int	numExperimentsReported = 0;
bool	traceOutput = false;

int	experimentNumber;	// The current experiment
int	curYear;  		// The current year within the current experiment

int	numBornThisYear;
int	numDiedThisYear;
int	numImmigratedThisYear;
int	numEmigratedThisYear;
int	numMarriedThisYear;

// If k = 0, 1, ..., numSlotsInDiedArray-2, then numDiedByAge[k] is the
// number of critters of age k who died.  numDiedByAge[numSlotsInDiedArray-1]
// is the number of critters of age >= numSlotsInDiedArray-1 who died.

const int numSlotsInDiedArray = 4;
int	numDiedByAge[numSlotsInDiedArray];

int	originalNumBirdsByAgeInYears[MaxMaxLifeSpanInYears];


// Parameter file stuff:

char *parmFileName = NULL;
FILE *parmfp = NULL;

#define MAX_LINE_LEN 1001

char parmLine[MAX_LINE_LEN];

int parmLineNum = 0 ;

struct Parm
{
	string  name;           // name of the parameter
	bool    integerType;    // false means the type is double
	union	{ int *pInt;  double *pDouble; };  // ptr to parm
	union	{ int lbInt;  double lbDouble; };  // lower bound
	union	{ int ubInt;  double ubDouble; };  // upper bound

	// a parm value must fall in the range [lower bound, upper bound]

	Parm (  int lb, int ub, const string &nameX, int *pIntX )
	{
		name        = nameX ;
		integerType = true;
		pInt        = pIntX;
		lbInt       = lb;
		ubInt       = ub;
	}

	Parm ( double lb, double ub, const string &nameX, double *pDoubleX )
	{
		name        = nameX ;
		integerType = false;
		pDouble     = pDoubleX;
		lbDouble    = lb;
		ubDouble    = ub;
	}
};

/*
	Note that for some of the table entries below, the min and max
	allowed values are actually set up dynamically.  See the function
	LoadParmFile below.
*/

Parm parmTable[] =
{
  Parm(  1,   100, "numYearsToSimulate=",                &numYearsToSimulate      ),
  Parm(  1,    25, "numYearsToSample=",                  &numYearsToSample        ),

  Parm(  1, 10000, "numBirdsPerSample01=",               &numBirdsPerSample[ 0]),
  Parm(  1, 10000, "numBirdsPerSample02=",               &numBirdsPerSample[ 1]),
  Parm(  1, 10000, "numBirdsPerSample03=",               &numBirdsPerSample[ 2]),
  Parm(  1, 10000, "numBirdsPerSample04=",               &numBirdsPerSample[ 3]),
  Parm(  1, 10000, "numBirdsPerSample05=",               &numBirdsPerSample[ 4]),
  Parm(  1, 10000, "numBirdsPerSample06=",               &numBirdsPerSample[ 5]),
  Parm(  1, 10000, "numBirdsPerSample07=",               &numBirdsPerSample[ 6]),
  Parm(  1, 10000, "numBirdsPerSample08=",               &numBirdsPerSample[ 7]),
  Parm(  1, 10000, "numBirdsPerSample09=",               &numBirdsPerSample[ 8]),
  Parm(  1, 10000, "numBirdsPerSample10=",               &numBirdsPerSample[ 9]),
  Parm(  1, 10000, "numBirdsPerSample11=",               &numBirdsPerSample[10]),
  Parm(  1, 10000, "numBirdsPerSample12=",               &numBirdsPerSample[11]),
  Parm(  1, 10000, "numBirdsPerSample13=",               &numBirdsPerSample[12]),
  Parm(  1, 10000, "numBirdsPerSample14=",               &numBirdsPerSample[13]),
  Parm(  1, 10000, "numBirdsPerSample15=",               &numBirdsPerSample[14]),
  Parm(  1, 10000, "numBirdsPerSample16=",               &numBirdsPerSample[15]),
  Parm(  1, 10000, "numBirdsPerSample17=",               &numBirdsPerSample[16]),
  Parm(  1, 10000, "numBirdsPerSample18=",               &numBirdsPerSample[17]),
  Parm(  1, 10000, "numBirdsPerSample19=",               &numBirdsPerSample[18]),
  Parm(  1, 10000, "numBirdsPerSample20=",               &numBirdsPerSample[19]),
  Parm(  1, 10000, "numBirdsPerSample21=",               &numBirdsPerSample[20]),
  Parm(  1, 10000, "numBirdsPerSample22=",               &numBirdsPerSample[21]),
  Parm(  1, 10000, "numBirdsPerSample23=",               &numBirdsPerSample[22]),
  Parm(  1, 10000, "numBirdsPerSample24=",               &numBirdsPerSample[23]),
  Parm(  1, 10000, "numBirdsPerSample25=",               &numBirdsPerSample[24]),

  Parm(  1, 10000, "initialPopulation=",                 &initialPopulation       ),
  Parm(  1, 10000, "minFinalPop=",                       &minFinalPop             ),
  Parm(  1, 20000, "maxFinalPop=",                       &maxFinalPop             ),
  Parm(  1, 10000, "numExperimentsToReportOn=",          &numExperimentsToReportOn),
  Parm(  1, 50000, "maxExperimentsToRun=",               &maxExperimentsToRun     ), 
  Parm(  1,   100, "MarriageableAge=",                   &MarriageableAge         ),
  Parm(  1,   100, "MaxLifeSpanInYears=",                &MaxLifeSpanInYears      ),

  Parm(0.0,   1.0, "InitialPopAgeDistributionValues01=", &InitialPopAgeDistributionValues[ 0]),
  Parm(0.0,   1.0, "InitialPopAgeDistributionValues02=", &InitialPopAgeDistributionValues[ 1]),
  Parm(0.0,   1.0, "InitialPopAgeDistributionValues03=", &InitialPopAgeDistributionValues[ 2]),
  Parm(0.0,   1.0, "InitialPopAgeDistributionValues04=", &InitialPopAgeDistributionValues[ 3]),
  Parm(0.0,   1.0, "InitialPopAgeDistributionValues05=", &InitialPopAgeDistributionValues[ 4]),
  Parm(0.0,   1.0, "InitialPopAgeDistributionValues06=", &InitialPopAgeDistributionValues[ 5]),
  Parm(0.0,   1.0, "InitialPopAgeDistributionValues07=", &InitialPopAgeDistributionValues[ 6]),
  Parm(0.0,   1.0, "InitialPopAgeDistributionValues08=", &InitialPopAgeDistributionValues[ 7]),
  Parm(0.0,   1.0, "InitialPopAgeDistributionValues09=", &InitialPopAgeDistributionValues[ 8]),
  Parm(0.0,   1.0, "InitialPopAgeDistributionValues10=", &InitialPopAgeDistributionValues[ 9]),
  Parm(0.0,   1.0, "InitialPopAgeDistributionValues11=", &InitialPopAgeDistributionValues[10]),
  Parm(0.0,   1.0, "InitialPopAgeDistributionValues12=", &InitialPopAgeDistributionValues[11]),
  Parm(0.0,   1.0, "InitialPopAgeDistributionValues13=", &InitialPopAgeDistributionValues[12]),
  Parm(0.0,   1.0, "InitialPopAgeDistributionValues14=", &InitialPopAgeDistributionValues[13]),
  Parm(0.0,   1.0, "InitialPopAgeDistributionValues15=", &InitialPopAgeDistributionValues[14]),
  Parm(0.0,   1.0, "InitialPopAgeDistributionValues16=", &InitialPopAgeDistributionValues[15]),
  Parm(0.0,   1.0, "InitialPopAgeDistributionValues17=", &InitialPopAgeDistributionValues[16]),
  Parm(0.0,   1.0, "InitialPopAgeDistributionValues18=", &InitialPopAgeDistributionValues[17]),
  Parm(0.0,   1.0, "InitialPopAgeDistributionValues19=", &InitialPopAgeDistributionValues[18]),
  Parm(0.0,   1.0, "InitialPopAgeDistributionValues20=", &InitialPopAgeDistributionValues[19]),

  Parm(  1,    20, "maxNumChildrenPerBirth=",            &maxNumChildrenPerBirth             ),

  Parm(0.0,   1.0, "meanForMeanProbOfOneChild=",         &meanForMeanProbOfOneChild          ),
  Parm(0.0,   1.0, "stdDevForMeanProbOfOneChild=",       &stdDevForMeanProbOfOneChild        ),
  Parm(0.0,   1.0, "stdDevForProbOfOneChild=",           &stdDevForProbOfOneChild            ),

  Parm(0.0,  20.0, "meanForMeanForNumChildrenPerBirth=", &meanForMeanForNumChildrenPerBirth  ),
  Parm(0.0,  20.0, "stdDevForMeanForNumChildrenPerBirth=",&stdDevForMeanForNumChildrenPerBirth),
  Parm(0.0,  20.0, "stdDevForNumChildrenPerBirth=",      &stdDevForNumChildrenPerBirth       ),

  Parm(0.0,   1.0, "meanForProbOfDyingFirstYear=",       &meanForProbOfDyingFirstYear        ),
  Parm(0.0,   1.0, "stdDevForProbOfDyingFirstYear=",     &stdDevForProbOfDyingFirstYear      ),
  Parm(0.0,   1.0, "meanForProbOfDyingAfterFirstYear=",  &meanForProbOfDyingAfterFirstYear   ),
  Parm(0.0,   1.0, "stdDevForProbOfDyingAfterFirstYear=",&stdDevForProbOfDyingAfterFirstYear ),

  Parm(0.0,   1.0, "meanForProbOfImmFirstYear=",         &meanForProbOfImmFirstYear          ),
  Parm(0.0,   1.0, "stdDevForProbOfImmFirstYear=",       &stdDevForProbOfImmFirstYear        ),
  Parm(0.0,   1.0, "meanForProbOfImmAfterFirstYear=",    &meanForProbOfImmAfterFirstYear     ),
  Parm(0.0,   1.0, "stdDevForProbOfImmAfterFirstYear=",  &stdDevForProbOfImmAfterFirstYear   ),

  Parm(0.0,   1.0, "meanForProbOfEmFirstYear=",          &meanForProbOfEmFirstYear           ),
  Parm(0.0,   1.0, "stdDevForProbOfEmFirstYear=",        &stdDevForProbOfEmFirstYear         ),
  Parm(0.0,   1.0, "meanForProbOfEmAfterFirstYear=",     &meanForProbOfEmAfterFirstYear      ),
  Parm(0.0,   1.0, "stdDevForProbOfEmAfterFirstYear=",   &stdDevForProbOfEmAfterFirstYear    ),

  Parm(0.0,   1.0, "InitialPopGenderDistributionValuesMale=",    &InitialPopGenderDistributionValues [0] ),
  Parm(0.0,   1.0, "InitialPopGenderDistributionValuesFemale=",  &InitialPopGenderDistributionValues [1] ),
  Parm(0.0,   1.0, "NativeBornGenderDistributionValuesMale=",    &NativeBornGenderDistributionValues [0] ),
  Parm(0.0,   1.0, "NativeBornGenderDistributionValuesFemale=",  &NativeBornGenderDistributionValues [1] ),
  Parm(0.0,   1.0, "ImmigrationGenderDistributionValuesMale=",   &ImmigrationGenderDistributionValues[0] ),
  Parm(0.0,   1.0, "ImmigrationGenderDistributionValuesFemale=", &ImmigrationGenderDistributionValues[1] ),
  Parm(0.0,   1.0, "ProbabilityOfEmigratingForGenderMale=",      &ProbabilityOfEmigratingForGender   [0] ),
  Parm(0.0,   1.0, "ProbabilityOfEmigratingForGenderFemale=",    &ProbabilityOfEmigratingForGender   [1] ),
};
#define PARM_TABLE_SIZE (sizeof(parmTable) / sizeof(parmTable[0]))


// Function prototypes:

void	AddCrittersToSample(void);
void	AddCrittersWhichAreBorn(void);
void	AddCrittersWhichImmigrate(void);
void	AnalyzeFinalPopulation(void);
void	ComputeRandomTableStuff
	(
		double *table,
		int    n,
		double mean0,
		double stddev0,
		double meanRest,
		double stddevRest
	);
void	ComputeRandomStuff(void);
void	CountBirdsByAge(int *countByYear);
Critter	*CreateNativeBornCritter(int momId, int popId);
void	DumpAllCritters(FILE *fp, const string &tag);
void	DisplayUsage(void);
CritterMap::iterator FindUnmarriedCritter(
		Gender               genderWanted, 
		CritterMap::iterator whereToStart);
double	genNormalRandomProbability ( double mean, double stddev ) ;
Critter	*GetCritter(int critterId);
bool	getLine(void);
void	GetRidOfAllCritters(void);
Critter	*GetSampleCritter(int critterId);
string	LoadParmFile(void);
void	MakeEachCritterOneYearOlder(void);
void	MarryOffSingleCritters(void);
Critter	*MustGetCritter(int critterId);
Gender	OppositeGender(Gender inGender);
void	ParseCommandLine(int argc, char **argv);
Gender	RandomGender(Distribution &distribution);
void	RemoveCrittersWhichDie(void);
void	RemoveCrittersWhichEmigrate(void);
void	RemoveReallyOldCritters(void);
void	SetUpInitialPopulation(void);
void	SimulateBirdPopulation(void);
bool	VerifyCritters(FILE *fp, const string &tag);


int main(int argc, char **argv)
{
	printf("Begin parsim\n");
	printf("\n");

	ParseCommandLine(argc, argv);

	if (!InitialPopAgeDistribution.InitDistribution(
			InitialPopAgeDistributionValues, MaxLifeSpanInYears))
	{
		printf("\n***ERROR in parm file:  "
			"InitialPopAgeDistributionValues do not form a "
			"valid distribution\n");
		exit(1);
	}

	if (!InitialPopGenderDistribution.InitDistribution(
			InitialPopGenderDistributionValues, 2))
	{
		printf("\n***ERROR in parm file:  "
			"InitialPopGenderDistributionValues do not form a "
			"valid distribution\n");
		exit(1);
	}

	if (!NativeBornGenderDistribution.InitDistribution(
			NativeBornGenderDistributionValues, 2))
	{
		printf("\n***ERROR in parm file:  "
			"NativeBornGenderDistributionValues do not form a "
			"valid distribution\n");
		exit(1);
	}

	if (!ImmigrationGenderDistribution.InitDistribution(
			ImmigrationGenderDistributionValues, 2))
	{
		printf("\n***ERROR in parm file:  "
			"ImmigrationGenderDistributionValues do not form a "
			"valid distribution\n");
		exit(1);
	}


	printf("\n");
	printf("%s for %d experiments (to report on), each for %d years, starting with %d "
		"birds each time\n", 
			postBreeding ? "Post-breeding" : "Pre-breeding",
			numExperimentsToReportOn, numYearsToSimulate, 
			initialPopulation);
	printf("\n");

	for (experimentNumber = 1;  experimentNumber <= maxExperimentsToRun;  
							++experimentNumber)
	{
		Critter::lastAssignedCritterId = 0;     // Reset id's
		ComputeRandomStuff();
		SimulateBirdPopulation();

		if ( numExperimentsReported >= numExperimentsToReportOn )
			break;
	}

	if ( experimentNumber > maxExperimentsToRun )
		experimentNumber = maxExperimentsToRun;

	if (!traceOutput)
		printf("\n");
	printf("%s for %d experiments (%d reported), each for %d years, starting with %d "
		"birds each time\n", 
			postBreeding ? "Post-breeding" : "Pre-breeding", 
			experimentNumber, numExperimentsReported, numYearsToSimulate, 
			initialPopulation);
	if ( numExperimentsReported < numExperimentsToReportOn )
		printf("NOTE:  Fewer experiments reported on (%d) than requested (%d)\n", 
			numExperimentsReported, numExperimentsToReportOn);

	printf("\n");
	printf("End parsim\n");

	return 0;

}   // end main


void SimulateBirdPopulation(void)
{
	SetUpInitialPopulation();

	if (traceOutput)
	{
		printf("Experiment %3d:  Number of birds at time 0:  %d\n", 
				experimentNumber, (int)TheCritterMap.size());
		printf("\n");
		if (postBreeding)
			printf("  year    die    emi    imm    wed   born"
				"  total    died at age:    0     1     2"
				"   >=3\n");
		else
			printf("  year   born    die    emi    imm    wed"
				"  total    died at age:    0     1     2"
				"   >=3\n");
	}

	stopSampling = false;

	for (curYear = 1;  curYear <= numYearsToSimulate;  ++curYear)
	{
		VerifyCritters(stdout, "Top of year loop");
//		int curPopulation = TheCritterMap.size();

		CountBirdsByAge(originalNumBirdsByAgeInYears);

		if (postBreeding)
		{
			RemoveCrittersWhichEmigrate(); VerifyCritters(stdout, "After RemoveCrittersWhichEmigrate");
			RemoveCrittersWhichDie();      VerifyCritters(stdout, "After RemoveCrittersWhichDie");     
			MakeEachCritterOneYearOlder(); // Some birds may be 1 year too old (but not for long)
			RemoveReallyOldCritters();     VerifyCritters(stdout, "After RemoveReallyOldCritters");    
			AddCrittersWhichImmigrate();   VerifyCritters(stdout, "After AddCrittersWhichImmigrate");  
			MarryOffSingleCritters();      VerifyCritters(stdout, "After MarryOffSingleCritters");     
			AddCrittersWhichAreBorn();     VerifyCritters(stdout, "After AddCrittersWhichAreBorn");    
			AddCrittersToSample();         VerifyCritters(stdout, "After AddCrittersToSample");        
		}
		else	// Pre-breeding:
		{
			AddCrittersWhichAreBorn();
			RemoveCrittersWhichEmigrate();
			RemoveCrittersWhichDie();
			MakeEachCritterOneYearOlder();
			RemoveReallyOldCritters();
			AddCrittersWhichImmigrate();
			MarryOffSingleCritters();
			AddCrittersToSample();
		}

		if (traceOutput)
		{
			if (postBreeding)
				printf("  %4d:  %4d   %4d   %4d   %4d   %4d"
				"   %4d", curYear, numDiedThisYear, 
				numEmigratedThisYear, numImmigratedThisYear,
				numMarriedThisYear, numBornThisYear, 
				(int)TheCritterMap.size());
			else
				printf("  %4d:  %4d   %4d   %4d   %4d   %4d"
				"   %4d", curYear, numBornThisYear, 
				numDiedThisYear, numEmigratedThisYear, 
				numImmigratedThisYear, numMarriedThisYear, 
				(int)TheCritterMap.size());

			printf("               ");
			for (int k = 0;  k < numSlotsInDiedArray;  ++k)
				printf("  %4d", numDiedByAge[k]);
			printf("\n");

			fflush(stdout);
		}

		if (TheCritterMap.size() == 0) 
			break;

	}   // end loop over years

	if (TheCritterMap.size() == 0)
		printf("Experiment %3d:  Entire population died out in "
			"year %3d\n", experimentNumber, curYear);
	else
		AnalyzeFinalPopulation();

	GetRidOfAllCritters();

}   // end SimulateBirdPopulation

/*
pre:
  year   born    die    emi    imm    wed  total    died at age:    0     1     2   >=3
  xxxx:  xxxx   xxxx   xxxx   xxxx   xxxx   xxxx                 xxxx  xxxx  xxxx  xxxx
post:
  year    die    emi    imm    wed   born  total    died at age:    0     1     2   >=3
  xxxx:  xxxx   xxxx   xxxx   xxxx   xxxx   xxxx                 xxxx  xxxx  xxxx  xxxx
*/

void SetUpInitialPopulation(void)
/*
	Note that age and gender are set up independently of one another.
	For maximum flexibility, we should have a gender distribution for 
	each possible age.  
*/
{
	GetRidOfAllCritters();

	for (int i = 1;  i <= initialPopulation;  ++i)
	{
		// Instantiate a new critter:

		Critter *pCritter = new Critter();

		// Set up data members of the new critter - all data members 
		// not set up below (except for id) are zero:

		pCritter->age = InitialPopAgeDistribution.RandomSlot();
		pCritter->yearBorn = - pCritter->age;
		pCritter->origin = kPartOfInitialPopulation;
		pCritter->gender = RandomGender(InitialPopGenderDistribution);
		pCritter->probOfOneChild = genNormalRandomProbability ( 
						meanForProbOfOneChild, 
						stdDevForProbOfOneChild ) ;

		// Insert the new critter into the critter map:

	        TheCritterMap.insert(make_pair(pCritter->id, pCritter));
	}

	MarryOffSingleCritters();

	// ???  Should some percentage of the new critters have 
	// ???  living mothers and fathers?  Be careful:  Make sure the 
	// ???  difference in age between children and parents is reasonable

}   // end SetUpInitialPopulation


void MakeEachCritterOneYearOlder(void)
{
	Critter              *pCritter;
	CritterMap::iterator critItr;

	// Iterate over the map and bump the age of each critter by 1 year

	for (critItr = TheCritterMap.begin();  
				critItr != TheCritterMap.end();  ++critItr)
	{
		pCritter = critItr->second;
        	++pCritter->age;
	}

}   // end MakeEachCritterOneYearOlder


void RemoveCrittersWhichDie(void)
{
	Critter              *pCritter;
	CritterMap::iterator critItr;

	int currentNumBirdsByAgeInYears[MaxMaxLifeSpanInYears];
	CountBirdsByAge(currentNumBirdsByAgeInYears);

	double probabilityOfDeathByAge[MaxMaxLifeSpanInYears];

	for (int i = 0;  i < MaxLifeSpanInYears;  ++i)
	{
		int curNum = currentNumBirdsByAgeInYears[i];
		if (curNum == 0)
		{
			probabilityOfDeathByAge[i] = 0;
			continue;
		}

		int numToDie = (int) (ProbabilityOfDying[i] *
			originalNumBirdsByAgeInYears[i] + 0.5);

		if (numToDie >= curNum)
			probabilityOfDeathByAge[i] = 1;
		else
			probabilityOfDeathByAge[i] = 
					((double)(numToDie))/curNum;
	}

	// Iterate over the map and remove all critters which die  --  Note
	// that we are careful NOT to increment an iterator after the critter 
	// to which it points has been removed from the map; such an iterator
	// can no longer be operated on.  This is why there is no ++critItr
	// at the end of the for-loop header.

	numDiedThisYear = 0;
	int k;

	for ( k = 0;  k < numSlotsInDiedArray;  ++k)
		numDiedByAge[k] = 0;

	for (critItr = TheCritterMap.begin();  
				critItr != TheCritterMap.end(); )
	{
		pCritter = critItr->second;

		int age = pCritter->age;

		CritterMap::iterator saveItr = critItr;

		++critItr;	// Works okay because no erasing done yet

        	if (age >= MaxLifeSpanInYears || 
				Distribution::RandomNumberFromUnitInterval()
				<= probabilityOfDeathByAge[age])
		{
			if (pCritter->mateId != 0)
			{
				// pCritter points at a married critter, so
				// we are about to create a widow or a widower
				Critter *pMate = 
					MustGetCritter(pCritter->mateId);
				pMate->mateId = 0;
			}
			TheCritterMap.erase(saveItr);	// Remove ptr from map
			delete pCritter;		// Delete the critter
			++numDiedThisYear;
			if (age > numSlotsInDiedArray-1)
				age = numSlotsInDiedArray-1;
			++numDiedByAge[age];
		}
	}

	int sumOfDied = 0;
	for (k = 0;  k < numSlotsInDiedArray;  ++k)
		sumOfDied += numDiedByAge[k];

	assert(sumOfDied == numDiedThisYear);

}   // end RemoveCrittersWhichDie


void RemoveReallyOldCritters(void)
{
	Critter              *pCritter;
	CritterMap::iterator critItr;

	// Iterate over the map and remove all critters which are too old:

	for (critItr = TheCritterMap.begin();  
				critItr != TheCritterMap.end(); )
	{
		pCritter = critItr->second;

		int age = pCritter->age;

		CritterMap::iterator saveItr = critItr;

		++critItr;	// Works okay because no erasing done yet

        	if (age >= MaxLifeSpanInYears)
		{
			if (pCritter->mateId != 0)
			{
				// pCritter points at a married critter, so
				// we are about to create a widow or a widower
				Critter *pMate =
					MustGetCritter(pCritter->mateId);
				pMate->mateId = 0;
			}
			TheCritterMap.erase(saveItr);	// Remove ptr from map
			delete pCritter;		// Delete the critter
			++numDiedThisYear;
			if (age > numSlotsInDiedArray-1)
				age = numSlotsInDiedArray-1;
			++numDiedByAge[age];
		}
	}

	int sumOfDied = 0;
	for (int k = 0;  k < numSlotsInDiedArray;  ++k)
		sumOfDied += numDiedByAge[k];

	assert(sumOfDied == numDiedThisYear);

}   // end RemoveReallyOldCritters


void RemoveCrittersWhichEmigrate()
{
	Critter              *pCritter;
	CritterMap::iterator critItr;

	// Iterate over the map and remove all critters which emigrate

	numEmigratedThisYear = 0;

	for (critItr = TheCritterMap.begin();  TheCritterMap.size() > 0 && 
					critItr != TheCritterMap.end(); )
	{
		pCritter = critItr->second;

		CritterMap::iterator saveItr = critItr;

		++critItr;	// Works okay because no erasing done yet

		double probabilityOfEmigrating = 
			ProbabilityOfEmigratingAtAge[pCritter->age] *
			ProbabilityOfEmigratingForGender[pCritter->gender-1];

		if (Distribution::RandomNumberFromUnitInterval() 
				<= probabilityOfEmigrating)
		{
			if (pCritter->mateId != 0)
			{
				// pCritter points at a married critter, so
				// we are about to create a widow or a widower
				Critter *pMate = 
					MustGetCritter(pCritter->mateId);
				pMate->mateId = 0;
			}
			TheCritterMap.erase(saveItr);	// Remove ptr from map
			delete pCritter;		// Delete the critter
			++numEmigratedThisYear;
		}

	}   // end loop over critters

}   // end RemoveCrittersWhichEmigrate


void AddCrittersWhichImmigrate()
{
	// Note that we only iterate to MaxLifeSpanInYears-1, not to
	// MaxLifeSpanInYears because the age of a newly-immigrated bird
	// is j+1.  We make sure that the new ages are all less than
	// MaxLifeSpanInYears

	for (int j = 0;  j < MaxLifeSpanInYears-1;  ++j)
	{
		int numBirdsToImmigrate = (int) (ImmigrationRateByAge[j] *
			originalNumBirdsByAgeInYears[j] + 0.5);

		for (int i = 1;  i <= numBirdsToImmigrate;  ++i)
		{
			// Instantiate a new critter:

			Critter *pCritter = new Critter();

			// Set up data members of the new critter - all data 
			// members not set up below (except for id) are zero:

			pCritter->age = j+1;
			pCritter->yearBorn = curYear - pCritter->age;
			pCritter->origin = kImmigrant;
			pCritter->gender = RandomGender(
						ImmigrationGenderDistribution);
			pCritter->probOfOneChild = 
				genNormalRandomProbability ( 
					meanForProbOfOneChild, 
					stdDevForProbOfOneChild ) ;

			// Insert the new critter into the critter map:

		        TheCritterMap.insert(make_pair(pCritter->id, 
								pCritter));
		}
	}

}   // end AddCrittersWhichImmigrate


void MarryOffSingleCritters(void)
/*
	The algorithm below is not the most efficient possible.  For speed,
	we should maintain a collection of unmarried males and a collection 
	of unmarried females.  However, for simplicity, the approach below
	works pretty well.
*/
{
	Critter              *pUnmarried1,  *pUnmarried2;
	CritterMap::iterator unmarriedPos1, unmarriedPos2;
	Gender               gender1,       gender2;

	numMarriedThisYear = 0;

	unmarriedPos1 = TheCritterMap.begin();

	for (;;)    // loop forever (exit via return's in loop body)
	{	
		unmarriedPos1 = FindUnmarriedCritter(kNoGender, unmarriedPos1);
		if (unmarriedPos1 == TheCritterMap.end())
			return;		// All critters married

		pUnmarried1 = unmarriedPos1->second;
		gender1     = pUnmarried1->gender;
		gender2     = OppositeGender(gender1);

//pUnmarried1->DumpCritter(stdout, "First unmarried critter");

		unmarriedPos2 = FindUnmarriedCritter(gender2, unmarriedPos1);
		if (unmarriedPos2 == TheCritterMap.end())
			return;		// No mate for pUnmarried1

		pUnmarried2 = unmarriedPos2->second;

//pUnmarried2->DumpCritter(stdout, "Second unmarried critter");

		// Marry 'em:
		pUnmarried1->mateId = pUnmarried2->id;
		pUnmarried2->mateId = pUnmarried1->id;
		++numMarriedThisYear;
	}

}   // end MarryOffSingleCritters


CritterMap::iterator FindUnmarriedCritter(
    Gender               genderWanted,
    CritterMap::iterator whereToStart)
{
	Critter              *pCritter;
	CritterMap::iterator pos;

	for (pos = whereToStart;  pos != TheCritterMap.end();  ++pos)
	{
		pCritter = pos->second;
		if (pCritter->mateId == 0 && pCritter->age >= MarriageableAge 
				&& (genderWanted == kNoGender || 
				    genderWanted == pCritter->gender))
			return pos;
	}

	return TheCritterMap.end();	// We didn't find an unmarried critter

}   // end FindUnmarriedCritter


void AddCrittersWhichAreBorn(void)
/*
	Note that we do not change TheCritterMap as it is scanned.  Instead,
	new children are stored in the ChildMap.  After all children have 
	been born, the ChildMap is inserted into TheCritterMap.
*/
{
	Critter              *pCritter;
	CritterMap::iterator pos;
	CritterMap           ChildMap;

	// Iterate over the map and create children for married couples.
	// Note that we encounter both critters of a couple during the 
	// scan, but we only want them to mate once, so we mate them 
	// when the female is encountered

	numBornThisYear = 0;

	for (pos = TheCritterMap.begin();  
				pos != TheCritterMap.end();  ++pos)
	{
		Critter *pMom = pos->second;

	       	if (pMom->mateId == 0)
			continue;		// Critter is not married

	       	if (pMom->gender != kFemale)
			continue;		// Critter is not female

		Critter *pPop = MustGetCritter(pMom->mateId);

		int numChildrenBorn = 0 ;

		if (maxNumChildrenPerBirth == 1)
		{
// 2006 Aug 17  RFH:  We no longer use the probOfOneChild member of Critter
//			double probOfOneChild = 
//				(pMom->probOfOneChild + 
//					pPop->probOfOneChild) / 2 ;

			double probOfOneChild = meanForProbOfOneChild ;

			if (Distribution::RandomNumberFromUnitInterval() <= 
								probOfOneChild)
				numChildrenBorn = 1 ;
		}
		else
		{
			double x = genNormalRandomNumber ( 
					meanForNumChildrenPerBirth , 
					stdDevForNumChildrenPerBirth ) ;
			if ( x < 0 )
				x = 0 ;
			numChildrenBorn = (int) (x + 0.5);
			if ( numChildrenBorn > maxNumChildrenPerBirth )  
				numChildrenBorn = maxNumChildrenPerBirth ;
		}

		for ( int i = 1 ;  i <= numChildrenBorn ;  ++i )
		{
			++(pMom->numOffspring);
			++(pPop->numOffspring);

			pCritter = CreateNativeBornCritter(pMom->id, pPop->id);
			ChildMap.insert(make_pair(pCritter->id, pCritter));
			++numBornThisYear;
		}
	}

	if (ChildMap.empty())
		return;

	// Insert the ChildMap into TheCritterMap and clear the former.
	// Note the use of pos to make the insertion faster.

	pos = TheCritterMap.end();
	--pos;		// pos points at the last critter in the map

	CritterMap::iterator childPos;

	for (childPos = ChildMap.begin();  childPos != ChildMap.end();  ++childPos)
	{
		pos = TheCritterMap.insert(pos, *childPos);
	}

	ChildMap.clear();

}   // end AddCrittersWhichAreBorn


Critter *CreateNativeBornCritter(int momId, int popId)
{
	// Instantiate a new critter:

	Critter *pCritter = new Critter();

	// Set up data members of the new critter - all data members 
	// not set up below (except for id) are zero:

	pCritter->momId          = momId;
	pCritter->popId          = popId;
	pCritter->yearBorn       = curYear;
	pCritter->origin         = kNativeBorn;
	pCritter->gender         = RandomGender(NativeBornGenderDistribution);
	pCritter->probOfOneChild = genNormalRandomProbability ( 
					meanForProbOfOneChild, 
					stdDevForProbOfOneChild ) ;
	return pCritter;

}   // end CreateNativeBornCritter


void AddCrittersToSample(void)
{
	Critter              *pCritter;
	CritterMap::iterator critItr;
	Critter              **Candidates;
	int                  numCandidates;
	int                  k;

	if (curYear <= numYearsToSimulate - numYearsToSample)
		return;

	if (stopSampling || TheCritterMap.size() == 0)
		return;

	Candidates = new Critter *[TheCritterMap.size()];
	numCandidates = 0;

	for (critItr = TheCritterMap.begin();  
				critItr != TheCritterMap.end();  ++critItr)
	{
		pCritter = critItr->second;
		if (GetSampleCritter(pCritter->id) == NULL)
			Candidates[numCandidates++] = pCritter;
	}

	// Now Candidates contains pointers to all critters in TheCritterMap 
	// which are NOT in the SampleMap

	int index = curYear - (numYearsToSimulate - numYearsToSample + 1) ;
	int numBirdsPerSampleThisYear = numBirdsPerSample [index];

	if (numCandidates <= numBirdsPerSampleThisYear)
	{
		// Copy all birds in Candidates to SampleMap

		for (k = 0;  k < numCandidates;  ++k)
		{
			pCritter = Candidates[k];
			SampleMap.insert(make_pair(pCritter->id, 
						pCritter->CopyCritter()));
		}

		delete [] Candidates;

		stopSampling = true;
		return;
	}

	// Select numBirdsPerSampleThisYear at random and add them to SampleMap

	double probabilityPicked = numBirdsPerSampleThisYear / (double)numCandidates;
	int    numPicked = 0;

	for (k = 0;  k < numCandidates;  ++k)
	{
		if (Distribution::RandomNumberFromUnitInterval() >
				probabilityPicked)
			continue;
		pCritter = Candidates[k];
		SampleMap.insert(make_pair(pCritter->id, 
					pCritter->CopyCritter()));
		Candidates[k] = NULL;
		++numPicked;
		if (numPicked == numBirdsPerSampleThisYear)
			break;
	}

	if (numPicked == numBirdsPerSampleThisYear)
	{
		delete [] Candidates;
		return;
	}

	for (k = 0;  k < numCandidates;  ++k)
	{
		pCritter = Candidates[k];

		if (pCritter == NULL)
			continue;

		SampleMap.insert(make_pair(pCritter->id, 
					pCritter->CopyCritter()));
		Candidates[k] = NULL;
		++numPicked;
		if (numPicked == numBirdsPerSampleThisYear)
			break;
	}

	assert(numPicked == numBirdsPerSampleThisYear);

	delete [] Candidates;

}   // end AddCrittersToSample


void CountBirdsByAge(int *countByYear)
{
	Critter              *pCritter;
	CritterMap::iterator critItr;

	for (int i = 0;  i < MaxLifeSpanInYears;  ++i)
		countByYear[i] = 0;

	// Iterate over the map and count critters by age in years

	for (critItr = TheCritterMap.begin();  
				critItr != TheCritterMap.end(); ++critItr)
	{
		pCritter = critItr->second;
		assert ( 0 <= pCritter->age && pCritter->age < MaxLifeSpanInYears ) ;
		++countByYear[pCritter->age];
	}

}  //   end CountBirdsByAge


void AnalyzeFinalPopulation(void)
{
	Critter              *pCritter;
	CritterMap::iterator critItr;
	vector<pair<int,int> > parentsVector;

	parentsVector.reserve ( SampleMap.size() ) ;

	// Iterate over the sample map and for each bird in the sample map,
	// sum up the number of parents that are also in the sample map.
	// In addition, build an array of int's which describe the parents 
	// of critters which are not immigrants and which are not part of 
	// the initial population.

	int numParents = 0;

	for (critItr = SampleMap.begin();  
				critItr != SampleMap.end();  ++critItr)
	{
		pCritter = critItr->second;

        	int momId = pCritter->momId;
		if (momId != 0 && GetSampleCritter(momId) != NULL)
			++numParents;

        	int popId = pCritter->popId;
		if (popId != 0 && GetSampleCritter(popId) != NULL)
			++numParents;

		if ( momId != 0 && popId != 0 )
			parentsVector.push_back ( pair<int,int>(momId, popId) ) ;
	}

	double percent = 0.0;

	if (SampleMap.size() > 0)
		percent = 100.0 * numParents / SampleMap.size();

	if ( !(minFinalPop <= (int)TheCritterMap.size() && (int)TheCritterMap.size() <= maxFinalPop) )
		return;	// Don't report if final population out of bounds

//	printf("\nSibling count array before sort:\n");
//	for ( int i = 0 ;  i < (int)parentsVector.size() ;  ++i )
//		printf("%4d: %10d  %10d\n", i, parentsVector[i].first , parentsVector[i].second );

	int numSiblingPairs = 0 ;

	sort ( parentsVector.begin() , parentsVector.end() ) ;

//	printf("\nSibling count array after sort:\n");
//	for ( int i = 0 ;  i < (int)parentsVector.size() ;  ++i )
//		printf("%4d: %10d  %10d\n", i, parentsVector[i].first , parentsVector[i].second );

	for ( int i = 0 ;  i < (int)parentsVector.size() ;  ++i )
	{
		int j ;

		for ( j = i+1 ;  j < (int)parentsVector.size() ;  ++j )
		{
			if ( ! ( parentsVector[j].first  == parentsVector[i].first  
			      && parentsVector[j].second == parentsVector[i].second ) )
				break;
		}

		int n = j - i;
		numSiblingPairs += n * (n-1) / 2 ;
		i = j - 1 ;
	}

	if (traceOutput)
		printf("\n");
	printf("Experiment %3d:  Final population %3d:  Sample of %3d birds "
		"contains %3d parents (%5.1f%%) and %3d sibling pairs\n", 
		experimentNumber, (int)TheCritterMap.size(), 
                (int)SampleMap.size(), numParents, percent, numSiblingPairs);
	if (traceOutput)
		printf("\n");

	++numExperimentsReported;

}   // end AnalyzeFinalPopulation


Gender OppositeGender(Gender inGender)
{
	Gender outGender = kNoGender;

	switch (inGender)
	{
	case kFemale:	outGender = kMale;	break;
	case kMale:	outGender = kFemale;	break;
	default:	assert(false);
	}

	return outGender;

}   // end OppositeGender


Critter *MustGetCritter(int critterId)
/*
	This function is intended solely for use when it is known that 
	a critter with id critterId exists in the map.
*/
{
	Critter *pCritter;

	pCritter = GetCritter(critterId);

	if (pCritter == NULL)
		assert(false);	// Blow up if critter not in the map

	return pCritter;

}   // end MustGetCritter


Critter *GetCritter(int critterId)
/*
	If a critter exists in the map with id critterId, then return 
	a pointer to that critter.  Otherwise return NULL.
*/
{
	CritterMap::iterator pos;

	pos = TheCritterMap.find(critterId);

	return (pos == TheCritterMap.end() ? NULL : pos->second);

}   // end GetCritter


Critter *GetSampleCritter(int critterId)
/*
	If a critter exists in the sample map with id critterId, then return 
	a pointer to that critter.  Otherwise return NULL.
*/
{
	CritterMap::iterator pos;

	pos = SampleMap.find(critterId);

	return (pos == SampleMap.end() ? NULL : pos->second);

}   // end GetSampleCritter


void GetRidOfAllCritters(void)
{
	CritterMap::iterator critItr;

	// Iterate over the map and delete all critters in the map:

	for (critItr = TheCritterMap.begin();  
				critItr != TheCritterMap.end();  ++critItr)
	{
		delete critItr->second;
	}

	TheCritterMap.clear();


	// Get rid of all critters in the sample map:

	for (critItr = SampleMap.begin();  
				critItr != SampleMap.end();  ++critItr)
	{
		delete critItr->second;
	}

	SampleMap.clear();

}   // end GetRidOfAllCritters


Gender RandomGender(Distribution &distribution)
{
	assert(distribution.NumberOfSlots() == 2);

	return (Gender)(distribution.RandomSlot() + 1);
}


bool VerifyCritters(FILE *fp, const string &tag)
/*
	At present only ages are verified.
*/
{
	Critter              *pCritter;
	CritterMap::iterator critItr;
	bool                 tagOutput = false;
	bool                 ok = true; 

	// Iterate over the map and verify all critters in the map:

	for (critItr = TheCritterMap.begin();  
				critItr != TheCritterMap.end();  ++critItr)
	{
		pCritter = critItr->second;
		if ( 0 <= pCritter->age && pCritter->age < MaxLifeSpanInYears )
			continue;

		ok = false;

		if (!tagOutput)
		{
			if (!tag.empty())
				fprintf(fp, "\n%s:\n", tag.c_str());
			tagOutput = true;
		}

		fprintf ( fp,  "  critter %d:  age out of bounds:  %d\n",  
				pCritter->id, pCritter->age );
	}

	return ok;

}   // end VerifyCritters


void DumpAllCritters(FILE *fp, const string &tag)
{
	Critter              *pCritter;
	CritterMap::iterator critItr;

	if (!tag.empty())
		fprintf(fp, "%s:\n", tag.c_str());

	// Iterate over the map and delete all critters in the map:

	for (critItr = TheCritterMap.begin();  
				critItr != TheCritterMap.end();  ++critItr)
	{
		pCritter = critItr->second;
        	pCritter->DumpCritter(stdout);
		fprintf(fp, "\n");
	}

}   // end DumpAllCritters


void ComputeRandomStuff(void)
{
	if ( ! useRandomValues )
		return;

	meanForProbOfOneChild = genNormalRandomProbability ( 
					meanForMeanProbOfOneChild, 
					stdDevForMeanProbOfOneChild ) ;

	meanForNumChildrenPerBirth = genNormalRandomNumber ( 
					meanForMeanForNumChildrenPerBirth,
					stdDevForMeanForNumChildrenPerBirth);

	ComputeRandomTableStuff
	(
		ProbabilityOfDying,
		MaxLifeSpanInYears,

		meanForProbOfDyingFirstYear,
		stdDevForProbOfDyingFirstYear,
		meanForProbOfDyingAfterFirstYear,
		stdDevForProbOfDyingAfterFirstYear
	);

	ComputeRandomTableStuff
	(
		ImmigrationRateByAge,
		MaxLifeSpanInYears,
		meanForProbOfImmFirstYear,
		stdDevForProbOfImmFirstYear,
		meanForProbOfImmAfterFirstYear,
		stdDevForProbOfImmAfterFirstYear
	);

	ComputeRandomTableStuff
	(
		ProbabilityOfEmigratingAtAge,
		MaxLifeSpanInYears,
		meanForProbOfEmFirstYear,
		stdDevForProbOfEmFirstYear,
		meanForProbOfEmAfterFirstYear,
		stdDevForProbOfEmAfterFirstYear
	);

	if ( ! traceOutput )
		return;

	printf("\nRandomly generated values:\n");
	printf("  Mean for probability of one child:          %0.16g\n", meanForProbOfOneChild ) ;
	printf("  Probability of dying in first year:         %0.16g\n", ProbabilityOfDying[0] ) ;
	printf("  Probability of dying in later years:        %0.16g\n", ProbabilityOfDying[1] ) ;
	printf("  Probability of immigrating in first year:   %0.16g\n", ImmigrationRateByAge[0] ) ;
	printf("  Probability of immigrating in later years:  %0.16g\n", ImmigrationRateByAge[1] ) ;
	printf("  Probability of emigrating in first year:    %0.16g\n", ProbabilityOfEmigratingAtAge[0] ) ;
	printf("  Probability of emigrating in later years:   %0.16g\n", ProbabilityOfEmigratingAtAge[1] ) ;

}  //   end ComputeRandomStuff


void ComputeRandomTableStuff
(
	double *table,
	int    n,
	double mean0,
	double stddev0,
	double meanRest,
	double stddevRest
)
{
	table[0] = genNormalRandomProbability ( mean0,    stddev0    ) ;
	double x = genNormalRandomProbability ( meanRest, stddevRest ) ;

	for ( int i = 1; i < n;  ++i )
		table[i] = x;
}


double genNormalRandomProbability ( double mean, double stddev )
{
	double x = genNormalRandomNumber ( mean, stddev ) ;

	if ( x < 0 )  return 0.0 ;
	if ( x > 1 )  return 1.0 ;

	return x ;
}


void ParseCommandLine(int argc, char **argv)
{
	string errorMess;

	if ( argc != 2 )
	{
		DisplayUsage();
		exit(1);
	}

	parmFileName = argv[1] ;

	parmfp = fopen(parmFileName, "r");

	if (parmfp == NULL)
	{
		DisplayUsage();
		printf("\n***ERROR:  Cannot open parm file %s\n", parmFileName);
		exit(1);
	}

	errorMess = LoadParmFile();

	fclose(parmfp);

	if (!errorMess.empty())
	{
		if (parmLineNum > 0 ) 
			fprintf(stderr, "\n***ERROR in parm file at line %d:  %s\n", 
						parmLineNum, errorMess.c_str());
		else
			fprintf(stderr, "\n***ERROR in parm file:  %s\n", errorMess.c_str());
		exit(1);
	}

} // end ParseCommandLine


string LoadParmFile(void)
{
	Parm   *p;
	size_t  len;
	char   *s;
	char   *endptr;
	int	intVal;
	double	doubleVal;
	const char *initPopStr   = "InitialPopAgeDistributionValues";
	int     initPopLen   = strlen(initPopStr);
	int     initPopCount = 0;
	const char *numBirdsPopStr   = "numBirdsPerSample";
	int     numBirdsPopLen   = strlen(numBirdsPopStr);
	int     numBirdsPopCount = 0;

	for ( size_t i = 0 ;  i <  PARM_TABLE_SIZE;  ++i )
	{
		p = &parmTable[i];

		if ( !getLine() )
			return "Too few parameters";

		len = p->name.size();

		if ( strncmp (parmLine, p->name.c_str(), len ) != 0 )
			return "Not a parm or parms not in correct order";

		s = parmLine + len ;

		if (p->integerType)
		{
			intVal = (int)strtol(s, &endptr, 0);
			if (s == endptr)
				return "Invalid integer value";

			if (strcmp(p->name.c_str(), "MaxLifeSpanInYears=") == 0)
			{
				if (intVal < MinMaxLifeSpanInYears)
					return "MaxLifeSpanInYears parameter too small";
				if (intVal > MaxMaxLifeSpanInYears)
					return "MaxLifeSpanInYears parameter too large";
			}
			else
			{
				if (intVal < p->lbInt)
					return "Integer parameter too small";
				if (intVal > p->ubInt)
					return "Integer parameter too large";

				if (strcmp(p->name.c_str(), "numYearsToSample=") == 0)
				{
					if (intVal > numYearsToSimulate)
						return "numYearsToSample parameter greater than numYearsToSimulate";
				}
			}

			if (strncmp(p->name.c_str(), numBirdsPopStr, numBirdsPopLen) == 0)
			{
				// Make sure numYearsToSample already
				// appeared in parmTable:
				assert( numYearsToSample > 0);
				++numBirdsPopCount;
				// Skip parmTable entries if numYearsToSample
				// is less than MaxNumYearsToSample
				if (numBirdsPopCount == numYearsToSample)
					i += MaxNumYearsToSample
						- numYearsToSample;
			}

			*(p->pInt) = intVal ;
		}
		else
		{
			doubleVal = strtod(s, &endptr);
			if (s == endptr)
				return "Invalid double value";
			if (doubleVal < p->lbDouble)
				return "Double parameter too small";
			if (doubleVal > p->ubDouble)
				return "Double parameter too large";

			if (strncmp(p->name.c_str(), initPopStr, initPopLen) == 0)
			{
				// Make sure MaxLifeSpanInYears already
				// appeared in parmTable:
				assert(MaxLifeSpanInYears > 0);
				++initPopCount;
				// Skip parmTable entries if MaxLifeSpanInYears
				// is less than MaxMaxLifeSpanInYears
				if (initPopCount == MaxLifeSpanInYears)
					i += MaxMaxLifeSpanInYears 
						- MaxLifeSpanInYears;
			}

			*(p->pDouble) = doubleVal ;
		}
	}

	if ( getLine() )
		return "Too many parameters";

	parmLineNum = 0;

	if (MarriageableAge < MinMaxLifeSpanInYears)
		return "MarriageableAge parameter too small";
	if (MarriageableAge > MaxLifeSpanInYears)
		return "MarriageableAge parameter too large";

	return "";

}   //  end LoadParmFile


bool getLine(void)
/*
	A true  return value means a line was read.  It is in parmLine.
	A false return value means EOF was encountered
*/
{
	size_t len ;

	for (;;)
	{
		if ( fgets(parmLine, MAX_LINE_LEN, parmfp) == NULL )
			return false;

		++parmLineNum;

		if ( strncmp(parmLine, "//", 2) == 0 )	// comment line
			continue ;

		len = strlen(parmLine) ;

		if (len > 0 && parmLine[len-1] == '\n')
			parmLine[--len] = '\0';

		len = strlen(parmLine) ;

		if (len > 0 && parmLine[len-1] == '\r')
			parmLine[--len] = '\0';

		len = strlen(parmLine) ;

		if ( strncmp(parmLine, "**", 2) == 0 )	// asterisk line
		{
			puts(parmLine+2);
			continue ;
		}

		if ( strspn(parmLine, " \t") == len )   // whitespace line
			continue;

		break;
	}

	return true;

}   //  end getLine


void DisplayUsage(void)
{
printf("\n");
printf("Usage:\n");
printf("\n");
printf("                parsim  parmfilename\n");
printf("\n");
printf("This program takes one parameter, the name of a parameter file.  All\n");
printf("output is written to stdout.\n");

} // end DisplayUsage


// End parsim.cpp
