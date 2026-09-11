// ORIG 0x8a5a86c：bitset → 32 字节（第二参 std::string&，resize(0x20) 后逐字节写）
// [b28d-solo-CMissionList_Charac 定稿] ORIG 0x08a5a8d2 加法段为读-加-写三段
//   `mov %eax,%edx; movzbl byte,%eax; lea (%edx,%eax,1),%eax; mov %al,byte`；
//   4.4-O0 对非 volatile uchar 累加器恒折 `add %al,mem`（P183 折叠；十三形探针
//   /tmp/probe_bs.cpp + /tmp/probe_bs2.cpp 实测）；**累加器加 volatile** 后 4.4
//   逐条复现 ORIG 四段，槽位 i(-0x10)/byte(-0x11)/j(-0xc) 亦全等（f6 探针）。
//   局部 volatile 无并发/信号观察者，语义等价（读-改-写顺序本就如此）。
void BitsetToStr(const std::bitset<256>& bitset, std::string& out)
{
    out.resize(0x20);
    for (int i = 0; i < 32; ++i)
    {
        volatile unsigned char byte = 0;
        for (int j = 0; j < 8; ++j)
            byte = (unsigned char)((bitset[i * 8 + j] << j) + byte);
        out[i] = (char)byte;
    }
}