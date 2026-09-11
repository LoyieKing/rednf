// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG  orig/ours=38/63 ndiff-blocks=3
//   帧差 sub $18 vs $2c + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x18,%esp | mov    0x8(%ebp),%eax | leave
// [负结论 2026-09-11 H1Dispatchers] 63→38 不可闭合：我方 `::new (this) PacketHeader(0x1b66,0x15f)`
//   桥生成 `call _ZnwjPv` + null 检查 + `call _ZN12PacketHeaderC1Ett` + EH 清理
//   （`_ZdlPvS_`/`_Unwind_Resume`）共 +25 条；ORIG 0x0822c202 只 `call _ZN12PacketHeaderC1Ett`
//   直构（无 operator new/无 EH），因 ORIG 源码是**基类初始化列表**（PacketHeader 为基类）。
//   我方视图 struct 无法继承 PacketHeader（m_pad0[6] 与 m_connNo@+0x06 重叠，
//   继承会把 m_uid@+0x06 推到 +0x0a，破坏 16396 消费点）⇒ 需改共享头
//   shared/packet/include/PacketHeader.h 的 ctor 声明加 `throw()` 才能消 EH（跨 TU 影响，本波不做）。
Packet_Mannerless_User_Accusation::Packet_Mannerless_User_Accusation()
{
    // [b28c-34] P0 清绑：原 `register void* self asm("eax") = this;` 已删（N96）；
    // 裸偏移 memset 改具名 m_chatMsg（§4.3）。ORIG 0x0822c1f6 this 装载先于常量 spill。
    ::new (static_cast<void*>(this)) PacketHeader(0x1b66, 0x15f);   // 0x0822c202
    m_characNo = -1;                                           // 0x0822c20a：+0xa = -1
    m_type = 0;                                                // 0x0822c214：+0xe = 0
    m_serverGroup = 4;                                         // 0x0822c21e：+0x12 = 4
    m_accIdHi = 0;                                             // 0x0822c228：+0x1e = 0
    memset(m_name, 0, 0x1e);                                   // 0x0822c235：+0x22[30] 清零
    memset(m_targetName, 0, 0x1e);                             // 0x0822c24a：+0x40[30] 清零（NEW 曾漏）
    memset(m_chatMsg, 0, 0x100);                               // 0x0822c26b：+0x5e[0x100]
    m_result = 0;                                              // ORIG 0x0822c: movb $0,0x15e(%eax) 单级
}