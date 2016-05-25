#include "Text.h"
#include "../Paker/Paker.h"

namespace Text
{
#define DEFAULT_FONT 0

	CText::CText()
	{
		m_pSprite = NULL;
	}
	CText::~CText()
	{
	}

	void CText::Start(Sprite::CSprite* pSprite)
	{
		m_pSprite = pSprite;
	}
	void CText::Stop()
	{
	}

	void CText::LoadBinData(const char* fontFile, const char* imgFile)
	{
		char* buffer = NULL;
		ulong size = 0;
		Pak::GetAsset(fontFile, &buffer, size);

		Rune rune;
		memcpy(&rune, buffer, size);

		delete buffer;
		buffer = NULL;
		size = 0;

		rune.imgID = this->m_pSprite->LoadTex(imgFile, true);
		this->m_vRunes.push_back(rune);
	}
	Word CText::GetWord(const char* szText, bool bSpace, slong slCounter)
	{
		Word ret;
		int len = strlen(szText);
		if (len == 0)//Quick escape
			return ret;

		if (bSpace)
		{
			//Get Spaces
			char delim[256];
			for (ulong i = 0; i < 256; i++)
				delim[i] = char(i);
			delim[0] = delim[' '] = 1;

			char* wordPtr = new char[len + 1];
			strcpy_s(wordPtr, len + 1, szText);
			char* context = nullptr;

			char* tok = strtok_s(wordPtr, delim, &context);

			for (int i = 0; i <= slCounter; i++)
			{
				if (tok == NULL)
				{
					ret.text = "";
					ret.pixWidth = 0;
					break;
				}
				ret.text = tok;
				ret.pixWidth = this->GetWordPixelLength(ret.text.c_str());
				tok = strtok_s(NULL, delim, &context);
			}

			/*while (tok != nullptr && count <= slCounter )
			{
				ret.text = tok;
				ret.pixWidth = this->GetWordPixelLength(ret.text.c_str());
				tok = strtok_s(NULL, delim, &context);
				count++;
			}*/
			delete[] wordPtr;
		}
		else
		{
			char* wordPtr = new char[len + 1];
			strcpy_s(wordPtr, len + 1, szText);
			char delim[] = " ";
			char* context = nullptr;

			char* tok = strtok_s(wordPtr, delim, &context);
			for (int i = 0; i <= slCounter; i++)
			{
				if (tok == NULL)
				{
					ret.text = "";
					ret.pixWidth = 0;
					break;
				}
				ret.text = tok;
				ret.pixWidth = this->GetWordPixelLength(ret.text.c_str());
				tok = strtok_s(NULL, delim, &context);
			}
			delete[] wordPtr;
		}

		return ret;
	}
	std::vector<Word> CText::ParseStringIntoWords(const char* szText)
	{
		// Create a vector of the words in the string
		std::vector<Word> vWords;
		slong slCounter = 0;
		slong len = strlen(szText);
		if (len == 0)//Quick escape
			return vWords;

		if (szText[0] == ' ')	//Do spaces first, then words
		{
			Word word;
			do
			{
				word = GetWord(szText, true, slCounter);
				if (word.text.empty())
					break;
				vWords.push_back(word);
				word = GetWord(szText, false, slCounter);
				if (word.text.empty())
					break;
				vWords.push_back(word);
				slCounter++;
			} while (!word.text.empty());
		}
		else					//Do words first, then spaces
		{
			Word word;
			do
			{
				word = GetWord(szText, false, slCounter);
				if (word.text.empty())
					break;
				vWords.push_back(word);
				word = GetWord(szText, true, slCounter);
				if (word.text.empty())
					break;
				vWords.push_back(word);
				slCounter++;
			} while (!word.text.empty());
		}
		return vWords;
	}
	std::vector<Word> CText::ParseWordsIntoLines(std::vector<Word> tWords, slong slWidth, float fScale)
	{
		std::vector<Word> tLines;
		int pixWidth = 0;
		int wordCount = 0;
		int lineCount = 0;
		tLines.push_back(Word());

		for (unsigned int i = 0; i < tWords.size(); i++)
		{
			//If the word fits on the current line, then add it to the line
			if (pixWidth + tWords[i].pixWidth <= slWidth)
			{
				tLines[lineCount] += tWords[i];
				pixWidth += tWords[i].pixWidth;
			}
			else if (tWords[i].text[0] == ' ') //If the word is actually space(s) then skip them
			{
				continue;
			}
			//Try it without the Kern
			else if (pixWidth + tWords[i].pixWidth - (slong)(fScale * m_vRunes[DEFAULT_FONT].pix.x * KERN) <= slWidth)
			{
				tLines[lineCount] += tWords[i];
				pixWidth += tWords[i].pixWidth - (slong)(fScale * m_vRunes[DEFAULT_FONT].pix.x * SPACE) - (slong)(fScale * m_vRunes[DEFAULT_FONT].pix.x * KERN);
			}
			
			else if (tWords[i].pixWidth > slWidth)//If the word is too big for any line, then put it somewhere
			{
				if (pixWidth == 0) //Put it on the current line
				{
					tLines[lineCount] += tWords[i];
					pixWidth += tWords[i].pixWidth;
				}
				else //Put it on the next line
				{
					lineCount++;
					pixWidth = 0;
					tLines.push_back(Word());
					tLines[lineCount] += tWords[i];
					pixWidth += tWords[i].pixWidth;
				}

			}
			else//If the word is too big for just that line, then put it on the next line
			{
				i--;
				lineCount++;
				pixWidth = 0;
				tLines.push_back(Word());
			}
		}

		//Remove final kern on each line
		ulong size = tLines.size();
		for (ulong i = 0; i < size; i++)
			tLines[i].pixWidth -= (slong)(fScale * m_vRunes[DEFAULT_FONT].pix.x * KERN);

		return tLines;
	}
	unsigned int CText::GetWordPixelLength(const char* word)
	{
		ulong val = 0;
		int len = strlen(word);
		for (int i = 0; i < len; i++)
		{
			char c = word[i];
			if (c == (' '))
				val += (int)(m_vRunes[DEFAULT_FONT].pix.x * SPACE);
			else
				val += (int)(m_vRunes[DEFAULT_FONT].lets[c]) + (int)(m_vRunes[DEFAULT_FONT].pix.x * KERN);
		}

		return val;
	}
	void CText::AddToQueue(std::vector<Word> vWords, std::string nStr)
	{
		TTextPair pair;
		pair.lines = vWords;
		pair.str = nStr;
		this->m_vTextQueue.push_back(pair);
		if (this->m_vTextQueue.size() > WORD_QUEUE)
			this->m_vTextQueue.pop_front();
	}
	void CText::WriteToScreen(std::vector<Word> tLines, _RECT<slong> tBox, float fScale, _COLOR<uchar> clr, EFontAlignment eAlign, SDL_BlendMode bMode)
	{
		switch (eAlign)
		{
		case EFontAlignment::F_LEFT:
			this->WriteLeftAlign(tLines, tBox, fScale, clr, bMode);
			break;
		case EFontAlignment::F_CENTER:
			this->WriteCenterAlign(tLines, tBox, fScale, clr, bMode);
			break;
		case EFontAlignment::F_RIGHT:
			this->WriteRightAlign(tLines, tBox, fScale, clr, bMode);
			break;
		}

		//this->m_pSprite->DrawRect(tBox, CLR::WHITE, false);
	}
	void CText::WriteLeftAlign(std::vector<Word> tLines, _RECT<slong> tBox, float fScale, _COLOR<uchar> clr, SDL_BlendMode bMode)
	{
		slong dir = (this->m_pSprite->GetCoordSys() == Sprite::CSprite::CARTESIAN) ? -1 : 1;

		Type2<slong> pos;
		for (unsigned int i = 0; i < tLines.size(); i++)
		{
			pos.x = tBox.x;
			pos.y = tBox.y + dir * i * (int)(fScale * m_vRunes[DEFAULT_FONT].pix.y);

			for (unsigned int l = 0; l < tLines[i].text.length(); l++)
			{
				//Get the current letter
				char c = tLines[i].text[l];

				// Add space and update location
				if (c == (' '))
				{
					pos.x += (int)(fScale * m_vRunes[DEFAULT_FONT].pix.x * SPACE);
					continue;
				}
				else// Add symbol
				{
					//Get the symbol rect on font.png
					SDL_Rect rChar;
					rChar.y = c / m_vRunes[DEFAULT_FONT].set.x * m_vRunes[DEFAULT_FONT].pix.y;
					rChar.x = c % m_vRunes[DEFAULT_FONT].set.x * m_vRunes[DEFAULT_FONT].pix.x;
					rChar.w = m_vRunes[DEFAULT_FONT].lets[c];
					rChar.h = m_vRunes[DEFAULT_FONT].pix.y;
					//Draw the symbol
					m_pSprite->DrawTex(m_vRunes[DEFAULT_FONT].imgID, pos, Type2<float>(fScale, fScale), &rChar, nullptr, bMode, clr);
					//Update the spacing
					pos.x += (int)(fScale * m_vRunes[DEFAULT_FONT].lets[c]) + (int)(fScale * m_vRunes[DEFAULT_FONT].pix.x * KERN);
					continue;
				}
			}
		}
	}
	void CText::WriteCenterAlign(std::vector<Word> tLines, _RECT<slong> tBox, float fScale, _COLOR<uchar> clr, SDL_BlendMode bMode)
	{
		slong dir = (this->m_pSprite->GetCoordSys() == Sprite::CSprite::CARTESIAN) ? -1 : 1;

		Type2<slong> pos;
		for (unsigned int i = 0; i < tLines.size(); i++)
		{
			pos.x = tBox.x + (int)((tBox.w - tLines[i].pixWidth) * 0.5f);
			pos.y = tBox.y + dir * i * (int)(fScale * m_vRunes[DEFAULT_FONT].pix.y);

			for (unsigned int l = 0; l < tLines[i].text.length(); l++)
			{
				//Get the current letter
				char c = tLines[i].text[l];

				// Add space and update location
				if (c == (' '))
				{
					pos.x += (int)(fScale * m_vRunes[DEFAULT_FONT].pix.x * SPACE);
					continue;
				}
				else// Add symbol
				{
					//Get the symbol rect on font.png
					SDL_Rect rChar;
					rChar.y = c / m_vRunes[DEFAULT_FONT].set.x * m_vRunes[DEFAULT_FONT].pix.y;
					rChar.x = c % m_vRunes[DEFAULT_FONT].set.x * m_vRunes[DEFAULT_FONT].pix.x;
					rChar.w = m_vRunes[DEFAULT_FONT].lets[c];
					rChar.h = m_vRunes[DEFAULT_FONT].pix.y;
					//Draw the symbol
					this->m_pSprite->DrawTex(m_vRunes[DEFAULT_FONT].imgID, pos, Type2<float>(fScale, fScale), &rChar, nullptr, bMode, clr);
					//Update the spacing
					pos.x += (int)(fScale * m_vRunes[DEFAULT_FONT].lets[c]) + (int)(fScale * m_vRunes[DEFAULT_FONT].pix.x * KERN);
					continue;
				}
			}
		}
	}
	void CText::WriteRightAlign(std::vector<Word> tLines, _RECT<slong> tBox, float fScale, _COLOR<uchar> clr, SDL_BlendMode bMode)
	{
		slong dir = (this->m_pSprite->GetCoordSys() == Sprite::CSprite::CARTESIAN) ? -1 : 1;

		Type2<slong> pos;
		for (unsigned int i = 0; i < tLines.size(); i++)
		{
			pos.x = tBox.x + tBox.w;
			pos.y = tBox.y + dir * i * (int)(fScale * m_vRunes[DEFAULT_FONT].pix.y);

			bool first = true;
			for (unsigned int l = tLines[i].text.length() - 1; !(l >= tLines[i].text.length()); l--)
			{
				//Get the current letter
				char c = tLines[i].text[l];

				// Add space and update location
				if (c == (' '))
				{
					//Ignore if at the end of the line
					if (l == tLines[i].text.length() - 1)
						continue;
					pos.x -= (int)(fScale * m_vRunes[DEFAULT_FONT].pix.x * SPACE);
					continue;
				}
				else// Add symbol
				{
					//Get the symbol rect on font.png
					SDL_Rect rChar;
					rChar.y = c / m_vRunes[DEFAULT_FONT].set.x * m_vRunes[DEFAULT_FONT].pix.y;
					rChar.x = c % m_vRunes[DEFAULT_FONT].set.x * m_vRunes[DEFAULT_FONT].pix.x;
					rChar.w = m_vRunes[DEFAULT_FONT].lets[c];
					rChar.h = m_vRunes[DEFAULT_FONT].pix.y;
					//Update the spacing
					if (first)
					{
						pos.x -= (int)(fScale * m_vRunes[DEFAULT_FONT].lets[c]);
						first = false;
					}
					else
					{
						pos.x -= (int)(fScale * m_vRunes[DEFAULT_FONT].lets[c]) + (int)(fScale * m_vRunes[DEFAULT_FONT].pix.x * KERN);
					}
					//Draw the symbol
					this->m_pSprite->DrawTex(m_vRunes[DEFAULT_FONT].imgID, pos, Type2<float>(fScale, fScale), &rChar, nullptr, bMode, clr);
					continue;
				}
			}
		}
	}
	void CText::Write(const char* strText, _RECT<slong> tBox, _COLOR<uchar> clr, float fScale, bool format, EFontAlignment eAlign, SDL_BlendMode bMode)
	{
		slong pixWidth = 0;
		slong maxWidth = tBox.w;
		slong line = 0;
		Type2<slong> loc = Type2<slong>(tBox.x, tBox.y);


		//See if the word is in the list of words
		for (auto iter = this->m_vTextQueue.begin(); iter != this->m_vTextQueue.end(); iter++)
		{
			if (strcmp((*iter).str.c_str(), strText) == 0) // Don't calculate new lines, use the previously calculated ones
			{
				this->WriteToScreen((*iter).lines, tBox, fScale, clr, eAlign, bMode);
				return;
			}
		}

		// Create a vector of the words in the string
		std::vector<Word> words = ParseStringIntoWords(strText);

		//Update the word widths by the scale
		for (unsigned int i = 0; i < words.size(); i++)
			words[i].pixWidth = int(words[i].pixWidth * fScale);
		std::vector<Word> lines = ParseWordsIntoLines(words, tBox.w, fScale);

		//Add the list of lines
		AddToQueue(lines, strText);

		//Print
		this->WriteToScreen(lines, tBox, fScale, clr, eAlign, bMode);
	}

	Type2<ulong>	CText::GetSize(const char* strText, float fScale, bool bWithFinalKern)
	{
		ulong pixWidth = 0;

		// Create a vector of the words in the string
		std::vector<Word> words = ParseStringIntoWords(strText);

		//Update the word widths by the scale
		ulong size = words.size();
		for (ulong i = 0; i < size; i++)
		{
			words[i].pixWidth = int(words[i].pixWidth * fScale);
			pixWidth += words[i].pixWidth;
			if (!bWithFinalKern && i == size - 1)
				pixWidth -=(slong)(fScale * m_vRunes[DEFAULT_FONT].pix.x * KERN); //Remove the Final Kern
		}
		Type2<ulong> ret = Type2<ulong>(pixWidth, (ulong)(this->m_vRunes[DEFAULT_FONT].pix.h * fScale));
		return ret;
	}

}