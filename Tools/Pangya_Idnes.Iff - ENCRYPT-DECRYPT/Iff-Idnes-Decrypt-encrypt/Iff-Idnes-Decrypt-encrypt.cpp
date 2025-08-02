// UpdateList_DE-Crypt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Iff-Decrypt-encrypt.h"
#include <vector>

// Function to process a single file
void processFile(const std::string& fileName, uint* key, const char* keyName) {
    printf("\n-----------------------------------------------\n");
    printf("Processing file: %s\n", fileName.c_str());

    int result = FileCryptDecrypt(fileName, key);

    if (result == -1) {
        printf("SUCCESS: The file has been encrypted!\n");
    } else if (result == 0) {
        printf("SUCCESS: The file has been decrypted!\n");
    } else if (result == 1) {
        printf("FAILED: Decryption failed. The key for '%s' is not correct for this file.\n", keyName);
    } else if (result == 2) {
        printf("FAILED: Sorry, I don't know how to encrypt this file!\n");
    } else if (result == 3) {
        // This case is handled by the check before calling, but included for completeness
        printf("INFO: File not found.\n");
    }
}

int main(int argc, char* argv[]) {
    printf("-----------------------------------------------\n");
    printf("|      Pangya Iff Season 2 Encrypt/Decrypt    |\n");
    printf("|                   Version 2.0               |\n");
    printf("|      (Now processes all supported files)    |\n");
    printf("-----------------------------------------------\n");
    printf("Created by DaveDevil's - Special thanks to HSReina\n");
    printf("Edit: Vitinho\n");

    // A list of supported files and their corresponding keys
    std::vector<std::pair<std::string, uint*>> supportedFiles;
    supportedFiles.push_back(std::make_pair("pangya_idnes.iff", keyIdnesiff));
    supportedFiles.push_back(std::make_pair("pangya_brs.iff", keyBriff));
    supportedFiles.push_back(std::make_pair("pangya_sg.iff", keySeaiff));

    int filesProcessed = 0;

    // Iterate through the list of supported files
    for (const auto& file_pair : supportedFiles) {
        const std::string& fileName = file_pair.first;
        uint* key = file_pair.second;

        // Check if the file exists in the current directory
        ifstream f(fileName.c_str());
        if (f.good()) {
            f.close();
            processFile(fileName, key, fileName.c_str());
            filesProcessed++;
        } else {
            f.close();
        }
    }

    if (filesProcessed == 0) {
        printf("\nNo supported files found in the application's folder.\n");
        printf("Please place pangya_idnes.iff, pangya_brs.iff, or pangya_sg.iff here.\n");
    }

    printf("\n-----------------------------------------------\n");
    printf("Processing complete. Press any key to exit.\n");
    system("pause");
    return 1;
}


int FileCryptDecrypt(string filename, uint key[4])
{
	bool encrypt = false;
	int ret;
	ifstream fOriginalFile(filename.c_str(), ios::binary | ios::ate);
	ifstream::pos_type pos = fOriginalFile.tellg();

	int size = (int)pos;

	if (size < 0)
	{
		// This check is now redundant because we check for file existence before calling
		printf("File not found : %s \n", filename.c_str());
		fOriginalFile.close();
		return 3;
	}

	char * Data = new char[size];
	char * DataFinal = new char[size];

	fOriginalFile.seekg(0, ios::beg);

	fOriginalFile.read(Data, size);

	// If data is decrypted -> check what crypt i need
    if (Data[0] == 'P' && Data[1] == 'K')
    {
        printf("Input file is a ZIP archive. Trying to Encrypt...\n");
        encrypt = true;
    }
    else
    {
        printf("Input file is encrypted. Trying to Decrypt...\n");
    }

	for (int i = 0; i < size; i = i + 8)
	{
		uint DataCut[8];

		memcpy(&DataCut[0], &Data[i], 8);

		if (encrypt)
		{
			xtea_encipher(16, (uint*)DataCut, key);
		}
		else
		{
			xtea_decipher(16, (uint*)DataCut, key);
		}

		memcpy(&DataFinal[i], &DataCut[0], 8);


		//If Decrypt fail ...
		if (i == 0 && DataFinal[0] != 'P' && DataFinal[1] != 'K' && encrypt == false)
		{
			fOriginalFile.close();
            delete[] Data;
            delete[] DataFinal;
			return 1; // Return 1 for failed decryption
		}
	}


	if (encrypt)
	{
        // To avoid creating "pangya_sg.iff.iff", we remove the original extension.
        string base_filename = filename.substr(0, filename.find_last_of('.'));
		string savefilename = base_filename + "_encrypted.iff";
        printf("Saving encrypted file to: %s\n", savefilename.c_str());
		std::fstream SaveFile(savefilename.c_str(), std::ios::out | std::ios::binary);
		SaveFile.write(DataFinal, size);
		SaveFile.close();
		ret = -1;
	}
	else
	{
        // To avoid creating "pangya_sg.iff.zip", we remove the original extension.
        string base_filename = filename.substr(0, filename.find_last_of('.'));
		string savefilename = base_filename + "_decrypted.zip";
        printf("Saving decrypted file to: %s\n", savefilename.c_str());
		std::fstream SaveFile(savefilename.c_str(), std::ios::out | std::ios::binary);
		SaveFile.write(DataFinal, size);
		SaveFile.close();
		ret = 0;
	}


	fOriginalFile.close();
    delete[] Data;
    delete[] DataFinal;
	return ret;
}

void xtea_encipher(unsigned int num_rounds, uint data[2], uint key[4]) 
{
	uint i;
	uint data0 = data[0];
	uint data1 = data[1];
	uint delta = 0x61c88647, sum = 0;
	for (i = 0; i < num_rounds; i++)
	{
		data0 += (((data1 << 4) ^ (data1 >> 5)) + data1) ^ (sum + key[sum & 3]);
		sum -= delta;
		data1 += (((data0 << 4) ^ (data0 >> 5)) + data0) ^ (sum + key[(sum >> 11) & 3]);
	}
	data[0] = data0;
	data[1] = data1;
}

void xtea_decipher(unsigned int num_rounds, uint data[2], uint key[4]) 
{
	uint i;
	uint data0 = data[0];
	uint data1 = data[1];
	uint delta = 0x61C88647, sum = 0xE3779B90;
	for (i = 0; i < num_rounds; i++)
	{
		data1 -= (((data0 << 4) ^ (data0 >> 5)) + data0) ^ (sum + key[(sum >> 11) & 3]);
		sum += delta;
		data0 -= (((data1 << 4) ^ (data1 >> 5)) + data1) ^ (sum + key[sum & 3]);
	}
	data[0] = data0;
	data[1] = data1;
}
