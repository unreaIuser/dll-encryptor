#include "EncryptedStreamer.h"
//guidedhacking.com

//This is the main app which will encrypt your DLL for you, decrypt it and test downloading & decrypting
int wmain(int argc, wchar_t* argv[])
{
	std::wcout << L"GH Encrypted DLL Streamer Example\n\n";
	std::wcout << L"This program has 3 modes:\n1) You drop a DLL on it, and it splits it into 4 encrypted parts\n2) You drop the first encrypted file \"1\" and it rebuilds the files to the original DLL\n3) You don't drop any file, you just run it, and it will decrypt the encrypted stream from the server\n";

	//Get current directory
	fs::path currDir = fs::path(argv[0]).remove_filename();

	fs::path dropped = "";

	//If no file was dragged and dropped, then assume we're in debug mode and provide the correct path below
	if (!argv[1])
	{
		#ifdef _DEBUG
				dropped = L"F:\\original.dll";
				//dropped = "F:\\1";
		#else
		{
			std::wcout << L"\nNo file was dropped, type the letter 'd' and hit enter to test download & decryption\n";

			wchar_t input;

			std::wcin >> input;

			if (_wcsicmp(&input, L"d"))
			{
				TestDownloadAndDecryption(currDir);

				std::wcout << "File downloaded and decrypted to " << currDir / L"original-rebuilt.dll" << L"\n Hit enter to exit\n";
				std::getwchar();
				return 0;
			}

			else
			{
				std::wcout << L"you done fucked up m8\n";
				std::getwchar();
				return 1;
			}
		}
#endif
	}

	//get path from drag and drop
	else dropped = fs::path(argv[1]);

	//if file is the first encrypted file, decrypt them and rebuild the original DLL
	if (dropped.filename() == "1")
	{
		DecryptLocalFiles(dropped);
		std::wcout << L"DLL Decrypted & Regenerated\nHit enter to exit\n";
		std::getwchar();
		return 0;
	}

	//if file is a DLL, encrypt it
	else if (dropped.has_extension() || dropped.extension() == ".dll")
	{
		Encrypt(dropped);
		std::wcout << L"DLL Split & Encrypted\nHit enter to exit\n";
		std::getwchar();
		return 0;
	}

	//if file is not a DLL
	else if (dropped.has_extension() || dropped.extension() != ".dll")
	{
		std::wcout << L"Not a DLL file, you must drag and drop a .dll file to encrypt\nHit enter to exit\n";
		std::getwchar();
		return 1;
	}

	else
	{
		std::wcout << L"you done really fucked up m8\n";

		std::getwchar();
		return 1;
	}
}