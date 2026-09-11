// ----------------------------------------------------------------------------
// ORIG 0x0867efae CUser::registerToBlackList(unsigned int accId) bool
// [b28d-Main·检查点14] 返回类型 uint→bool（调用点 0x084d0e51 `call; xor $1; test %al; je`）；
// mangled 不含返回类型，符号名不变；定义尾 ORIG 为 mov $1/mov $0 两岛，与 bool 同形。
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
}