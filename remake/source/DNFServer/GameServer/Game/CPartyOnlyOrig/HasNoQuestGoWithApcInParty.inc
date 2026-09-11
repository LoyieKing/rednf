// ORIG T 0x085b65a2（HasNoQuestGoWithApcInParty.md）：状态 5/10 的成员若未在
// 做与 APC 同行任务（triggerType==3 且 triggerSubType==0 判定配对）则 false。
// b28d-solo-CPartyOnlyOrig 判词：DIFF（N 封顶）@0x085b65a2。修法已落：删
// userQuest 局部槽（内联 getCurCharacQuestR()->IsDoingQuest）对齐 ORIG 直接
// 压参形；外层条件改正向 `==5 || ==10`。残余 N 封顶：4.4 对 `||` 第二条件后
// 恒物化 `mov $1/$0; test %al,%al` 岛，4.1 为 `je body / jne continue` 直跳
//（探针 f1..f6 + 本轮 p19 f1..f5：正/负极性、嵌套 if、goto、switch 五形全产
// 物化岛）。语义等价。
bool CParty::HasNoQuestGoWithApcInParty(Quest const* quest)
{
    for (int i = 0; i <= 3; i++)
    {
        if (_checkValidUser(i))
        {
            // C05③ 试形：ORIG 0x73/0xa8/0xb3/0xc0 四条失败 jcc 汇同一 false
            // 出口（0x666b `mov $0,%eax`）= 4.1 单条短路链的 lowering
            // （0x41 `je body` 为 `||` 首条件直跳）。原两层 if 写法令 4.4 逐层
            // 各物化一次（多 `mov $1; jmp; mov $0; test; je` 5 条）。
            if ((m_member[i].m_pUser->get_state() == 5 ||
                 m_member[i].m_pUser->get_state() == 10) &&
                m_member[i].m_pUser->getCurCharacQuestR()
                    ->IsDoingQuest(quest->m_index) &&
                quest->m_triggerType == 3 && quest->m_triggerSubType == 0)
            {
                return false;
            }
        }
    }
    return true;
}