// ---------------------------------------------------------------------------
// 45. CUser::verifyDoingQuest @0x0868d144
// ---------------------------------------------------------------------------
// [b28d-22 N73] ORIG 0x0868d198 `call isClearQuest; test %al,%al; je`（uchar 返回值
// 直判）。callee UserQuest::isClearQuest 权威型 unsigned char（ORIG 0x086ab920 直返
// 无归一，CUser.h:1268 b28c C28L9 定案）。4.4 对 uchar 返回值的任何 if 写法均物化
// （/tmp/p22_uchar.cpp a/b/c/d/e/g/h/i 八形：test+setne+test 或 cmpb 槽化）⇒ 跨版本
// bool/uchar ABI 差异，封顶；语义等价。
void CUser::verifyDoingQuest()
{
    for (int i = 0; i <= 0x13; ++i)
    {
        // ORIG 0x0868d156：doingIdx 槽 -0xc 先死初始化 0（4.1 具名局部）；
        // 0x0868d163/0x0868d184：getCurCharacQuestR 每使用点各调一次（免 spill 槽，
        // 技巧37）；0x0868d1a2/0x0868d1be：getCurCharacQuestW 同样两次独立调用。
        int doingIdx = 0;
        doingIdx = getCurCharacQuestR()->m_quest[i];         // 类内 +0x7538
        if (doingIdx == 0)
            continue;
        if (getCurCharacQuestR()->isClearQuest(doingIdx))
        {
            getCurCharacQuestW()->m_quest[i]   = 0;
            getCurCharacQuestW()->m_trigger[i] = 0;          // 类内 +0x7588
        }
    }
}