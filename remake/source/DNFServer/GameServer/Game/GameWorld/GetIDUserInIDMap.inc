int GameWorld::GetIDUserInIDMap(const char* name)
{
    if (name == NULL)
    {
        return 0;
    }
    if (!(strlen(name) < 0x1f))
    {
        return 0;
    }
    {
        std::string strName(name);
        std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(strName);
        // ORIG 0x86cd8af-0x86cd8d4：eq 真→mov$0(fall)，false→je 前跳 operator[]
        // `mov (%eax),%ebx` 单步结果 ⇒ ==end 早退形
        if (it == m_idMap.end())
        {
            return 0;
        }
        // ORIG 0x86cd8d2 `mov (%eax),%ebx` 单步 vs 4.4 `mov (%eax),%eax; mov %eax,%ebx`
        // = N18a call 后返回值中转不可造（asm("ebx") 绑定实测同形），残留 1 行
        return m_idMap[strName].m_id;
    }
}