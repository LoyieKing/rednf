int DisPatcher_SetPartyInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stSetPartyInfo& out = static_cast<stSetPartyInfo&>(param);
    // [b28d-solo-DisPatcherCore] P211：ORIG `call; mov %eax,%edx; mov -0x18(%ebp),%eax;
    // mov %edx,0x4(%eax)`（RHS-first+edx 中转）由「unsigned 返回 + 无具名 err 局部」
    // 复现（探针 /tmp/p211probe.cpp b/c 形）；删 ec/dst 后 out 引用槽与 ORIG 一致。
    out.m_errNo = check_error(user, msg);
    if (out.m_errNo > 0)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0x10f6,
                        __PRETTY_FUNCTION__,
                        out.m_errNo, user->get_acc_id());
    }

    // ORIG 0x081c4562：槽链 out(-0x18)→info(-0x14)→party(-0x10)→nparty(-0xc)
    //（技21 声明序=远→近；技37 GetParty 结果独立具名槽 `mov %eax,-0xc; cmpl $0,-0xc`，
    // 非 party 复用——复用会让 4.4 合并槽号致全体错位 4B）。
    const MSG_PARTY_SET_INFO& info = *reinterpret_cast<const MSG_PARTY_SET_INFO*>(&msg);
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        CParty* nparty = G_CGameManager()->GetParty();
        if (nparty == 0)
        {
            out.m_errNo = 4;
            return 0;
        }
        nparty->create_party(user);
        nparty->SetPartyInfoUI(info);
        G_GameWorld()->send_party_info_to_all(nparty, 0);
    }
    else
    {
        if (party->getManager() != user)
        {
            out.m_errNo = 8;
            return 0;
        }
        party->SetPartyInfoUI(info);
        G_GameWorld()->send_party_info_to_all(party, 1);
    }
    return 0;
