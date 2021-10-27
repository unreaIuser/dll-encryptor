#include <windows.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <WinInet.h>
#pragma comment(lib, "Wininet.lib")
//guidedhacking.com
#include "blowfish\blowfish.h"
#include "EncryptedStreamer.h"

//init blowfish encryption using random key
//ideally you would use 4 different types of encryption instead of the same one
BLOWFISH blowfish1("0096442170857917");
BLOWFISH blowfish2("1460853456758055");
BLOWFISH blowfish3("7798751519739505");
BLOWFISH blowfish4("1238150549789312");

bool replace(std::string& str, const std::string& from, const std::string& to)
{
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

void Encrypt(fs::path dropped)
{
	//open the file stream
	std::ifstream ifs(dropped.c_str(), std::ios::binary | std::ios::ate);

	//get the file size
	int fileSize = ifs.tellg();
	ifs.seekg(0, std::ios::beg);

	std::string data = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

	size_t partSize = (fileSize) / 4;
	size_t remainder = (fileSize) % 4;
	size_t lastPartSize = partSize - remainder;

	std::string part1Buffer(data.begin() + partSize * 0, data.begin() + partSize * 0 + partSize);
	std::string part2Buffer(data.begin() + partSize * 1, data.begin() + partSize * 1 + partSize);
	std::string part3Buffer(data.begin() + partSize * 2, data.begin() + partSize * 2 + partSize);
	std::string part4Buffer(data.begin() + partSize * 3, data.begin() + partSize * 3 + lastPartSize);

	//Ideally you would use 4 different encryption algorithms to annoy reverse engineers, instead of just 1
	std::string part1BufferEncrypted = blowfish1.Encrypt_CBC(part1Buffer);
	std::string part2BufferEncrypted = blowfish2.Encrypt_CBC(part2Buffer);
	std::string part3BufferEncrypted = blowfish3.Encrypt_CBC(part3Buffer);
	std::string part4BufferEncrypted = blowfish4.Encrypt_CBC(part4Buffer);

	//write the encrypted parts to disk for testing
	std::ofstream ofs;
	ofs.open(L"1", std::ios::binary);
	std::copy(part1BufferEncrypted.begin(), part1BufferEncrypted.end(), std::ostream_iterator<char>(ofs));
	ofs.close();
	ofs.open(L"2", std::ios::binary);
	std::copy(part2BufferEncrypted.begin(), part2BufferEncrypted.end(), std::ostream_iterator<char>(ofs));
	ofs.close();
	ofs.open(L"3", std::ios::binary);
	std::copy(part3BufferEncrypted.begin(), part3BufferEncrypted.end(), std::ostream_iterator<char>(ofs));
	ofs.close();
	ofs.open(L"4", std::ios::binary);
	std::copy(part4BufferEncrypted.begin(), part4BufferEncrypted.end(), std::ostream_iterator<char>(ofs));
	ofs.close();
}

void DecryptLocalFiles(fs::path dropped)
{
	fs::path dir = dropped.remove_filename();

	//read each file into a string
	std::ifstream ifs(dir / L"1", std::ios::binary);
	std::string data1 = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
	ifs.close();

	ifs.open(dir / L"2", std::ios::binary);
	std::string data2 = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
	ifs.close();

	ifs.open(dir / L"3", std::ios::binary);
	std::string data3 = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
	ifs.close();

	ifs.open(dir / L"4", std::ios::binary);
	std::string data4 = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
	ifs.close();
	
	//decrypt each part
	std::string part1BufferDecrypted = blowfish1.Decrypt_CBC(data1);
	std::string part2BufferDecrypted = blowfish2.Decrypt_CBC(data2);
	std::string part3BufferDecrypted = blowfish3.Decrypt_CBC(data3);
	std::string part4BufferDecrypted = blowfish4.Decrypt_CBC(data4);

	//Test to make sure it encrypts and decrypts correctly
	//bool result = std::equal(part1Buffer.begin(), part1Buffer.end(), part1BufferDecrypted.begin(), part1BufferDecrypted.end());

	//rebuild the DLL from decrypted data
	std::ofstream ofs;
	ofs.open(L"original-rebuilt.dll", std::ios::binary);
	std::copy(part1BufferDecrypted.begin(), part1BufferDecrypted.end(), std::ostream_iterator<char>(ofs));
	std::copy(part2BufferDecrypted.begin(), part2BufferDecrypted.end(), std::ostream_iterator<char>(ofs));
	std::copy(part3BufferDecrypted.begin(), part3BufferDecrypted.end(), std::ostream_iterator<char>(ofs));
	std::copy(part4BufferDecrypted.begin(), part4BufferDecrypted.end(), std::ostream_iterator<char>(ofs));
	ofs.close();
}

std::string Decrypt(EncryptedData_t encryptedData)
{
	//decrypt each part
	std::string BufferDecrypted = blowfish1.Decrypt_CBC(encryptedData.a);
	BufferDecrypted += blowfish2.Decrypt_CBC(encryptedData.b);
	BufferDecrypted += blowfish3.Decrypt_CBC(encryptedData.c);
	BufferDecrypted += blowfish4.Decrypt_CBC(encryptedData.d);

	//rebuild the DLL from decrypted data
	std::ofstream ofs;
	ofs.open(L"original-rebuilt.dll", std::ios::binary);
	std::copy(BufferDecrypted.begin(), BufferDecrypted.end(), std::ostream_iterator<char>(ofs));
	ofs.close();

	return BufferDecrypted;
}

std::string StreamFileToMemString(std::wstring URL)
{
	const wchar_t* header = L"Accept: *" "/" "*\r\n\r\n";
	HANDLE hInterWebz = InternetOpen(L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
	HANDLE hURL = InternetOpenUrl(hInterWebz, URL.c_str(), header, lstrlen(header), INTERNET_FLAG_DONT_CACHE, 0);

	char* Buffer = new char[100000000]; //100mb
	memset(Buffer, 0, 100000000);
	DWORD BytesRead = 1;

	std::string data;

	if (InternetReadFile(hURL, Buffer, 100000000, &BytesRead))
	{
		data = std::string(Buffer);
	}

	delete[] Buffer;
	InternetCloseHandle(hInterWebz);
	InternetCloseHandle(hURL);

	return data;
}

std::string GetDecryptedDLL()
{
	std::string data1 = StreamFileToMemString(LR"(https://guidedhacking.com/gh/dl/dlltest/1)");
	std::string data2 = StreamFileToMemString(LR"(https://guidedhacking.com/gh/dl/dlltest/2)");
	std::string data3 = StreamFileToMemString(LR"(https://guidedhacking.com/gh/dl/dlltest/3)");
	std::string data4 = StreamFileToMemString(LR"(https://guidedhacking.com/gh/dl/dlltest/4)");

	std::string decryptedDLL = Decrypt({ data1, data2, data3, data4 });

	return decryptedDLL;
}

//This is for testing
int TestDownloadAndDecryption(fs::path currDir)
{
	std::string decryptedDLL = GetDecryptedDLL();

	//Test output to disk
	std::ofstream ofs;
	ofs.open(currDir / L"original-rebuilt.dll", std::ios::binary);
	std::copy(decryptedDLL.begin(), decryptedDLL.end(), std::ostream_iterator<char>(ofs));
	ofs.close();

	std::getwchar();
	return 0;


	//Ensure the DLL bytes are destroyed at this point
}

