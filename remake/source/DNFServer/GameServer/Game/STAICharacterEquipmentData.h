// HdrCreate3: ORIG exported symbols, first .h declaration.
// Evidence: nm ORIG + 复制定义（has_def=1，源 CharacterAIA2.cpp:217-223）
#ifndef DNF_GAME_STAICHARACTEREQUIPMENTDATA_H_
#define DNF_GAME_STAICHARACTEREQUIPMENTDATA_H_

// ORIG W 0x088ceaf2 _ZN25STAICharacterEquipmentDataD1Ev（nm 仅析构；
// push_back 步长 0xc，见 W 0x088cc8cc 区）。
// 注意：CharacterAIA2.cpp:217 已有同构定义（未删除）——引入本头的 TU 需避免
// 重复定义；cpp 侧清理由宿主统一安排（冲突已记录）。
// 分歧提示：ORIG 存在非平凡 dtor 符号，而当前复刻布局（3 个 int）析构平凡；
// 若 ORIG 含 vector 成员则布局有差异，待后续报告核实【推断】。
struct STAICharacterEquipmentData
{
    int m_a;  // ScanInt 第 1 值
    int m_b;  // ScanInt 第 2 值
    int m_c;  // ScanInt 第 3 值
};
#if defined(STAICHARACTEREQUIPMENTDATA_ASSERT_SIZE)
typedef char assert_stai_equipmentdata_size[(sizeof(STAICharacterEquipmentData) == 0xc) ? 1 : -1];
#endif

