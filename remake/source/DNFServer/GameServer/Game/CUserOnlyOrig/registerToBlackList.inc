bool CUser::registerToBlackList(unsigned int accId)
{
    if (m_vec8cef4.size() >= 10) {
        LogManager::logFormat(
            1, "user.cpp",
            __PRETTY_FUNCTION__, 0x7d62,
            "vt_black_list_.size() >= BLACKLIST_CNT_LIMIT");
        return 0;
    }
    // ORIG 0x0867f00e：`call isBlackUser; xor $0x1,%eax; test %al,%al; je <push>` ⇒ 4.1 具名取反物化形；
    // return0 岛在 push 体之后（867f039: mov $0,%eax）
    if (!isBlackUser(accId)) {
        m_vec8cef4.push_back(accId);
        return 1;
    }
    return 0;
