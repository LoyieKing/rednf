bool CInGameAdvertisementManager::checkCondition(CUser* user,
    CAdvertisementExposeInfo* info)
{
    if (info == 0)
    {
        return false;
    }
    // b28d-36：ORIG 0x080fcd2c 段为 `mov -0xc(%ebp),%edx; movzbl (%edx,%eax,1),%eax`
    // （基址入 edx、索引留 eax 的 scaled 形）。4.4 对**指针**下标恒折
    // `add base,%eax; movzbl (%eax)`（探针 /tmp/d36_probe_idx.cpp f1..f8 八形全败）；
    // 改用**数组引用**（探针 f9/f10）即逐条复现。
    char (&ageTbl)[1] =
        *reinterpret_cast<char(*)[1]>(&G_CDataManager()->m_inGameAd);  // +0x4a5c
    if (static_cast<unsigned char>(info->getExposeCondition()->m_pad0[1]) != 0xffu)
    {
        if (static_cast<unsigned char>(info->getExposeCondition()->m_pad0[1]) == 0)
        {
            if (ageTbl[user->GetAge()] == 1)
            {
                return false;
            }
        }
        else
        {
            if (ageTbl[user->GetAge()] == 0)
            {
                return false;
            }
        }
    }
    if (static_cast<unsigned char>(info->getExposeCondition()->m_pad0[0]) != 0xffu)
    {
        if (static_cast<unsigned char>(info->getExposeCondition()->m_pad0[0]) == 1)
        {
            if (static_cast<int>(user->getSex()) == 0)
            {
                return false;
            }
        }
        else
        {
            if (static_cast<int>(user->getSex()) == 1)
            {
                return false;
            }
        }
    }
    if (checkLevel(user, info->getExposeCondition()->m_pad0 + 3) != 1)
    {
        return false;
    }
    // 商城币区间：ORIG 嵌套三目汇聚（ja/jb + mov $1/jmp/mov $0 + test，无 bool 局部）
    if (info->getExposeCondition()->m_minCera >
            static_cast<unsigned int>(user->GetCera())
        ? true
        : (static_cast<unsigned int>(user->GetCera()) <
               info->getExposeCondition()->m_maxCera
           ? false
           : true))
    {
        return false;
    }
    if (static_cast<unsigned char>(info->getExposeCondition()->m_pad0[2]) != 0xffu)
    {
        if (user->get_charac_job() !=
            static_cast<signed char>(info->getExposeCondition()->m_pad0[2]))
        {
            return false;
        }
    }
    if (static_cast<unsigned char>(
            info->getExposeCondition()->m_pad0[0x203]) != 0)
    {
        if (user->get_acc_id() % static_cast<unsigned int>(
                static_cast<unsigned char>(
                    info->getExposeCondition()->m_pad0[0x203])) == 0)
        {
            info->getExposeCondition()->m_exposeFlag = 0;
        }
        else
        {
            info->getExposeCondition()->m_exposeFlag = 1;
        }
    }
    else
    {
        info->getExposeCondition()->m_exposeFlag = 1;
    }
    return true;
