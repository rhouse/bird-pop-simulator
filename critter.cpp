/*  critter.cpp  ->  A class for describing critters

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


#include "critter.h"

int	Critter::lastAssignedCritterId = 0;


Critter::Critter()		// Default constructor
{
	id             = ++Critter::lastAssignedCritterId;
	momId          = 0;
	popId          = 0;
	mateId         = 0;
	age            = 0;
	yearBorn       = 0;
	numOffspring   = 0;
	origin         = kNoOrigin;
	gender         = kNoGender;
	probOfOneChild = 0.0;

}   // end Critter default ctor


Critter::~Critter()		// Destructor
{
	// Nothing to do at present
}


Critter *Critter::CopyCritter(void)
{
	Critter *p = new Critter;

	p->id             = id;
	p->momId          = momId;
	p->popId          = popId;
	p->mateId         = mateId;
	p->age            = age;
	p->yearBorn       = yearBorn;
	p->numOffspring   = numOffspring;
	p->origin         = origin;
	p->gender         = gender;
	p->probOfOneChild = probOfOneChild;

	return p;

}   // end CopyCritter


void Critter::DumpCritter(FILE *fp, char *tag)
{
	if (tag != NULL && *tag != 0)
		fprintf(fp, "%s:\n", tag);
	fprintf(fp, "  id             = %d\n",     id             );
	fprintf(fp, "  momId          = %d\n",     momId          );
	fprintf(fp, "  popId          = %d\n",     popId          );
	fprintf(fp, "  mateId         = %d\n",     mateId         );
	fprintf(fp, "  age            = %d\n",     age            );
	fprintf(fp, "  yearBorn       = %d\n",     yearBorn       );
	fprintf(fp, "  numOffspring   = %d\n",     numOffspring   );
	fprintf(fp, "  origin         = %d\n",     origin         );
	fprintf(fp, "  gender         = %d\n",     gender         );
	fprintf(fp, "  probOfOneChild = %0.16g\n", probOfOneChild );

}   // end DumpCritter

//  end critter.cpp
