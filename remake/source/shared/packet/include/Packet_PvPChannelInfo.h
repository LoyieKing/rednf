//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_PVPCHANNELINFO_H
#define PACKET_PVPCHANNELINFO_H

#include "PacketHeader.h"

class Packet_PvPChannelInfo : public PacketHeader {
public:
    unsigned int m_charNo;          // +0xa  FindUser_CharNo
    unsigned int m_uid;             // +0xe  Dispatcher_PvPChannelInfo::dispatch_sig: CUser::GetUID
    unsigned int m_schoolNo;        // +0x12  dispatch_sig: CUser::GetSchoolNo
    unsigned char m_channelCount;   // +0x16  inbound request
    unsigned char m_count;          // +0x17  reply
    // +0x18：0x17e 个 STPvPChannelInfo（0x10 字节/项，合计 0x17e0 字节，逐字节同旧 char[0x17e0]）。
    // [SharedHdr-Monitor 2026-09-15 · Main 裁决(b)] 机理 E′：本成员原为 char[0x17e0] 占位，
    //   调用点（Monitor CPacketTranslater::OnPvPChannelUserCount）只能写
    //   `(STPvPChannelInfo*)reply.m_channels` ⇒ GCC 把两个实参**预计算**（先算 arg2 再 arg3、
    //   同时占用 eax/edx）✗；ORIG 为逐条 compute→store（先 arg3 store 再 arg2 store）✓。
    //   给出真实元素类型后调用点直取 `reply.m_channels`（数组 decay，无 cast 节点）⇒ 复现 ORIG
    //   形态（mdiff 6→0 判 AE ✓）。类型提供：**仅** Monitor 的 DNFPacketTranslater.cpp 在 include
    //   本头前先 include Monitor/DNFChannelWaitingUser.h 并定义 PVPINFO_REAL_SLOTS；
    //   其余 TU（默认分支）保持 char[0x17e0] ⇒ 目标码逐字节不变。属**临时**的类型提供方式，
    //   待上游把 STPvPChannelInfo 统一到共享层后即可去掉本分支与调用点的 define/include。
    //   （未直接移动类型定义：Game 侧另有同名语义类型，源码级 ODR 风险会进入不可重链的 game。）
#if defined(PVPINFO_REAL_SLOTS)
    STPvPChannelInfo m_channels[0x17e];
#else
    char m_channels[0x17e0];
#endif
    Packet_PvPChannelInfo() : PacketHeader(0xfa2, 0x17f8){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_PvPChannelInfo, 0x17f8);

#endif  // PACKET_PVPCHANNELINFO_H
