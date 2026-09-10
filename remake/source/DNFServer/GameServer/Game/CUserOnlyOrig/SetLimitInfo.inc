void CUser::SetLimitInfo(SIG_LIMIT_ITEM_INFO* info)
{
    if (info != 0) {
        m_limitItemUsage.clear();
        stLimitItemUsageInfoEx* usageInfo = G_CDataManager()->GetLimitItemUsageInfo();
        std::vector<stLimitItemUsageInfo::LimitInfo>::iterator it =
            usageInfo->m_info0.m_list.begin();
        // [FL5 2026-09-10 P28/P167] ORIG 增量段调 `__normal_iterator<...>::ppEi`（后置，
        // 带独立 sret 临时槽；p167 直证：ORIG 有 ppEi、我方旧 `++it` 产 ppEv 差集 1）。
        for (; it != usageInfo->m_info0.m_list.end(); it++) {
            LimitItemUsage usage;
            bool matched = false;
            for (int i = 0; i < info->m_count; ++i) {
                if (info->m_entries[i].m_itemId == it->m_a) {
                    usage.m_field10 = it->m_kind;
                    usage.m_field11 = it->m_b;  // +0x11
                    usage.m_field15 = it->m_a;
                    usage.m_field19 = info->m_entries[i].m_field0;
                    reinterpret_cast<CommonTime*>(&usage)
                        ->SetCreateTime(info->m_entries[i].m_createTime);
                    m_limitItemUsage.push_back(usage);
                    matched = true;
                    break;
                }
            }
            if (!matched) {
                usage.m_field10 = it->m_kind;
                usage.m_field11 = it->m_b;  // +0x11
                usage.m_field15 = it->m_a;
                usage.m_field19 = 0;
                reinterpret_cast<CommonTime*>(&usage)->SetCurTime();
                m_limitItemUsage.push_back(usage);
            }
        }
    }
