bool CGuildServerProxy::ModifyGuildInfoProxy(unsigned int guildId, STGuildProxy& info)
{
    std::map<unsigned int, STGuildProxy>::iterator it = m_guildInfoMap.find(guildId);
    if (it == m_guildInfoMap.end())
    {
        LogManager::logFormat(1, "DF_GuildServerProxy.cpp", __PRETTY_FUNCTION__, 0x852,
                              "Find fail from guild info proxy(guild_id:%d)", guildId);
        return false;
    }
    else
    {
        // [b28d-solo-CGuildServerProxy P171 死写入照抄] ORIG @0x0846e9eb/0x0846e9fc
        // 对 it->second 连发两次 _Rb_tree_iterator::ptEv：第一次 `add $0x4` 结果
        // 写入 -0xc(%ebp) 后全程未读（死槽），第二次结果 +0x4 作为逐字段拷贝 dst。
        // 该死槽只能由「具名 pGuild 取址 + 另写 it->second」两语句复现（探针
        // /tmp/probe_mgi.cpp f vs f2：单语句只发一次 ptEv）。
        STGuildProxy* pGuild = &it->second;
        it->second = info;
        (void)pGuild;
        return true;
    }
