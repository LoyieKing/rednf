// _Z20readStringFromBufferRSsRPc 0x08ad0460：
// 读 ushort 长度 n（buf 前移 2），string resize(n+1) 后 assign n 字节，buf 前进 n。
// [b28d-28] DIFF | ORIG 0x08ad0460 | 同上 N82/P75：ORIG ebx/esi/edi 缓存 +
// `mov %si,0x9501a1a`（readWordFromBuffer 局部 static，weak 合并）；4.4 槽化
// n（-0xa）+ 逐次重读 0xc(%ebp)。语义等价。
void readStringFromBuffer(std::string& str, char*& buf)
{
    // [b28e-L5] 同上（ORIG 此处 `mov %si,0x9501a1a`）。
    const unsigned short n = readWordFromBuffer(buf);
    str.resize(n + 1, 0);
    str.assign(buf, n);
    buf += n;
}