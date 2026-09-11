bool WindowsDataFile::LoadWindowsDataFile(const char* path)  // 0x08574e80
{
    // ORIG 0x08574e80 尾部（+0x20..+0x37）为 `call LoadClientFile; xor $1,%eax;
    // test %al,%al; je L; mov $0,%eax; jmp end; L: mov $1,%eax; leave; ret` ——
    // bool 归一化走**取反短路物化 + 常量分支岛**（P32 xor$1 族）。裸
    // `return f(...)` 被 4.4 折成 call 后直接 ret（−6 insns），三目
    // `return r ? true : false` 只得 test/je 缺 xor（−2）；本形 20/20 逐条全等。
    if (!LoadClientFile(&m_size,
                        reinterpret_cast<void**>(&m_data), path))
    {
        return false;
    }
    return true;
}