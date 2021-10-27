#pragma once
#include <Windows.h>
#include <Urlmon.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include "blowfish\blowfish.h"
//guidedhacking.com
#pragma comment( lib, "Urlmon.lib" )

namespace fs = std::filesystem;

bool replace(std::string& str, const std::string& from, const std::string& to);

struct EncryptedData_t
{
	std::string a;
	std::string b;
	std::string c;
	std::string d;
};

void Encrypt(fs::path dropped);

std::string Decrypt(EncryptedData_t encryptedData);

void DecryptLocalFiles(fs::path dropped);

int TestDownloadAndDecryption(fs::path currDir);

std::string StreamFileToMemString(std::wstring URL);

std::string GetDecryptedDLL();