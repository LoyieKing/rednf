// importFromFile @ 0x8acfd00
bool ScriptStringManager::importFromFile(const char* fileName)
{
    std::FILE* f = std::fopen(fileName, "rb");  // 8acfd1d（mode "rb"）
    if (f == 0)
        return false;
    if (std::fseek(f, 0, SEEK_END) != 0)        // 8acfd46
    {
        std::fclose(f);
        return false;
    }
    {
        long size = std::ftell(f);              // 8acfd51
        if (size < 0)
        {
            std::fclose(f);
            return false;
        }
        if (std::fseek(f, 0, SEEK_SET) != 0)    // 8acfd76
        {
            std::fclose(f);
            return false;
        }
        {
            unsigned char* buf = new unsigned char[size];  // 8acfd7b _Znaj
            if (buf == 0)
            {
                std::fclose(f);
                return false;
            }
            std::fread(buf, size, 1, f);        // 8acfd99（ORIG 忽略返回值）
            std::fclose(f);                     // 8acfda1
            importFromBuffer(buf, (int)size);   // 8acfdb4（ORIG 忽略返回值）
            delete[] buf;                       // 8acfdbc _ZdaPv
            return true;                        // 8acfdc1：无条件 true
        }
    }
}