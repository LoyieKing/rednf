// 布局（由 C1 反汇编推得）：float m_arr[22] @+0x0（循环 i<=0x15 逐项清 0，
// 0.0f 赋值产生 mov $0,%ecx 中转——int 数组直写立即数，二者指令形态不同）、
// int m_field58 @+0x58（movl $0 直写，非 float）、
// std::vector<STResultItemCnt> m_vec @+0x5c（C1 先构造再 clear）。
#ifndef DNF_GAME_ST3RDCHRONICLEDESTRUCTIONRESULT_H_
#define DNF_GAME_ST3RDCHRONICLEDESTRUCTIONRESULT_H_

#include <vector>

#include "EquipmentParameterInfo.h"  // STResultItemCnt

class ST3rdChronicleDestructionResult
{
public:
    ST3rdChronicleDestructionResult()   // ORIG W 0x0891a8e0
    {
        for (int i = 0; i <= 0x15; ++i)
            m_arr[i] = 0.0f;
        m_field58 = 0;
        m_vec.clear();
    }
    ~ST3rdChronicleDestructionResult() {}  // ORIG W 0x0854b3a4（仅成员析构）

    // 字段直访（batch-14 访问器专项）：原 4 个类内 inline 访问器已删 ——
    // c6root gcc 4.4.7 `-O0` 对类内定义的 inline 成员函数**照样发 call**，而
    // ORIG importUpgradeTable @0x08900c26 是 `mov 0x48(%eax)` / `lea 0x490(%eax)`
    // 帧内直访（实测每访问点 +1 call）。偏移与 sizeof 零改动。
    float m_arr[22];                            // +0x00..+0x57
    int m_field58;                              // +0x58
    std::vector<STResultItemCnt> m_vec;         // +0x5c
};

#endif  // DNF_GAME_ST3RDCHRONICLEDESTRUCTIONRESULT_H_
