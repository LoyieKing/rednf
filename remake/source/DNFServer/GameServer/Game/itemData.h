// ============================================================================
// itemData.h — eventReward 奖励条目数据结构（ORIG 布局实证，独占 TU
// itemData.cpp 提供 ctor/clear/析构）。
//
// ORIG 符号（df_game_r nm，全部 W）：
//   _ZN8itemDataC1Ev / _ZN8itemDataC2Ev   0x080f1134（同址）
//   _ZN8itemData5clearEv                  0x080f1162
//   _ZN8itemDataD1Ev / _ZN8itemDataD2Ev   0x080f32fa（空体，**隐式** trivial 析构；
//     判据：ORIG eventReward::ImportRewardScript 0x080ef3a2 全函数对 6 处
//     `itemData item = curItem;` 局部副本**无任何 D1Ev 调用** ⇒ ORIG 无用户
//     声明析构（用户空析构会强制 -O0 逐点发 call）。P317：删声明后 D1/D2 仍由
//     使用点发射同名 W 符号，符号集不变。）
//
// 布局证据（objdump df_game_r 逐指令）：
//   itemData::C1/clear：+0x00 字节=0、+0x01 字节=1、+0x02 字节=1、
//     +0x04 int=0、+0x08 int=0（每次 store 前重读 this → 函数体赋值形态）；
//   vector<itemData>::push_back（0x080f172a）元素步长 lea 0xc
//     → sizeof(itemData) = 0x0c；
//   eventReward::getOptionType 0x080ef190 / defaultOption 0x080ef25c /
//     applyItemOption 0x080f0d6e 以 movb/movzbl 访问 +0/+1/+2 字节、
//     movl 访问 +4/+8。
// ============================================================================
#ifndef __ITEMDATA_H__
#define __ITEMDATA_H__

struct itemData
{
    signed char m_upgrade;   // +0x00 强化档（>1 → SetUpgrade；低字节 0 → defaultOption 置 1）
    // ORIG applyItemOption 0x080f0d6e 以 movzbl+cmp $1,%al+jle 比较（signed char，
    // unsigned char 生成 jbe）、defaultOption 以 movsbl 读取 → 必须 signed。
    unsigned char m_seal;      // +0x01 seal 标志（getOptionType "seal"/"unseal" 写 1/0）
    // [R19ty T-2a 2026-09-13] `unsigned char`→`bool`：ORIG applyItemOption 0x080f0e06/0x080f0e0a
    //   `movzbl 0x2(%eax),%eax; xor $0x1,%eax; test %al,%al; je`（取反物化，P357 字段轴/
    //   P158）⇒ 字段真型 bool；我方 uchar 版产 `movzbl; test %al,%al; jne`（无 xor，形态差）。
    //   宽度 1B 不变（sizeof 仍 0x0c、+0x04/+0x08 偏移不变）；值域 {0,1}（getOptionType
    //   写 0/1、ctor/clear 写 1）⇒ 语义等价。
    bool m_roi;                // +0x02 sealROI 标志（getOptionType "sealROI"/"unsealROI" 写 1/0）
    int m_itemIdx;             // +0x04 [推断] 道具 idx（ImportRewardScript 行内 ScanInt）
    int m_count;               // +0x08 [推断] 数量（stackable 行 ScanInt；defaultOption action==2 置 1）

    itemData();
    void clear();
};                             // sizeof 0x0c

#endif  // __ITEMDATA_H__
