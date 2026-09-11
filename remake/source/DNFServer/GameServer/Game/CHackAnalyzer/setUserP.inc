// ORIG WongWork::CAutoPunishRuleHackTypeMgr 布局镜像（权威定义在
// GlobalData.cpp；此处仅按偏移只读访问 m_exceptionMap @+0x58）。「推断」标注：
// 偏移来自 mgr ctor 反汇编 0x82a5b1a。
void CHackAnalyzer::setUserP(CUser* user)
{
    // ORIG T 0x080f79c0（setUserP.md）：*(this+0) = user（与 beginCollectHackInfo
    // 共用首槽槽位复用）。
    // 指针赋值形（ORIG 先 load this 再 load 实参；int cast 形会先物化实参）。
    *(CUser**)&m_baseHackType = user;
}