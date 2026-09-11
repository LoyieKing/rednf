// [T1-GI · P327] ClearData 前移为本 TU 首个强外部符号 ⇒ GI 键名
// `_GLOBAL__I__ZN8APSystem24CActionPointEtcParameter9ClearDataEv`（ORIG 0x08895652，
// ORIG CU localchina_RDARScriptActionPointSystem.cpp）。仅移函数定义，零语义/布局改动。
void CActionPointEtcParameter::ClearData()
{
    m_actionPoints.clear();
    m_actionPointRefs.clear();
    m_rewardMessages.clear();
    m_rewardMessages2.clear();
    m_medalRewards.clear();
    m_todayRewards.clear();
}