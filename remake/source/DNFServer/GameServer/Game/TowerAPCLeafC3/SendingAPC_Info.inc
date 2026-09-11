// ---------------------------------------------------------------------------
// SendingAPC_Info::Set_APC_Info —— ORIG T 0x085fe906 [0x085fe906, 0x085febc9]
// clear → m_userCharacNo=info[+0] → put_header(0,0x15a) → put_byte(layer)
// → (put_int(len)+put_str(name@+4)) → put_byte(lv/job/grow_type @+24/+28/+2c)
// → (put_int(len)+put_str(guild@+30)) → put_int(pvp_grade@+48)
// → 循环 i=0..21：put_int(*(int*)(info + 0x4c + i*4))（能力表 22 项）
// → (put_int(len)+put_str(creature_name@+a4)) → put_int(creature_id@+b4)
// → finalize(true) → cMyTrace 日志。
// ---------------------------------------------------------------------------
// SendingAPC_Info::SendingAPC_Info —— ORIG T 0x085fe860 [0x085fe860,0x085fe8c9]
// 报告 SendingAPC_Info/SendingAPC_Info.md：构造 +0x04 PacketBuf → clear →
// memset(m_apcInfo@+0x17338, 0, 0xbc)。
SendingAPC_Info::SendingAPC_Info()
{
    m_buf.clear();
    memset(&m_apcInfo, 0, 0xbc);
}