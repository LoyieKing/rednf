// [R18pkt 2026-09-13] 已落修：DIFF(38/63) → 基类初始化列表（= ORIG 形态）。
//   根因（探针实证）：我方 SigTypes_E.h 镜像 struct 原先未真继承 PacketHeader ⇒ 只能手写
//   `::new (static_cast<void*>(this)) PacketHeader(0x1b66,0x15f)`，4.4 对 placement new 恒发
//   `_ZnwjPv` call + size 装载 + 判空 + EH 清理（`_ZdlPvS_`/`_Unwind_Resume`）共 +25 条；
//   ORIG 0x0822c202 只 `call _ZN12PacketHeaderC1Ett` 直构（无 operator new / 无 EH）。
//   ** `throw()` 不是解 **（旧备选方案作废）：探针 /tmp/verify357/R18pkt/probe_pn.cpp
//   （g++4.4.7 -m32 -O0）实测 ctor 加 `throw()` 只消 EH 两段，`_ZnwjPv` call + size 装载 +
//   判空仍在（4.4 不内联 placement operator new = N83 上半）；只有真继承 + 初始化列表
//   产出与 ORIG 逐条同形的 0 额外指令体（探针 fA vs fB vs D3/D4 对照）。
//   SigTypes_E.h 已把该 struct 改为 `: public PacketHeader`（删 m_pad0[6] 与其后 m_uid@+0x06，
//   布局不变：sizeof 0x15f、成员起点仍 +0x0a）。
//   ORIG 0x0822c1f6 this 装载先于常量 spill；下列语句序照 ORIG 逐条。
Packet_Mannerless_User_Accusation::Packet_Mannerless_User_Accusation()
    : PacketHeader(0x1b66, 0x15f)                              // 0x0822c202
{
    // [b28c-34 保留] P0 清绑：原 `register void* self asm("eax") = this;` 已删（N96）；
    // 裸偏移 memset 改具名 m_chatMsg（§4.3）。
    m_characNo = -1;                                           // 0x0822c20a：+0xa = -1
    m_type = 0;                                                // 0x0822c214：+0xe = 0
    m_serverGroup = 4;                                         // 0x0822c21e：+0x12 = 4
    m_accIdHi = 0;                                             // 0x0822c228：+0x1e = 0
    memset(m_name, 0, 0x1e);                                   // 0x0822c235：+0x22[30] 清零
    memset(m_targetName, 0, 0x1e);                             // 0x0822c24a：+0x40[30] 清零（NEW 曾漏）
    memset(m_chatMsg, 0, 0x100);                               // 0x0822c26b：+0x5e[0x100]
    m_result = 0;                                              // ORIG 0x0822c: movb $0,0x15e(%eax) 单级
}