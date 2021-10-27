# dll-encryptor


People who make pay hacks typically have down syndrome and are incapable of using their brains in any fashion, and yet these bath salt smoking morons are making pay hacks...Sooner or later when they get close to actually releasing their cheat, they realize "omg I pasted this entire thing, what if someone leaks my DLL, they'll know I'm a retard!"

That is when they then come to GH and ask "how to stream a DLL without hitting disk!?!?"

Well look what we have here, our old friend Senor Paster McGee is back and needs help doing actual development, something he can't paste. Well don't worry folks, Lord Rake has granted you a glimpe into his omniscience with this fresh AF proof of concept that will show you how to stream a DLL, without touching disk, and we'll even slap some juicy encryption on it as well.

This source code shows you how to split the DLL into 4 different files and encrypt each of them using blowfish encryption with seperate keys. Your loader would download these 4 files using InternetReadFile, decrypt them and then combine them into the original DLL bytes as a string, and then manually map it. I have left manual mapping out of this project, you have to figure that part out.

People making payhacks ask how to stream a DLL in C++ all the time, maybe they shouldn't be making pay hacks if they can't solve simple problems like this, smh. So how do you stream a DLL from your web server without downloading it to disk?

Disclaimer: I whipped this up as fast as I could, nothing too special, this is just something I came up with to reduce the chance your DLL gets dumped.

<div align="center">
    <img src="https://guidedhacking.com/attachments/1615003895185-png.13605/"/>
</div>

This is how you stream a DLL into a string, without ever touching disk, simply using InternetReadFile
(obviously it would be best to use something different that isn't a simple WinAPI call, but this is just a PoC)

```cpp
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
```

**This is how we download the 4 encrypted file & decrypt it into a string which represents our streamed DLL**
```cpp
std::string GetDecryptedDLL()
{
    std::string data1 = StreamFileToMemString(LR"(https://guidedhacking.com/gh/dl/dlltest/1)");
    std::string data2 = StreamFileToMemString(LR"(https://guidedhacking.com/gh/dl/dlltest/2)");
    std::string data3 = StreamFileToMemString(LR"(https://guidedhacking.com/gh/dl/dlltest/3)");
    std::string data4 = StreamFileToMemString(LR"(https://guidedhacking.com/gh/dl/dlltest/4)");

    std::string decryptedDLL = Decrypt({ data1, data2, data3, data4 });

    return decryptedDLL;
}
```

**This function shows the blowfish decryption of the streamed DLL:**
```cpp
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
```

**This is showing a test case of downloading, decrypting and saving the file to disk, for testing that it works correctly:**
```cpp
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
}
```
**Now what? How to inject the DLL stream**


```cpp
std::string decryptedDLL = GetDecryptedDLL();
ManualMap(decryptedDll.c_str());
//Ensure the DLL bytes are destroyed at this point
```

This encrypted DLL streaming project is a great starting point for any amateur pay cheat.

Obviously if you were reversing this, you would just dump the argument to the manual mapping function, but if you slapped VMProtect on this and used a few other tricks, it would be annoying enough where most people would give up. Also the flow of execution is very obvious, if you were to do this in stages, sprinkled through the entire execution of your loader, it would be a lot less obvious.

Pro Tip: if you're gonna use this or any other type of encryption, randomize the S boxes and the P array, if you don't SignSearch will detect them and the person analyzing it will instantly know the encryption routine. Most encryption like blowfish will give you this default seed data, and every single implementation you find online will all use the same default seed data, making it trivial to identify with something like SignSearch. (some even use the same key, smh). When you use signsearch and it identifies the encryption, it takes about 15 seconds to find the decryption function. I have already randomized them in my download above.

Also be wary of using cryptopp or other common libraries, they are super easy to identify, some expose RTTI and others have pdbs, then you have TypeLibraries and Lumina servers, making it too easy to identify and reverse them with limited effort.

You can download the entire project below, and that ends my C++ how to stream a DLL tutorial.



< THIS IS A TOTAL REPOST OF https://guidedhacking.com/threads/how-to-stream-a-dll-without-touching-disk-encrypted.16940/ >
