bool CDataManager::GetPvPChannelGrade()
{
    // [b28d-solo-CDataManager AE] 本波 TU 口径已闭合（不在 DIFF/NEAR 列表）。
    // ORIG 0x08357e9e T：-0x1c(dst)/-0x38(serverGroup) 在 guard 前物化；
    // guard `xor$1;test;je` 为 if(IsPVPChannel()) 正向包裹（0x08357ecd）。
    // 返回 0/1（ORIG `mov$0/mov$1,%eax` 双路，P159）。
    pvp_channel_info_t* dst = &m_pvpChannel;                    // 0x08357eaa
    int serverGroup = G_CEnvironment()->m_serverEnvir.m_serverGroup;  // +0x378
    if (!G_GameWorld()->IsPVPChannel())
    {
        return 0;
    }
    {
        std::map<int, std::vector<pvp_channel_info_t> >::iterator it =
            m_liveServerInfo.m_pvpChannelMap.find(serverGroup);
        if (it != m_liveServerInfo.m_pvpChannelMap.end())
        {
            // ORIG 0x08357f57：channelNo 每轮重取 env+0x1b0（G_CEnvironment
            // 调用不被缓存，P48）；命中整结构赋值（0x08357f83..0x119 逐 dword
            // 展开拷贝 9 项）后跳出。end 每轮重取 it->second.end()（0x08357fcd）。
            for (std::vector<pvp_channel_info_t>::iterator vit =
                     it->second.begin();
                 vit != it->second.end(); ++vit)
            {
                if (G_CEnvironment()->m_serverEnvir.m_channelNo == vit->m_field0)
                {
                    // ORIG 0x08357f83 `mov -0x1c(%ebp),%edx` + 逐 dword (0..0x20)
                    // = 经命名指针槽的聚合赋值（非 this+disp 直写）。
                    *dst = *vit;
                    break;
                }
            }
        }
        // ORIG 0x08358001..：P37 临时链式——R2L 求值使 maxLevel..+0xc 落
        // -0x58..-0x4c 槽、+8/+4 跨 ctor 存 edi/esi、group 存 ebx，随后才
        // ctor（0x08358052）→ operator()（0x08358095）。pvpChannelNum 常数 0。
        cMyTrace(__PRETTY_FUNCTION__, 0x18f3, 0)(
            "PVP Channel Information  serverGroup(%d), pvpChannelNum(%d), "
            "minGrade(%d), maxGrade(%d), upgradeRevision(%d), type(%d), "
            "minLevel(%d), maxLevel(%d)",
            serverGroup, 0, dst->m_minGrade, dst->m_maxGrade,
            dst->m_upgradeRevision, dst->m_channelType, dst->m_minLevel,
            dst->m_maxLevel);
        return 1;
    }
    return 0;
