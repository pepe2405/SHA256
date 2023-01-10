#include <iostream>

using namespace std;

const uint32_t h[8] = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };
const uint64_t k[64] = { 0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
						0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
						0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
						0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
						0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
						0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
						0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
						0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 };

//Common methods needed
//get length of a string
int len(const char* string)
{
	int length = 0;
	for (int i = 0; i < string[i] != '\0'; i++)
	{
		length++;
	}
	return length;
}

//concat the second value with the first one from given index
void concat(int* first, const int currentIndex, int* second, int secondLen)
{
	for (int i = 0; i < secondLen; i++)
	{
		first[currentIndex + i] = second[i];
	}
}

void convertLastLineToAppend(int n, int* binary)
{
	for (int i = 63; i >= 0; i--)
	{
		binary[i] = n % 2;
		n /= 2;
	}
}

//Methods converting Word to Binary
void convertLetterToBinary(const char letter, int* binary)
{
	int charNum = letter;
	for (int i = 7; i >= 0; i--)
	{
		binary[i] = charNum % 2;
		charNum /= 2;
	}
}

void convertWordToBinary(const char* word, int* binaryArray)
{
	const int wordLength = len(word);
	const int binaryLength = 8 * wordLength;
	
	for (int i = 0; i < wordLength; i++)
	{
		int* letterBinary = new int[8]{};
		convertLetterToBinary(word[i], letterBinary);

		const int currentIndex = i * 8;
		concat(binaryArray, currentIndex, letterBinary, 8);
		
		delete[] letterBinary;
	}
}

//Rearrange the big chunk into 64 smaller ones
void convertStringTo32BitChunks(int* string, uint32_t* chunkWords)
{
	for (int i = 0; i < 16; i++)
	{
		int start = i * 32;
		int end = i * 32 + 32 - 1;
		int n = 0;
		int adder = 1;
		for (int j = end; j >= start; j--)
		{
			n += (string[j] * adder);
			adder *= 2;
		}
		chunkWords[i] = n;
	}
}

//Bit functions
uint32_t sigma1(uint32_t x)
{
	uint32_t rotateRight17 = (x >> 17) | (x << 15);
	uint32_t rotateRight19 = (x >> 19) | (x << 13);
	uint32_t shiftRight10 = (x >> 10);

	return rotateRight17 ^ rotateRight19 ^ shiftRight10;
}

uint32_t sigma0(uint32_t x)
{
	uint32_t rotateRight7 = (x >> 7) | (x << 25);
	uint32_t rotateRight18 = (x >> 18) | (x << 14);
	uint32_t shiftRight3 = (x >> 3);

	return rotateRight7 ^ rotateRight18 ^ shiftRight3;
}

uint32_t change(uint32_t x, uint32_t y, uint32_t z)
{
	return (x & y) ^ ((~x) & z);
}

uint32_t bigSigma1(uint32_t x)
{
	uint32_t rotateRight6 = (x >> 6) | (x << 26);
	uint32_t rotateRight11 = (x >> 11) | (x << 21);
	uint32_t rotateRight25 = (x >> 25) | (x << 7);

	return rotateRight6 ^ rotateRight11 ^ rotateRight25;
}

uint32_t bigSigma0(uint32_t x)
{
	uint32_t rotateRight2 = (x >> 2) | (x << 30);
	uint32_t rotateRight13 = (x >> 13) | (x << 19);
	uint32_t rotateRight22 = (x >> 22) | (x << 10);

	return rotateRight2 ^ rotateRight13 ^ rotateRight22;
}

uint32_t major(uint32_t x, uint32_t y, uint32_t z)
{
	return (x & y) ^ (x & z) ^ (x & z);
}

int main()
{
	int zerosArray[10000]{};
	//Creating the first binary block (only the string in binary)
	char word[10000];
	cin.getline(word, 10000);
	const int wordLength = len(word);
	const int binaryLength = wordLength * 8;
	int* firstBinary = new int[binaryLength + 1]{};
	convertWordToBinary(word, firstBinary);
	
	//Creating the second binary block
	const int l = binaryLength;
	int k = 0;

	while((l + k + 1 + 64) % 512 != 0)
	{
		k++;
	}
	int newLength = l + k + 1 + 64;

	//Appending the zeros and the last line
	int* secondBinaryBlock = new int[newLength] {};
	concat(secondBinaryBlock, 0, firstBinary, l);
	secondBinaryBlock[l] = 1;
	
	int binaryToAppend[64]{};
	convertLastLineToAppend(binaryLength, binaryToAppend);
	concat(secondBinaryBlock, newLength - 64, binaryToAppend, 64);
	
	//Now we have the %512 length block
	const int chunkNumbers = newLength / 512;

	//For each chunk
	for (int i = 1; i <= chunkNumbers; i++)
	{
		//Creating the 64 array of 32 bits
		uint32_t chunks[64]{};
		convertStringTo32BitChunks(secondBinaryBlock, chunks);

	}

	delete[] firstBinary;

}