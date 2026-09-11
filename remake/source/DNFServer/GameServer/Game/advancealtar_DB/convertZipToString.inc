// [objdump 0x813dc48] b28d-26: AE。修法=两处无符号比较取「否定早返」正向形
//   （zipCap <= zlen / strCap <= rawLen）+ zlen 同型化 unsigned（P209 姊妹：4.4 对
//   unsigned 形参比较以形参为 dst 直发 ja）。ORIG 0x813dc73 / 0x813dcda 实证。
bool convertZipToString(MySQL* mysql, int col, char* zipBuf, unsigned int zipCap,
                        char* strBuf, unsigned int strCap)
{
    // Ghidra 侧 local_14 为 uint（ORIG 用 ja 无符号序），同型化让 4.4 以形参为 dst。
    const unsigned int zlen = mysql->get_binary_length(col);
    if (zlen == 0)
        return true;
    // ORIG 0x813dc73：`mov 0x14(%ebp),%eax; cmp -0x10(%ebp),%eax; ja <body>`
    // = 早返 `if (zipCap <= (unsigned)zlen) return false;`（!C 取反后 GCC 直接发 ja）。
    if (zipCap <= zlen)
        return false;
    std::memset(zipBuf, 0, zipCap);
    if (!mysql->get_binary(col, zipBuf, static_cast<int>(zlen)))
        return false;
    const unsigned int rawLen = (*reinterpret_cast<unsigned int*>(zipBuf)) & 0x3fffffff;
    // ORIG 0x813dcda：`mov 0x1c(%ebp),%eax; cmp -0xc(%ebp),%eax; ja <body>`（同形）。
    if (strCap <= rawLen)
        return false;
    std::memset(strBuf, 0, strCap);
    unsigned int len = rawLen;
    // P168a①：bool callee 裸负形 = ORIG `call; xor$1,%eax; test%al; je` choose。
    if (!uncompress_zip(strBuf, len, zipBuf, zlen))
        return false;
    return true;
}