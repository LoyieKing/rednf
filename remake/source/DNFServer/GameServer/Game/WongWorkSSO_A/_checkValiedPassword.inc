bool WongWork::CSimpleSSO::_checkValiedPassword(char* pwd, char const* clientHash,
                                                CRijndael& cipher)
{
    unsigned int srcLen = strlen(pwd);  // ORIG -0x14(%ebp)
    unsigned int paddedLen = 0;         // ORIG -0x10(%ebp)

    char hexBuf[132];  // ORIG -0x98(%ebp)（catch 分支同样读此缓冲，故放 try 外）
    try
    {
        // IMethod::GetBlockSize(this, NULL)（ORIG 0x08602bfe `call 080b4416` **真调用**；
        // 旧形按等价字段访问内联 ⇒ 我方 171 vs ORIG 123）。语义等价（未初始化返 0 /
        // 初始化返 m_blockSize），实现见 DBMW/Method.cpp:72；同域先例 Arad_Misc.cpp
        // 即用 `cipher->IMethod::GetBlockSize(NULL)` 形。
        int blockSize = cipher.IMethod::GetBlockSize(NULL);

        if (blockSize < 0)
        {
            paddedLen = srcLen;
        }
        else if (static_cast<int>(srcLen) % blockSize == 0)
        {
            paddedLen = srcLen;
        }
        else
        {
            paddedLen = (static_cast<int>(srcLen) / blockSize + 1) * blockSize;
        }

        // IMethod::Pad(pwd, len, NULL)（ORIG 0x08602c42 起 `call 080b44a6` **真调用**；
        // 旧形把 Pad 体整体内联）。ORIG 守卫 = `if (-1 < blockSize)`
        //（Ghidra C：`if (-1 < local_10) { IMethod::Pad(…) }`）。
        if (blockSize >= 0)
        {
            cipher.IMethod::Pad(pwd, static_cast<int>(srcLen), NULL);
        }

        unsigned char encBuf[132];  // ORIG -0x120(%ebp) 区（块缓冲 0x84 使用域）
        // ORIG 经 vtable+0xc 虚调用（slot=Encrypt）
        cipher.Encrypt(pwd, reinterpret_cast<char*>(encBuf), paddedLen);

        // ORIG 0x08602cde 段 `mov -0x10(%ebp),%eax; add %eax,%eax; add $0x1,%eax;
        // mov %eax,-0x14(%ebp)` ⇒ **复用 -0x14 槽**（Ghidra C：`local_18 = local_14*2+1`）。
        // 我方旧形另开 `hexLen` 具名局部 ⇒ 本函数全帧槽位整体深 4（P0 律：原码无该局部）。
        srcLen = paddedLen * 2 + 1;
        DNFFLib::Binary2Hex(encBuf, static_cast<int>(paddedLen), hexBuf);
        hexBuf[srcLen] = '\0';

        if (strncmp(hexBuf, clientHash, 0x84) != 0)
        {
            LogManager::logFormat(
                1, "SimpleSSO.cpp",
                "bool WongWork::CSimpleSSO::_checkValiedPassword(char*, const char*, CRijndael&)",
                0xee,
                "_checkValiedPassword() diff password server(%s), client(%s)",
                hexBuf, clientHash);
            return false;
        }
        return true;
    }
    catch (...)
    {
        LogManager::logFormat(
            1, "SimpleSSO.cpp",
            "bool WongWork::CSimpleSSO::_checkValiedPassword(char*, const char*, CRijndael&)",
            0xe7,
            "_checkValiedPassword() exception server(%s), client(%s)", "", clientHash);
        return false;
    }
