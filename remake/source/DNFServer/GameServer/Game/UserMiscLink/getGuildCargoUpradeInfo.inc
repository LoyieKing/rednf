// ===========================================================================
// 6) GuildParameterScript::getGuildCargoUpradeInfo(int) —— ORIG T 0x08979726
//    报告 GuildParameterScript/getGuildCargoUpradeInfo.md（拼写 Uprade 为
//    ORIG mangled 原样）：m_vec38 线性查找首字段==capacity。
// ===========================================================================
STGuildStorageUpgradeInfo* GuildParameterScript::getGuildCargoUpradeInfo(int capacity)
{
    for (std::vector<STGuildStorageUpgradeInfo>::iterator it = m_vec38.begin();
         it != m_vec38.end(); ++it)
    {
        STGuildStorageUpgradeInfo* v = &*it;   // ORIG：de 结果存局部
        if (v->m_field0 == capacity)
        {
            return v;
        }
    }
    return 0;
}