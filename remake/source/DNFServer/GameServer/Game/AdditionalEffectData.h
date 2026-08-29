// AdditionalEffectData —— ORIG 域（df_game_r W 符号转写）。
// 布局证据：objdump C1 0x0891b05e / C1ERKSs 0x08929074 / op= 0x0891c754 /
// D1 0x0891c73e（报告缺失，按 nm 定址转写）。ORIG 无 vtable/typeinfo、
// D1 无 vptr 重置 → 非虚用户析构（范式 P39）。sizeof 0x30（+0x2c 字节 + 3 pad）。
// 字段语义【推断】：+0x10=2 疑似段数/模式常量，+0x18/+0x1c=-1 疑似区间，
// 无 ORIG getter/setter 可佐证，命名保守。
#ifndef DNF_GAME_ADDITIONALEFFECTDATA_H_
#define DNF_GAME_ADDITIONALEFFECTDATA_H_

#include <string>
#include <vector>

class AdditionalEffectData
{
public:
    AdditionalEffectData();                              // ORIG C1 0x0891b05e
    AdditionalEffectData(AdditionalEffectData const&);   // ORIG C1ERKSs 0x08929074
    ~AdditionalEffectData();                             // ORIG D1 0x0891c73e（非虚）
    AdditionalEffectData& operator=(AdditionalEffectData const&);  // ORIG 0x0891c754（返回 this）

    int m_id;                      // +0x00 C1 置 -1
    std::vector<std::string> m_strings;   // +0x04（C1 构造后 clear）
    int m_mode;                    // +0x10 C1 置 2
    int m_count;                   // +0x14 C1 置 0
    int m_min;                     // +0x18 C1 置 -1
    int m_max;                     // +0x1c C1 置 -1
    int m_valueA;                  // +0x20 C1 置 0
    int m_valueB;                  // +0x24 C1 置 0
    int m_valueC;                  // +0x28 C1 置 0
    unsigned char m_flag;          // +0x2c C1 置 0
};

#endif // DNF_GAME_ADDITIONALEFFECTDATA_H_
