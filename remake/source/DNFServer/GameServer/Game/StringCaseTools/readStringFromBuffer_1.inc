void readStringFromBuffer(std::string& str, char*& buf)
{
    // [b28e-L5] 同上（ORIG 此处 `mov %si,0x9501a1a`）。
    const unsigned short n = readWordFromBuffer(buf);
    str.resize(n + 1, 0);
    str.assign(buf, n);
    buf += n;
