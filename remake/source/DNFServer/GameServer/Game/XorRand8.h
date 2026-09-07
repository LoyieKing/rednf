// XorRand8：64 位 xorshift 风格字节替换表 RNG。ORIG 符号见
// docs/class_func_reports/XorRand8/（0x08adfc92 起 12 个函数；ms_table0..7
// 为类静态 const 成员，VA 0x08e33cc0..0x08e37cc0，各 256 x uint64，
// 数据自 ORIG .rodata 提取）。
#ifndef DNF_GAME_XORRAND8_H_
#define DNF_GAME_XORRAND8_H_

class XorRand8
{
public:
    XorRand8();
    XorRand8(unsigned long long seed);
    void seed(unsigned long long seed);    // _ZN8XorRand84seedEy
    unsigned long long getUInt64();        // _ZN8XorRand89getUInt64Ev
    long long getInt64();                  // _ZN8XorRand88getInt64Ev
    unsigned long long operator()();       // _ZN8XorRand8clEv
    unsigned long long min() const;        // _ZNK8XorRand83minEv
    unsigned long long max() const;        // _ZNK8XorRand83maxEv
    static long long getStaticInt64(unsigned long long seed);        // _ZN8XorRand814getStaticInt64Ey
    // ORIG W（0x08adfec6）：类内 inline，调用方帧 0x28/参槽 -0x10/-0xc 依此形
    //（NL26 probeH 同机制实证：callee 类外定义时 4.4 调用方帧塌缩）。
    static unsigned long long getStaticUInt64(unsigned long long seed)
    {
        unsigned char* c = (unsigned char*)&seed;
        return ms_table0[c[0]] ^ ms_table1[c[1]] ^ ms_table2[c[2]] ^
               ms_table3[c[3]] ^ ms_table4[c[4]] ^ ms_table5[c[5]] ^
               ms_table6[c[6]] ^ ms_table7[c[7]];
    }  // _ZN8XorRand815getStaticUInt64Ey
    static long long getStaticInt64();     // _ZN8XorRand814getStaticInt64Ev
    static unsigned long long getStaticUInt64();  // _ZN8XorRand815getStaticUInt64Ev
private:
    unsigned long long m_counter;          // +0x00（低 4 字节 +0，高 4 字节 +4）
    static const unsigned long long ms_table0[256];  // VA 0x08e33cc0
    static const unsigned long long ms_table1[256];  // VA 0x08e344c0
    static const unsigned long long ms_table2[256];  // VA 0x08e34cc0
    static const unsigned long long ms_table3[256];  // VA 0x08e354c0
    static const unsigned long long ms_table4[256];  // VA 0x08e35cc0
    static const unsigned long long ms_table5[256];  // VA 0x08e364c0
    static const unsigned long long ms_table6[256];  // VA 0x08e36cc0
    static const unsigned long long ms_table7[256];  // VA 0x08e374c0
};

#endif  // DNF_GAME_XORRAND8_H_
