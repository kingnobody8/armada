/*
File:		Word.h
Author:		Daniel Habig
Purporse:	The word struct contains the data that makes up a word made from runes
Date:		6-1-14
*/

#pragma once
#include <string>
#include "../Util/Types.h"
using namespace Type;

namespace Text
{
	struct Word
	{
		slong pixWidth; /*in pixels*/
		std::string text;

		Word(void)
		{
			pixWidth = 0;
			text = "";
		}
		inline Word operator+(const Word &v) const
		{
			Word word;
			word.text = this->text + v.text;
			word.pixWidth = this->pixWidth + v.pixWidth;
			return word;
		}
		inline Word &operator+=(const Word &v)
		{
			this->text += v.text;
			this->pixWidth += v.pixWidth;
			return *this;
		}
	};
}