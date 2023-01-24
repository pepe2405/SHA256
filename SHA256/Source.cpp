#include <iostream>
#include <fstream>

using namespace std;

//CONSTANTS
uint32_t H[8] = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };
uint32_t K[64] = { 0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
						0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
						0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
						0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
						0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
						0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
						0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
						0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 };
uint32_t state[8]{ H[0], H[1], H[2], H[3], H[4], H[5], H[6], H[7] };
const int CHUNK_SIZE = 512;
const int BUFFER_SIZE = 10000;


//BIT FUNCTIONS
uint32_t rotateRight(const uint32_t x, const int amount)
{
	return (x >> amount) | (x << (32 - amount));
}
uint32_t shiftRight(const uint32_t x, const int amount)
{
	return (x >> amount);
}
uint32_t sigma1(const uint32_t x)
{
	return rotateRight(x, 17) ^ rotateRight(x, 19) ^ shiftRight(x, 10);
}
uint32_t sigma0(const uint32_t x)
{
	return rotateRight(x, 7) ^ rotateRight(x, 18) ^ shiftRight(x, 3);
}
uint32_t bigSigma1(const uint32_t x)
{
	return rotateRight(x, 6) ^ rotateRight(x, 11) ^ rotateRight(x, 25);
}
uint32_t bigSigma0(const uint32_t x)
{
	return rotateRight(x, 2) ^ rotateRight(x, 13) ^ rotateRight(x, 22);
}
uint32_t major(const uint32_t a, const uint32_t b, const uint32_t c)
{
	return (a & b) ^ (a & c) ^ (b & c);
}
uint32_t choice(const uint32_t a, const uint32_t b, const uint32_t c)
{
	return (a & b) ^ ((~a) & c);
}

//COMMON METHODS
int fileWork(char* input)
{
	char fileName[300]{};
	cin >> fileName;

	fstream myFile;
	myFile.open(fileName);
	if (!myFile.is_open())
	{
		cout << "Error while opening the file.";
		return 0;
	}

	myFile.getline(input, BUFFER_SIZE);
	myFile.close();
	return 1;
}

void finalFileProcessing(const char* input, const char* finalHexString)
{
	fstream finalFile;
	finalFile.open("result.txt");
	finalFile << input << " =&&= " << finalHexString << endl;
	finalFile.close();
}

//get length of a string
int len(const char* string)
{
	int length = 0;
	while (string[length] != '\0')
	{
		length++;
	}
	return length;
}

void concat(int* first, const int startIndex, const int* second, const int secondLen)
{
	for (int i = 0; i < secondLen; i++)
	{
		first[startIndex + i] = second[i];
	}
}

void concat(char* first, const int startIndex, const char* second, const int secondLen)
{
	for (int i = 0; i < secondLen; i++)
	{
		first[startIndex + i] = second[i];
	}
}

void copyPartOfString(int* first, const int* second, const int startIndex, const int length)
{
	for (int i = 0; i < length; i++)
	{
		first[i] = second[startIndex + i];
	}
}

//Methods converting String to Binary
void convertLetterTo8BitBinary(const char letter, int* binary)
{
	int charNum = letter;
	const int LAST_INDEX = 7;
	for (int i = LAST_INDEX; i >= 0; i--)
	{
		binary[i] = charNum % 2;
		charNum /= 2;
	}
}

void convertWordToBinary(const char* word, int* binaryArray)
{
	const int WORD_LENGTH = len(word);
	
	for (int i = 0; i < WORD_LENGTH; i++)
	{
		int letterBinary[8]{};
		convertLetterTo8BitBinary(word[i], letterBinary);

		const int CURRENT_INDEX = i * 8;
		concat(binaryArray, CURRENT_INDEX, letterBinary, 8);
	}
}

void convertLastLineToAppend(int n, int* binary)
{
	const int LAST_INDEX = 63;
	for (int i = LAST_INDEX; i >= 0; i--)
	{
		binary[i] = n % 2;
		n /= 2;
	}
}

//Rearrange the big chunk into 64 smaller ones
void convertStringTo32BitChunks(const int* string, uint32_t* chunkWords)
{
	for (int i = 0; i < 16; i++)
	{
		const int START = i * 32;
		const int END = i * 32 + 32 - 1;
		int n = 0;
		int adder = 1;
		for (int j = END; j >= START; j--)
		{
			n += (string[j] * adder);
			adder *= 2;
		}
		chunkWords[i] = n;
	}
}

//Chunk functions
void createMessageSchedule(uint32_t* w)
{
	for (int j = 16; j < 64; j++)
	{
		const uint32_t s0 = sigma0(w[j - 15]);
		const uint32_t s1 = sigma1(w[j - 2]);
		w[j] = w[j - 16] + s0 + w[j - 7] + s1;
	}
}

void compress(const uint32_t* w)
{
	//Compression loop
	const int LAST_INDEX = 64;
	for (int i = 0; i < LAST_INDEX; i++)
	{
		const uint32_t S1 = bigSigma1(H[4]);
		const uint32_t ch = choice(H[4], H[5], H[6]);
		const uint32_t temp1 = H[7] + S1 + ch + K[i] + w[i];
		const uint32_t S0 = bigSigma0(H[0]);
		const uint32_t maj = major(H[0], H[1], H[2]);
		const uint32_t temp2 = S0 + maj;
		H[7] = H[6];
		H[6] = H[5];
		H[5] = H[4];
		H[4] = H[3] + temp1;
		H[3] = H[2];
		H[2] = H[1];
		H[1] = H[0];
		H[0] = temp1 + temp2;
	}
}

void modify()
{
	//Modifying final values
	H[0] = H[0] + state[0];
	H[1] = H[1] + state[1];
	H[2] = H[2] + state[2];
	H[3] = H[3] + state[3];
	H[4] = H[4] + state[4];
	H[5] = H[5] + state[5];
	H[6] = H[6] + state[6];
	H[7] = H[7] + state[7];
}

//Initializing binary arrays
void initializeFirstBinary(int* firstBinary, const char* input)
{
	convertWordToBinary(input, firstBinary);
}

void initializeSecondBinary(int* secondBinary, const int* firstBinary, const int binaryLength, const int newLength)
{
	concat(secondBinary, 0, firstBinary, binaryLength);
	secondBinary[binaryLength] = 1;
	int binaryToAppend[64]{};
	convertLastLineToAppend(binaryLength, binaryToAppend);
	concat(secondBinary, newLength - 64, binaryToAppend, 64);
}

//Chunk Method
void chunkLoop(const int chunkCount, const int* secondBinary)
{
	for (int i = 0; i < chunkCount; i++)
	{
		//Creating the 64 array of 32 bits
		uint32_t w[64]{};
		int messageToConvert[CHUNK_SIZE]{};
		const int currentStartIndex = (i + 1) * CHUNK_SIZE;
		copyPartOfString(messageToConvert, secondBinary, i * CHUNK_SIZE, CHUNK_SIZE);
		convertStringTo32BitChunks(messageToConvert, w);

		//Message schedule
		createMessageSchedule(w);

		//Compress and modify
		compress(w);
		modify();
	}
}

//Hex methods
void decToHex(uint32_t n, char* hex)
{
	int index = 7;
	while (n != 0) {
		
		int rem = 0;
		char ch = 0;
		rem = n % 16;
		if (rem < 10) {
			ch = rem + 48;
		}
		else {
			ch = rem + 87;
		}
		hex[index] = ch;
		n = n / 16;
		index--;
	}
}

void createFinalHex(char* finalHex)
{
	char hexString[9]{};
	for (int i = 0; i < 8; i++)
	{
		decToHex(H[i], hexString);
		concat(finalHex, i * 8, hexString, 8);
	}
}

void printHex(const char* hex)
{
	cout << "The has value of the string: " << endl;
	const int length = len(hex);
	for (int i = 0; i < length + 1; i++)
	{
		cout << hex[i];
	}
}

int main()
{
	//User input
	cout << "Enter a file name: " << endl;

	//STEP 1 - PRE PROCESSING
	char input[BUFFER_SIZE];
	if (fileWork(input) == 0)
	{
		return -1;
	}

	//STEP 2 - Creating first binary block
	const int STRING_LENGTH = len(input);
	const int BINARY_LENGTH = STRING_LENGTH * 8;
	int* firstBinary = new int[BINARY_LENGTH + 1]{};
	initializeFirstBinary(firstBinary, input);

	//STEP 3 - Creating the second binary block
	const int remainder = (BINARY_LENGTH + 65) / CHUNK_SIZE;
	const int NEW_LENGTH = (remainder + 1) * CHUNK_SIZE;
	int* secondBinaryBlock = new int[NEW_LENGTH] {};
	initializeSecondBinary(secondBinaryBlock, firstBinary, BINARY_LENGTH, NEW_LENGTH);

	//STEP 4 - CHUNK LOOP
		//STEP 5 - Creating message schedule
		//STEP 6 - Modifying and compressing
	const int CHUNK_COUNT = NEW_LENGTH / CHUNK_SIZE;
	chunkLoop(CHUNK_COUNT, secondBinaryBlock);

	//STEP 7 - Concatenate final hash and print it
	char finalHexString[65]{};
	createFinalHex(finalHexString);
	printHex(finalHexString);

	//Write the complete data
	finalFileProcessing(input, finalHexString);

	delete[] firstBinary;
	delete[] secondBinaryBlock;
}