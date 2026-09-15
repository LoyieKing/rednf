//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_FRAME_LAG_STATISTIC_ADD_H
#define PACKET_FRAME_LAG_STATISTIC_ADD_H

#include "PacketHeader.h"

// [2026-09-15 机理 E/E′] 原先本头是「空类（仅 ctor）」，statics 侧只能靠
//   `((FrameLagView*)pkt)->...` 视图 cast 访问，而**内联 cast 会强制 base 先载入**
//   （隔离实验 g1/g4：`mov 0xc(%ebp),%ecx` 先于 `mov -0xc(%ebp),%eax`），
//   与 ORIG 的 index-先（`is_valid_statistic_packet` 7 处、`PushOneFrameLagData`、
//   `accFrameLagStruct`）相反。此处按 ORIG 反汇编逐项补真实字段 ⇒ 后续可零 cast 直访。
//
// 布局（ORIG `is_valid_statistic_packet` 0x8067d94 / `PushOneFrameLagData` 0x8067xxx /
//   ctor PacketHeader(0xc26, 0x186) 实证）：
//   +0x0a..+0x1f : 头字段（m_directx +0x1b 奇偏移、m_module +0x1f）
//   +0x0c        : sub 区（{int,float} 步长 8，索引 i*7 + (j+4)）
//   +0x20        : items[6]（步长 0x38，item 内 v0..v3 位于 +0x04..+0x0b）
//   +0x174/+0x17a: used memory 上报 char[6] + short[6]（PushOne 尾区）
//   m_min(+0x0a int) 与 sub 区(+0x0c) 交叠、items 区(+0x20) 与 sub 区尾部交叠
//   ⇒ 用同一 union 的两个成员表达（wire 本就是同一段字节的多重解读）。
//
// [2026-09-15 实测记录] `is_valid_statistic_packet` 残余的 items 基址常量拆分
//   （ORIG `add $0x20,%eax; movzwl 0x4(%eax)` vs 我方 `add $0x10,%eax; movzwl 0x14(%eax)`）
//   经微实验证实：仅当 items 是「无 union 祖先的类直接成员」时才得 0x20+0x4；匿名 union
//   与具名 union 一律折成 0x10+0x14。items(+0x20..0x170) 与 sub(+0x0c..0x15c) 同址重叠
//   ⇒ 二者不可兼得，且保 items 会牺牲 sub 区零 cast（载入序回转）⇒ 维持现状。
class Packet_Frame_Lag_Statistic_Add : public PacketHeader {
public:
    struct SubItem
    {
        int m_i;     // +0x00
        float m_f;   // +0x04
    } __attribute__((packed));

    struct Item
    {
        short m_s0;          // +0x00（类 +0x20，PushOne 的 m_sum1 同址）
        short m_s1;          // +0x02（类 +0x22，m_sum2 同址）
        short m_v0;          // +0x04
        short m_v1;          // +0x06
        short m_v2;          // +0x08
        short m_v3;          // +0x0a
        // [2026-09-15 W10-Static] sub 视图（8 字节 {int,float} 元素）直接落在 item+0x0c
        //   ⇒ `m_rest[j]` 地址 = 0x2c+56i+8j，与 is_valid 原 `u_sub.m_sub[i][j][4]`
        //   （0x0c+(i*7+j+4)*8）逐字节一致。
        SubItem m_rest[5];   // +0x0c（40 字节；j=5 时地址 = item+0x34，仍在 0x38 内）
        char m_pad34[4];     // +0x34..+0x37（补足 0x38）
    } __attribute__((packed));

    union Layout
    {
        // [2026-09-15 W10-Static] 原 u_sub 成员已删（唯一使用点 is_valid 改用
        //   `m_items[i].m_rest[j]`）；items 移出 union 成为类直接成员 ⇒ 复现 ORIG 的
        //   `add $0x20,%eax; movzwl 0x4(%eax)` 拆分（带 union 祖先时折成 0x10+0x14）。
        struct
        {
            int m_min;                      // 类 +0x0a（cpu clock）
            char m_field_e;                 // +0x0e（cpu_processor_num）
            char m_field_f;                 // +0x0f（cpu_vendor）
            char m_pad10[2];                // +0x10 驻留（wire 段间空档）
            unsigned short m_field_12;      // +0x12（videocard_vendor 低 16）
            unsigned short m_field_14;      // +0x14
            unsigned short m_field_16;      // +0x16
            unsigned short m_field_18;      // +0x18（ram）
            char m_field_1a;                // +0x1a（os_version）
            unsigned int m_directx;         // +0x1b（奇偏移，packed）
            char m_module;                  // +0x1f
        } __attribute__((packed)) u_main;
    } __attribute__((packed));

    Layout m_layout;        // 类 +0x0a 起（union 大小 0x16 ⇒ 至 +0x1f）
    Item m_items[6];        // +0x20，步长 0x38（类直接成员）
    char m_tail[4];         // +0x170..0x173
    char m_used[6];         // +0x174（used memory 上报）
    short m_val[6];         // +0x17a..0x185

    Packet_Frame_Lag_Statistic_Add() : PacketHeader(0xc26, 0x186){};
} __attribute__((packed));

#endif  // PACKET_FRAME_LAG_STATISTIC_ADD_H
