void CUser::SetGrowthWeaponEventInfo(const SIG_GROWTH_WEAPON_EVENT& sig)
{
    // SetGrowthWeaponEventInfo.md @0x8691bae：sig(+0/+4/+8/+0xc/+0x10/+0x12)
    // 六字段搬运至 GrowthWeaponEvent(+0x8ebe4)；repeat 事件 100(0x64) 进行中且
    // m_eventFlag != 1 时发送 noti 0x1ba（ORIG 对事件指针无空检直接虚调）。
    // [b28d-solo-CUserOnlyOrig] ORIG 0x08691bae 每字段 `mov 0xc(%ebp),%eax;
    // mov 0x4(%eax),%edx; mov %edx,0x8ebe8(%eax)` 直读直存（无 memcpy/无槽）；
    // SIG_GROWTH_WEAPON_EVENT 是 uchar[0x14]，用具名载荷视图内联 cast 逐字段访问。
    // 字段序照 ORIG 发射序：+4、+8、+0x10、+0xc、+0x12。
    struct GrowthWeaponSigView {
        bool m_eventFlag;            // +0x00（与目标 m_eventFlag 同为 bool ⇒ P300 RHS-first 直存）
        char m_pad1[3];              // +0x01..0x03
        unsigned int m_f4;           // +0x04
        unsigned int m_f8;           // +0x08
        unsigned int m_fc;           // +0x0c
        short m_f10;                 // +0x10（与目标 m_field10 同型，P211 档③）
        short m_f12;                 // +0x12（与目标 m_field12 同型）
    };
    m_growthWeapon.m_eventFlag = ((const GrowthWeaponSigView*)&sig)->m_eventFlag;
    m_growthWeapon.m_field4    = ((const GrowthWeaponSigView*)&sig)->m_f4;
    m_growthWeapon.m_field8    = ((const GrowthWeaponSigView*)&sig)->m_f8;
    m_growthWeapon.m_field10   = ((const GrowthWeaponSigView*)&sig)->m_f10;
    m_growthWeapon.m_fieldc    = ((const GrowthWeaponSigView*)&sig)->m_fc;
    m_growthWeapon.m_field12   = ((const GrowthWeaponSigView*)&sig)->m_f12;

    if (GlobalData::s_event_manager->GetRepeatEvent(100)->IsEventing(
            static_cast<CUser*>(0)))
    {
        // [b28d-solo-CUserOnlyOrig P225 推广] 拆嵌套 if：ORIG 两条件各发一段
        // `xor $1/test/je`；4.4 对 `a && b` 恒折直跳/物化岛，嵌套 if 逐条复现。
        // [P292/P168a] ORIG 0x08691c48 `movzbl 0x8ebe4(%eax),%eax; xor $0x1,%eax;
        // test %al,%al; je` = bool 字段 + `!field`（旧 uchar + `!=1` 产 `cmp $1,%al`）。
        if (!m_growthWeapon.m_eventFlag)
        {
            PacketGuard packet;
            packet.put_header(0, 0x1ba);
            packet.finalize(true);
            Send(packet);
        }
    }
