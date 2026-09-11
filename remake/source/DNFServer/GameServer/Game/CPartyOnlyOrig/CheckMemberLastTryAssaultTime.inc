// ORIG T 0x085bcab6（CheckMemberLastTryAssaultTime.md）：任一成员 600 秒内
// 有混沌击杀时间记录即 1（禁止立即组队袭击）。
int CParty::CheckMemberLastTryAssaultTime()
{
    for (int i = 0; i <= 3; i++)
    {
        if (!checkValidUser(i))
        {
            continue;
        }
        // ORIG 0x85bcb2e 域：info 槽(-0x14) 物化 → getCurSec(-0x10) → killTime；
        // 4.4 R→L/寄存器分配异构（单表达式形实测 48 insn 更差，见Lane6探针轮），
        // 残差为槽序/求值序 N82 族，维持双语句最优 43 基线。
        // b28d-45 试形：ORIG 先物化 info 到 -0x14（具名局部）再 getCurSec。
        CUserCharacInfo* info =
            reinterpret_cast<CUserCharacInfo*>(m_member[i].m_pUser);
        int curSec = GlobalData::s_systemTime_.getCurSec();
        int lastKillTime = info->GetCurCharacChaosKillTime();
        if (curSec < lastKillTime + 600)
        {
            return 1;
        }
    }
    return 0;
}