/*
File:		Rune.h
Author:		Daniel Habig
Purporse:	The Rune struct contains the data that makes up an entire alphabet
Date:		6-1-13
*/

#pragma once
#include "../Util/Types.h"
using namespace Type;

namespace Text
{
	const ulong kulNumLetters = 256;
	struct Rune
	{
		slong			lets[kulNumLetters];
		Type2<slong>	pix;
		Type2<slong>	set;
		slong			imgID;

		Rune(void)
		{
			pix.x = pix.y = set.x = set.y = 0;
			imgID = -1;
			for (long i = 0; i < kulNumLetters; i++)
				lets[i] = 0;
		}

	};
}