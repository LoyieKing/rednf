// ---------------------------------------------------------------------------
// OnTryEnchant（报告 OnTryEnchant.md，ORIG T 0x849dfcc sz 0xb1）
// ---------------------------------------------------------------------------
// [b28d-solo-ExpertJobOnlyEnchanter] P254 档③ + P235 匿名 union 双视角：
// ORIG `mov 0xe(%eax),%eax; mov %eax,%ebx` 两段升格由「字段与 find_item(int) 实参
// 异型」触发 ⇒ 取 MSG_STATIC_COMMAND 的 unsigned 视角 m_value0eU（探针
// /tmp/probe_p254d.cpp f2 逐条实证；int 视角 f1 仍单段，OnPutItemForEnchant 不受影响）。
void CEnchanter::OnTryEnchant(CUser* user, MSG_STATIC_COMMAND* msg)
{
    int err = 0;   // ORIG 首条 movl $0,-0xc：err 声明置函数头
    m_field40 = G_CDataManager()->find_item(msg->m_value0eU);
    if ((err = _TryEnchant(
             user, &msg->m_pos13,
             (ENUM_TRY_ENCHANT_TYPE)(unsigned char)msg->m_type0d)) != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xac,
                                 (unsigned char)(err & 0xff));
    }
    if (err == 0)
    {
        APSystem::CUserProc::ClearActionAndSendtoUser(
            user, APSystem::_ActionGroupIndex(0x1f), 0,
            (ENUM_PACKETCLASS)0);
    }
}