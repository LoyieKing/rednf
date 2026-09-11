// ---------------------------------------------------------------------------
// LinkClosureB-2 追加：_NS_NonClient_CheckResponse（无独立报告，ORIG objdump
// 0x08274e4e sz 0xe3 逐指令转写，标注【推断】仅限语义命名）
// ---------------------------------------------------------------------------
// ORIG _Z27_NS_NonClient_CheckResponseiii：
//   user=G_CGameManager()->getUser(slot,2)；user==0 → 0；
//   (uint16)user->get_unique_id() != param_2 → 0（ORIG movzwl 16 位比较，
//     值域内与 int 比较等价）；
//   getCurCharacR()==0 → 0；getCurCharacNo()!=param_3 → 0；
//   !GetNonClientFlag()（ORIG xor $1 bool 取反）→
//     getHackAnalyzer()->addServerHackCnt(user, ENUM_HACKTYPE(0x33a), 1, 0, 0)；
//   return 1。
// [b28d-30] 复核 DIFF：N18b——同型 int callee 折 mem 直比；异型化旋钮需改
// CUserCharacInfo::getCurCharacNo 返回类型（跨泳道共享头，上报 Main 裁决）。
// [b28d-solo-TimerDispatcher] 已 AE：Main 代落 getCurCharacNo→unsigned int 后两段
// 物化对齐（63/63）。
int _NS_NonClient_CheckResponse(int slot, int uniqueId, int characNo)
{
    CUser* user = G_CGameManager()->getUser(slot, 2);
    if (user == 0)
    {
        return 0;
    }
    if (user->get_unique_id() != uniqueId)
    {
        return 0;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    if (user->getCurCharacNo() != characNo)
    {
        return 0;
    }
    if (!user->GetNonClientFlag())
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x33a, 1, 0, 0);
    }
    return 1;
}