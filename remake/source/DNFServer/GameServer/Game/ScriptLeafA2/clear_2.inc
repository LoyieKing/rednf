// ============================================================================
// seriaRoomDecoAniEventScript::clear —— ORIG W 0x083732a4
// 【报告缺失】无专属函数报告；已先读同类报告
// _lt_global_gt_/importSeriaRoomAnimationDecoEvent.md 与 ORIG ctor 0x083731ca
// （objdump）确认五成员布局后转写：
//   vector@+0x00 clear / map@+0x0c clear / map@+0x24 clear /
//   map@+0x40 clear / string@+0x3c clear。
// ============================================================================
void seriaRoomDecoAniEventScript::clear()
{
    m_rewardList.clear();
    m_itemMap.clear();
    m_aniList.clear();
    m_finishAniMap.clear();
    m_finishSound.clear();
}