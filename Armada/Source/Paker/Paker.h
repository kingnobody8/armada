// - Pak File 
// - Creator -> Daniel Habig
//
// This implementation of the a pak file loader will load one and only one pak file at a time
// This is intended to be a quick and easy class to get pak loading going
// It is suggested that you make a Paker.cpp and place the functions in there
//--------------------------

#pragma once
#include <unordered_map>
#include <string>
#include <fstream>
#include "../Util/Types.h"
using namespace Type;

namespace Pak
{
	namespace Paker
	{
		const uchar kucSignatureLength = 4;
		const char	SIG[] = { 'P', 'A', 'C', 'K' };
		const uchar kucFileNameLength = 56;


		// signature	- used to determine if file is a valid pack file
		// length		- the length in bytes of all the FileHeaders in the atlas
		// offset		- the absolute position in bytes of the atlas
		struct PakHeader
		{
			char signature[kucSignatureLength];
			int length;
			int offset;
		};


		// filename	- relative path of file
		// length	- the size in bytes of the file
		// offset	- the absolute position in bytes of the file
		struct FileHeader
		{
			char fileName[kucFileNameLength];
			int byteLength;
			int byteOffset;
		};

		class CPaker
		{
		private:
			std::unordered_map< std::string, FileHeader > atlas;
			std::fstream stream;

		public:
			CPaker(void);
			~CPaker(void);

			void ClearPak(void);
			bool LoadPak(const char* _pak_file);
			bool IsInPak(const char* _file_name);
			bool GetAsset(const char* _asset_name, char** _buffer, ulong &_file_size);
		};
	}

	void Clear(void);
	bool Load(const char* _pak_file);
	bool IsInPak(const char* _file_name);
	bool GetAsset(const char* _asset_name, char** _buffer, ulong &_file_size);
}