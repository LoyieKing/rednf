// ORIG _ZNK14CNPCScriptList18getFavorLevelValueEj 0x858174e：level 0..3 返
// m_favorExp[level]（level 0 返 0），越界（>3）返 getMaxFavorValue()。
// [b28d-solo-CDataManagerScripts] AE：①返回类型 unsigned short（ORIG 0x0858177d
// 单条 `movzwl` 直返；int 返回 4.4 恒多一条 `movzwl %ax,%eax`，P9/探针
// /tmp/b28d_s160_p1.cpp 实证）；②本定义置于 getMaxFavorValue 之前（P210：callee
// 定义在后 ⇒ 4.4 走保守 `sub $0x18`，与 ORIG 0x08581751 一致）；ORIG 0x0858174e。
unsigned short CNPCScriptList::getFavorLevelValue(unsigned int level) const
{
    if (level > 3)
        return getMaxFavorValue();
    if (level == 0)
        return 0;
    return m_npcCommon.m_favorExp[level];
}