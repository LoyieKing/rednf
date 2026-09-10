void UserMercenaryInfoMgr::UpdateCharacName(unsigned int characNo,
                                            std::string const& name)
{
    // [b28c-L29] P218：ORIG 0x081b462e 的 operator[] 实参为「先 idx 后 this」
    // （mov -0xc,%edx; mov 0x8(%ebp),%eax）——下标局部声明 int 触发 size_t
    // 转换中转即复现。
    const int idx = static_cast<int>(FindIndexByCharacNo(characNo));
    if (!CheckCharacIndex(static_cast<unsigned char>(idx)))
        return;

    // [b28c-L29] ORIG 0x081b462e：两次 operator[] 独立调用（无 entry 引用缓存，
    // P183/P157），c_str 结果经 %ebx 跨第二次 operator[] 保活。
    std::memset(m_characs[idx].m_name, 0, 0x1e);
    std::strncpy(m_characs[idx].m_name, name.c_str(), 0x1d);
