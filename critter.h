/*  critter.h  ->  A class for describing critters

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
#include <stdio.h>
#include <map>

using namespace std;


// An "immigrant" is a critter that arrives from another population.
// An "emigrant"  is a critter that leaves our population and goes elsewhere.

enum Origin	// Origin of a critter
{
	kNoOrigin,

	kPartOfInitialPopulation,
	kImmigrant,
	kNativeBorn,

	kNumOrigins
};


enum Gender	// Gender of a critter
{
	kNoGender,

	kFemale,
	kMale,

	kNumGenders
};

/*
	Important info on Critters:

	If the mateId of a critter c is a non-zero value k, then a critter
	with id k exists, and the mate of critter k is critter c.

	If the momId of a critter c is non-zero value m, it does NOT mean 
	that a critter with id m exists.  Ditto for the popId.  In other 
	words, when a critter c dies, the momId and popId of the children 
	of c are not changed.
*/

class Critter	// Description of a critter
{
public:
	int	id;		// Unique non-zero id of this critter
	int	momId;		// Id of mom; zero unless native born
	int	popId;		// Id of pop; zero unless native born
	int	mateId;		// Id of mate; zero if no mate
	int	age;		// Age of this critter
	int	yearBorn;	// What year critter was born; can be < 0
	int	numOffspring;	// Total no. of offspring this critter has had
	Origin	origin;		// Where did this critter come from
	Gender	gender;		// Male or female
	double	probOfOneChild; // Probability that one child is born
	Critter();		// Default constructor
virtual	~Critter();		// Destructor

	Critter *CopyCritter(void);

	void DumpCritter(FILE *fp, char *tag = NULL);

private:  // We don't allow the following:
	Critter(const Critter &rhs);			// Copy ctor
	Critter	&operator=(const Critter &rhs);	// Assignment operator
	bool	operator==(const Critter &rhs) const;
	bool	operator!=(const Critter &rhs) const;
public:

static	int	lastAssignedCritterId;	// For assigning unique id's

};  // end class Critter


// Now we define a map whose keys are critter ids and whose elements are 
// pointers to Critters:

typedef map<int, Critter*> CritterMap;

//  end critter.h
