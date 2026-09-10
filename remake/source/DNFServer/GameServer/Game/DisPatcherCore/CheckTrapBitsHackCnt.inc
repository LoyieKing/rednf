void DisPatcher_MoveMap::CheckTrapBitsHackCnt(CUser* user, MSG_MOVE_MAP& info)

{
    // ORIG 为 0x2493 魔数除法序列：q=v/7 后 r=v-q*7，r!=0 即非整除
    // PacketTypes.h MSG_MOVE_MAP::m_trapBits（ushort@+0x18，Main 已代落）真成员直读。
    // b28c-28（P0 清绑，P168a④ 裸形复测翻案）：原
    // `register unsigned short trapBits asm("ecx") = info.m_trapBits;` 绑定与 P0
    // 相悖，已删除并改为使用点直读成员表达式。旧判词称「4.4 恒产 movzwl+mov 两步、
    // 保留绑定使 divseq 保持 16 位形」系误判：实测清绑后 `info.m_trapBits % 7 == 0`
    // 直出 ORIG 的 `mov 0x10(%ebp),%eax; movzwl 0x18(%eax),%ecx; movzwl %cx,%eax;
    // imul $0x2493` 16 位 HImode 魔数除法链，ndiff 20 → 1。
    // [b28d-solo-DisPatcherCore] P214① 正向包裹：原 `if(cond){return;}` 早返形令 4.4
    // 生成空 return bb（`nop`）+ 循环出口 `jmp <epilogue>`（ORIG 0x081c4bb5 `je` 直落
    // epilogue 且无岛）⇒ 改 `if (bits%7 != 0) { body }` 后 4.4 发 `test; je L_after`，
    // 循环出口 fall-through 到 epilogue，残差 1→0。
    if (info.m_trapBits % 7 != 0)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x1f9, 1, 0, 0);
        // ORIG 0x081c4...：`cmp $0x4,%eax; setg %al; test; je` —— 正向 >4 大块（技巧17）
        if (user->getHackAnalyzer()->getServerHackAccumulatedCnt((WongWork::ENUM_HACKTYPE)0x1f9) > 4)
        {
            CParty* party = static_cast<CParty*>(user->GetParty());
            if (party != 0)
            {
                for (int idx = 0; idx <= 3; ++idx)   // ORIG cmpl $0x3; setle（尾块无 jmp）
                {
                    // ORIG `xor $0x1,%eax; test %al,%al; jne`：!boolcall continue 形（技巧1）
                    if (!party->checkValidUser(idx))
                    {
                        continue;
                    }
                    CUser* member = party->get_user(idx);
                    if (member != user)
                    {
                        member->getHackAnalyzer()->addServerHackCnt(
                            member, (WongWork::ENUM_HACKTYPE)0x1fa, 1, 0, 0);
                    }
                    DB_ReqSaveHackUserPunish::makeRequest(member->get_acc_id(), 0,
                                                          member->m_network.get_str_ip());
                    member->DisConnSig((DISCONN_SIG)0x18, true, 0);
                }
            }
        }
    }
}
