// ---------------------------------------------------------------------------
// OnPutItemForEnchant（报告 OnPutItemForEnchant.md，ORIG T 0x849df5e sz 0x6d）
// msg+0xd byte = kind；msg+0xe 同时被解释为 stItemPos_t 内联结构与首 dword。
// ---------------------------------------------------------------------------
void CEnchanter::OnPutItemForEnchant(CUser* user, MSG_STATIC_COMMAND* msg)
{
    int err = 0;
    if ((err = _PutItemForEnchant(
             user, (int)(unsigned char)msg->m_type0d,
             reinterpret_cast<stItemPos_t*>((char*)msg + 0xe),
             msg->m_value0e)) != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xad,
                                 (unsigned char)(err & 0xff));
}