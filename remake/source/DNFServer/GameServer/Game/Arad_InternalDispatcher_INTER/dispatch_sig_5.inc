// ============================================================================
// Arad_INTER_kAvatarHiddenOptionChange —— ORIG 0x08199d6e
// ============================================================================
int Arad_INTER_kAvatarHiddenOptionChange::dispatch_sig(CUser* user, char* body, int len)
{
    // [b28d-solo-Arad_InternalDispatcher_INTER 判词] AE（ORIG 0x08199d6e）：112=112、
    // 帧 0x30/槽 -0xc/-0x18 全等；SigHV 视图改真实字段、sete 直测形见下方注释。
    // ORIG ebx 出口寄存：各失败路径直接设值返回（检查顺序 user→sig→getCurCharacR）。
    if (user == 0)
        return 0x664;

    arad::SigAvatarHiddenOptionChange* sig = (arad::SigAvatarHiddenOptionChange*)body;   // ORIG 读 0x10(%ebp)=body
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x669, "AVATAR HIDDENOPTION CHANGE INTER_DSP(SigAvatarHiddenOptionChange) null.");
        return 0x66a;
    }
    // [b28d-solo-Arad_InternalDispatcher_INTER] ORIG 0x08199dd3：call; test %eax,%eax; sete %al; test %al,%al; je
    // ＝ 直接 `if (x == 0)` 的 4.4 sete 物化直测形（int callee 非 bool）。
    // 反例：`bool z=(x==0); if(z)` 落 `mov %al,-0x9; cmpb $0,-0x9` 撑帧；
    // `register bool z` 免槽但落 %bl（P78 汇聚变量寄存器差）⇒ 均劣于直接 if。
    if (user->getCurCharacR() == 0)
        return 0x670;

    // P48：ORIG 每次使用均重调 Singleton<...>::Get()，不许提升到局部。
    if (!Singleton<AvatarFixedHiddenOptionServer>::Get()->CheckCondition(user, sig))
        return 0;
    if (!Singleton<AvatarFixedHiddenOptionServer>::Get()->ChangeHiddenOption(user, sig))
        return 0;

    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x24c);
    packet.put_byte(1);
    // ORIG movzwl 0x2(%eax)+cwtl / movzwl 0x4(%eax)+movzwl %ax disp 折叠
    // ＝ 真实字段 short m_avatarSlot / unsigned short m_hiddenOption。
    packet.put_short((int)sig->m_avatarSlot);
    packet.put_short((unsigned int)sig->m_hiddenOption);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}