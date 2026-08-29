// XorRand4：32 位 xorshift 风格字节替换表 RNG。ORIG 符号见
// docs/class_func_reports/XorRand4/（0x08adfa90 起 12 个函数；ms_table0..3
// 为类静态 const 成员，VA 0x08e32cc0/0x08e330c0/0x08e334c0/0x08e338c0，
// 各 256 x uint32，数据自 ORIG .rodata 提取）。
#ifndef DNF_GAME_XORRAND4_H_
#define DNF_GAME_XORRAND4_H_

class XorRand4
{
public:
    XorRand4();
    XorRand4(unsigned int seed);
    void seed(unsigned int seed);          // _ZN8XorRand44seedEj
    unsigned int getUInt32();              // _ZN8XorRand49getUInt32Ev
    int getInt32();                        // _ZN8XorRand48getInt32Ev
    unsigned int operator()();             // _ZN8XorRand4clEv
    unsigned int min() const;              // _ZNK8XorRand43minEv
    unsigned int max() const;              // _ZNK8XorRand43maxEv
    static int getStaticInt32(unsigned int seed);        // _ZN8XorRand414getStaticInt32Ej
    static unsigned int getStaticUInt32(unsigned int seed);  // _ZN8XorRand415getStaticUInt32Ej
    static int getStaticInt32();           // _ZN8XorRand414getStaticInt32Ev
    static unsigned int getStaticUInt32(); // _ZN8XorRand415getStaticUInt32Ev
private:
    unsigned int m_counter;                // +0x00
    static const unsigned int ms_table0[256];  // VA 0x08e32cc0
    static const unsigned int ms_table1[256];  // VA 0x08e330c0
    static const unsigned int ms_table2[256];  // VA 0x08e334c0
    static const unsigned int ms_table3[256];  // VA 0x08e338c0
};

#endif  // DNF_GAME_XORRAND4_H_
