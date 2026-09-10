void Dispatcher_MailBox_Send::send(CUser* user, ParamBase& param)
{
    stMailboxSendParam& p = static_cast<stMailboxSendParam&>(param);
    if (p.m_4 == 0x7fffffff)                                    // ORIG 0x081ccdc9: je→nop 岛
        return;
    if (p.m_4 != 0)                                             // ORIG 0x081ccdd3: je→leave 直落
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x61),
                                 static_cast<unsigned char>(p.m_4 & 0xff));
    // 余差为 4.1 尾块 jmp/nop 中继 vs 4.4 fall-through（版本块序伪影，N 类）。
