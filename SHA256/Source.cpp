#include <iostream>

using namespace std;

uint32_t H[8] = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };
uint32_t k[64] = { 0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
						0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
						0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
						0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
						0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
						0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
						0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
						0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 };

//Bit functions
uint32_t sigma1(uint32_t x)
{
	const uint32_t rotateRight17 = (x >> 17) | (x << 15);
	const uint32_t rotateRight19 = (x >> 19) | (x << 13);
	const uint32_t shiftRight10 = (x >> 10);

	return rotateRight17 ^ rotateRight19 ^ shiftRight10;
}

uint32_t sigma0(uint32_t x)
{
	const uint32_t rotateRight7 = (x >> 7) | (x << 25);
	const uint32_t rotateRight18 = (x >> 18) | (x << 14);
	const uint32_t shiftRight3 = (x >> 3);

	return rotateRight7 ^ rotateRight18 ^ shiftRight3;
}

uint32_t bigSigma1(uint32_t x)
{
	const uint32_t rotateRight6 = (x >> 6) | (x << 26);
	const uint32_t rotateRight11 = (x >> 11) | (x << 21);
	const uint32_t rotateRight25 = (x >> 25) | (x << 7);

	return rotateRight6 ^ rotateRight11 ^ rotateRight25;
}

uint32_t bigSigma0(uint32_t x)
{
	const uint32_t rotateRight2 = (x >> 2) | (x << 30);
	const uint32_t rotateRight13 = (x >> 13) | (x << 19);
	const uint32_t rotateRight22 = (x >> 22) | (x << 10);

	return rotateRight2 ^ rotateRight13 ^ rotateRight22;
}

uint32_t major(const uint32_t a, const uint32_t b, const uint32_t c)
{
	return (a & b) ^ (a & c) ^ (b & c);
}

uint32_t change(const uint32_t a, const uint32_t b, const uint32_t c)
{
	return (a & b) ^ ((~a) & c);
}

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

void compressionAndModifyFinal(const uint32_t* w)
{
	uint32_t a = H[0];
	uint32_t b = H[1];
	uint32_t c = H[2];
	uint32_t d = H[3];
	uint32_t e = H[4];
	uint32_t f = H[5];
	uint32_t g = H[6];
	uint32_t h = H[7];
	//Compression loop
	for (int i = 0; i < 64; i++)
	{
		uint32_t S1 = bigSigma1(e);
		uint32_t ch = change(e, f, g);
		uint32_t temp1 = h + S1 + ch + k[i] + w[i];
		uint32_t S0 = bigSigma0(a);
		uint32_t maj = major(a, b, c);
		uint32_t temp2 = S0 + maj;
		h = g;
		g = f;
		f = e;
		e = d + temp1;
		d = c;
		c = b;
		b = a;
		a = temp1 + temp2;

		
	}

	//Modifying final values
	H[0] = H[0] + a;
	H[1] = H[1] + b;
	H[2] = H[2] + c;
	H[3] = H[3] + d;
	H[4] = H[4] + e;
	H[5] = H[5] + f;
	H[6] = H[6] + g;
	H[7] = H[7] + h;
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
	const int newLength = l + k + 1 + 64;

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
	for (int i = 0; i < chunkNumbers; i++)
	{
		//Creating the 64 array of 32 bits
		uint32_t w[64]{};
		convertStringTo32BitChunks(secondBinaryBlock, w);

		//Message schedule
		createMessageSchedule(w);

		//Compression
		compressionAndModifyFinal(w);

		
		
		
	}

	delete[] firstBinary;
}