unsigned int GameWorld::GetLeftTimeOfForbiddenChat(const char* name)
{
    // N89 封顶（与 IsForbiddenChat/IsForbiddenMove 同根因）：ORIG 4.1 对
    // `std::string str(name)` 发 allocator 临时 SaIcEC1Ev/D1Ev call（槽
    // -0x2d），4.4 判 trivial 省略 ⇒ str/it/end/trace 槽组连锁平移；指令数
    // 147/147 与 call 序全等，槽组不可翻（显式 allocator 形 4.4 仍省略）。
    // ORIG 0x086ce73c：64-bit end/diff（有符号比较）；经 G_GameWorld() 调 WhenIsTheEnd
    if (name == 0)
    {
        return 0;    // ORIG：name 空直接返回，无 trace
    }
    if (strlen(name) >= 31)
    {
        // b28d-lane11：P221 全表达式临时链（兄弟块 trace 槽序翻正试形）。
        cMyTrace(__PRETTY_FUNCTION__, 0x15f2, 0)("GetLeftTimeOfForbiddenChat return 1\n");
        return 0;
    }
    std::string str(name);
    std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(str);
    if (it == m_idMap.end())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x15fa, 0)("GetLeftTimeOfForbiddenChat return 2\n");
        return 0;
    }
    long long end = 0;
    end = G_GameWorld()->WhenIsTheEndOfForbiddenChat(name);
    // 探针实证：4.4 下 `end > 0` 正向判定生成 ORIG 的 js/jg/jbe 树；`end <= 0` 生成 jg/js/ja
    if (end > 0)
    {
        unsigned int cur1000 = GlobalData::s_systemTime_.getCurSec() * 1000;   // ORIG 零扩展（mov $0,%edx）
        end -= cur1000;
        if (end > 0)
        {
            if (end < 60000)
            {
                return 60000;
            }
            return (unsigned int)end;
        }
        return 0;
    }
    return 0;
