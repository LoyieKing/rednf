// ============================================================================
// CParty::map_load_forced_finish —— ORIG 0x085b1e66：装载超时强制收尾；
// 空队走 destroy+日志，否则未完成装载的有效成员逐个 giveup_game。
// ============================================================================
void CParty::map_load_forced_finish()
{
    // b28d-49 判词：N 封顶（N82 帧/槽 + P223 continue 岛）。P167 调用点差集 CLEAR。@0x85b1e66
    if (m_pad119 == 0)                                  // +0x119 处理守卫
    {
        int count = get_member_count();                 // -0x10
        if (count == 0)
        {
            // ORIG 0x85b1e9d：GetPartyIndex() 在 cMyTrace 临时链前求值，
            // 返回值经 %ebx 跨构造传参（技巧36/37 临时链翻槽序）；勿具名局部。
            cMyTrace(__PRETTY_FUNCTION__, 0x3184, 0)(
                "<IN_DUNGEON_NULL_PARTY> not_member PartyIndex(%d)",
                (int)GetPartyIndex());
            destroy();
            LogManager::logFormat(
                1, "party.cpp", __PRETTY_FUNCTION__, 0x3187,
                "CParty::map_load_forced_finish 1, Using: %d, Member = 0\n",
                (unsigned int)(unsigned char)m_field68);   // +0x68
        }
        else
        {
            for (int i = 0; i <= 3; ++i)                // -0xc
            {
                // b28d-49 @0x85b1f31：ORIG 两独立守卫各自 `xor $0x1,%eax; test %al,%al`
                //（P133 拆守卫 + P168a 裸 `!` 形）：`!call` → jne <continue 岛>，
                // `!m_arr50[i]` → je <跳过 giveup_game>。合并 && 链产 cmp$1 物化岛（ndiff 6）。
                if (!_checkValidUser(i))
                    continue;
                if (!m_battleData.m_arr50[i])
                    giveup_game(m_member[i].m_pUser, false, false, false);
            }
            if (get_member_count() > 0)
                check_allmember_loading(true);
        }
    }
    else
    {
        // b28d-solo-CParty：P285 —— ORIG 尾 +0x172 nop 岛 + if 体尾 jmp（4.4 纯正向
        // if 恒 jump-thread），空 else{return;} 保留体尾 jmp+nop。
        return;
    }
}