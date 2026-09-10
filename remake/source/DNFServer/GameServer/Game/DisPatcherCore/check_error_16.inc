int DisPatcher_BuySkill::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return 0xffffffff;
    }
    // 具名字段（MSG_BUY_SKILL pack(1)：m_count@+0x0e、m_entries[70]@+0x0f stride 3，
    //  条目首字节 m_skillIdx；ORIG 0x081bf93a `movzbl 0xe(%eax)`+`movsbl` 为计数
    //  有符号扩展，0x081bf8ec/0x081bf8ef `lea (%ecx,%eax,1),%eax; add $0xf` 为
    //  entries 基址 + 3i 两步寻址——与成员访问形态一致）
    const MSG_BUY_SKILL& req = *reinterpret_cast<const MSG_BUY_SKILL*>(&msg);
    for (int i = 0; i < (int)(signed char)req.m_count; ++i)
    {
        const unsigned char skillId = req.m_entries[i].m_skillIdx;
        if (skillId > 0xc7 && skillId < 0xd1)
        {
            if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                    user, (RestrictCategory::Enum)1, 0x1e))
            {
                return 0xd1;
            }
        }
    }
    return 0;
