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

    // 内联访问器（不产生符号）：+0x58 字段（upgrade_table_t 整体偏移 0x48c，
    // importUpgradeTable 0x08900c26 直写实证）。
    int getField58() const { return m_field58; }
    void setField58(int v) { m_field58 = v; }
    // m_vec（upgrade_table_t 整体偏移 0x490 = 旧 m_resultItems，importUpgradeTable
    // 0x08900c26 [destruction result] 段直访实证）。
    std::vector<STResultItemCnt>& getVec() { return m_vec; }
    const std::vector<STResultItemCnt>& getVec() const { return m_vec; }

private:
    float m_arr[22];                            // +0x00..+0x57
    int m_field58;                              // +0x58
    std::vector<STResultItemCnt> m_vec;         // +0x5c
};

#endif  // DNF_GAME_ST3RDCHRONICLEDESTRUCTIONRESULT_H_
