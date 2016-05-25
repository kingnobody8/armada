#include "Paker.h"
using namespace std;

namespace Pak
{
	namespace Paker
	{
		CPaker::CPaker()
		{
		}
		CPaker::~CPaker()
		{
		}

		//Main Functions
		void CPaker::ClearPak(void)
		{
			//Close any open file streams
			if (this->stream.is_open())
				this->stream.close();

			//Clears any internal memory
			this->atlas.clear();
		}
		bool CPaker::LoadPak(const char* _pak_file)
		{
			//Reset the list
			ClearPak();

			//Open stream
			this->stream.open(_pak_file, std::ios_base::in /*| std::ios_base::out*/ | std::ios_base::binary);

			//Early out
			if (!this->stream.is_open())
				return false;

			//Read Header
			PakHeader header;
			stream.read(reinterpret_cast<char*>(&header), sizeof(PakHeader));

			//Test the signature
			string headSig;
			string testSig;
			for (int i = 0; i < kucSignatureLength; i++)
			{
				headSig.push_back(header.signature[i]);
				testSig.push_back(SIG[i]);
			}
			if (strcmp(testSig.c_str(), headSig.c_str()) != 0)
				return false;

			//If atlas exists
			if (header.length == 0)
				return true;

			//Seek to the atlas
			stream.seekg(header.offset, ios_base::beg);

			//Get count of FileHeaders
			int AFH = header.length / sizeof(FileHeader);

			//Fill the atlas
			for (int i = 0; i < AFH; i++)
			{
				//Seek to FileHeader location
				stream.seekg(header.offset + i * sizeof(FileHeader), ios_base::beg);

				//Create FileHeaderPair
				pair<string, FileHeader> FHP;

				//Copy data into FileHeaderPair
				stream.read(reinterpret_cast<char*>(&FHP.second), sizeof(FileHeader));
				FHP.first = FHP.second.fileName;

				//Place data into the map
				atlas.insert(FHP);
			}

			return true;
		}
		bool CPaker::IsInPak(const char* _file_name)
		{
			//Get the iterator to the atlas map
			auto iter = this->atlas.find(_file_name);

			//Check if the iterator is valid
			if (iter == this->atlas.end())
				return false;

			return true;
		}
		bool CPaker::GetAsset(const char* _asset_name, char** _buffer, ulong& _file_size)
		{
			//Test if file is in the PAK
			if (IsInPak(_asset_name) == false)
				return false;

			//Go through atlas and find the mem location of the directory
			FileHeader header = atlas[_asset_name];

			//Set the _file_size
			_file_size = header.byteLength;

			//Allocate memory for the buffer
			*_buffer = new char[_file_size];

			//Seek to mem location
			stream.seekg(header.byteOffset, ios_base::beg);

			//Read memory into buffer
			stream.read(reinterpret_cast<char*>(*_buffer), _file_size);

			return true;
		}
	}

	static Paker::CPaker pak;

	void Clear(void)
	{
		pak.ClearPak();
	}
	bool Load(const char* _pak_file)
	{
		return pak.LoadPak(_pak_file);
	}
	bool IsInPak(const char* _file_name)
	{
		return pak.IsInPak(_file_name);
	}
	bool GetAsset(const char* _asset_name, char** _buffer, ulong &_file_size)
	{
		return pak.GetAsset(_asset_name, _buffer, _file_size);
	}
}