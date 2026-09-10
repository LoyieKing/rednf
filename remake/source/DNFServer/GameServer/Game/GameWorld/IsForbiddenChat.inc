bool GameWorld::IsForbiddenChat(const char* name)
{
    
    // ORIG 0x086cd908：NULL/strlen 独立早退块；ban 正向测试+末尾 return false（P16/P63 块排布）
    if (name == 0)
    {
        return false;
    }
    if (strlen(name) >= 31)
    {
        return false;
    }
    std::string str(name);
    // 槽序探针记录（N82 封顶，勿再试）：指令数已 124/124 与 ORIG 完全相等，
    // 差集只剩 str/iterator 组整体 +16（ORIG -0x24/-0x1d/-0x1c vs
    // OURS -0x14/-0xd/-0xc；-0x28/-0x18 两边一致）。已试并否决的三种写法：
    //  ① 删具名 iterator 合并成 find(...)==end() → 30 NEAR 恶化到 42 DIFF；
    //  ② iterator 声明/赋值分离 → 多 emit 默认构造 call；
    //  ③ 删 str 直接传 name → key 类型不符（见 GetIDUserInIDMap 同类 N18a）。
    std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(str);
    if (it == m_idMap.end())
    {
        return false;
    }
    if (m_idMap[str].m_chatBan)
    {
        if (GetLeftTimeOfForbiddenChat(name) == 0)
        {
            EnableChat(name);
            return false;
        }
        // b28c-lane3：P221 全表达式临时链（单次调用），复现 ORIG trace 浅槽。
        cMyTrace(__PRETTY_FUNCTION__, 0x153d, 0)(
            "%s is forbidden to chat\n", name);
        return true;
    }
    return false;
