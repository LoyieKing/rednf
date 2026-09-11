// ============================================================================
// ORIG 0x8284b8e：空奖励槽（m_id==0）以生成结果首个掉落物的 add_info2
// （金币额）乘金币率写入 m_field04。
// [b28d-N82] fresh 复验 91/94：同 DecisionRewardType（多 push %edi、帧 0x30→0x3c、
// 循环槽 -0x10/-0xc → -0x20/-0x1c）+ N18a 索引差 ⇒ 语义等价封顶。
// [b28d-solo-CTournamentDungeonReward] DIFF 91/95：修 empty()→begin()!=end()（P197④
// 反向：4.4 empty 是 out-of-line call+bool 物化岛）与 get_add_info()→m_addInfo2
// 直读（ORIG `mov 0x7(%eax),%eax`）后调用点 4=4 与 ORIG 全等；残 = N18a + push %edi/
// N115 帧 0x10；p167 4=4 CLEAR。
// [SweepCTournament 2026-09-09] 复核 91/95：残 = N115 多 push %edi + N18a 中转×2；
// p167 4=4 CLEAR；jcc（jne/je/setle）与 ORIG 逐位全等 ⇒ N 封顶。
// ============================================================================
void CTournamentDungeonReward::SetRewardGold(
    WongWork::stGenerateResult_t& result, float goldRate)
{
    for (int card = 0; card < 2; ++card)
    {
        for (int slotIdx = 0; slotIdx < 2; ++slotIdx)
        {
            if (m_rewards[card][slotIdx].m_id == 0)
            {
                // ORIG：具名 iterator 局部（槽 -0x18）+ end() 临时（-0x14）
                // + operator!=/operator-> 实例；4.4 的 empty() 是 out-of-line
                // call 且带 bool 物化岛（P197④ 反向），故写 begin()!=end() 形。
                std::vector<Inven_Item>::iterator it =
                    result.m_dropItems.begin();
                if (it != result.m_dropItems.end())
                {
                    // ORIG 直读 +0x07（m_addInfo2），无 get_add_info call；
                    // x87 RC=truncate 直接截断（fildl+fmuls+fistpl，无 +0.5）
                    m_rewards[card][slotIdx].m_field04 =
                        (int)((float)it->m_addInfo2 * goldRate);
                }
            }
        }
    }
}