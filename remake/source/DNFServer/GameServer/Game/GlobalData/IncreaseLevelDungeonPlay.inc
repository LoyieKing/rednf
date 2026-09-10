void CLevelDungeonPlayStatistic::IncreaseLevelDungeonPlay(int level, int playCount)
{
    if (level < 1 || level > 200)
    {
        // ORIG 0x0860ecd2：`mov 0xc(%ebp),%ebx` 发生在 cMyTrace ctor 实参装填
        // 之前（level 跨 ctor call 保活 ⇒ 4.1 直落 callee-saved %ebx，
        // 并连带 push/pop %ebx 与帧 0x64）。4.4 默认在 operator() 处重新
        // reload ⇒ register asm("ebx") + 声明位置钉住早装载（技巧 2/13/28）。
        // ORIG 0x0860ecd2：`mov 0xc(%ebp),%ebx` 在 cMyTrace ctor 实参装填之前
        // （level 跨 ctor call 落 callee-saved %ebx）。P227 全表达式临时链
        // `cMyTrace(...)("fmt", expr)` 使 4.4 在 ctor 前求值 op() 实参并以
        // %ebx 保活（具名局部/裸内联两形均劣）。
        cMyTrace(__PRETTY_FUNCTION__, 0x550, 0)(
            "LEVEL_DUNGEON_PLAY_STATISTIC wrong level(%d)", level);
        return;
    }
    // ORIG 0x0860ed0x：find 两实参装载序（-0x44 先入 0x4(%esp)、-0x30 后入
    // (%esp)）；P50 先声明后赋值实测反劣（4→11，多默认构造 call+拷贝）⇒ 回退。
    std::map<int, stDungeonPlayInfo>::iterator it = m_levelPlayMap.find(level);
    // [b28d-solo-GlobalData] P74 迭代器实参序旋钮：ORIG 0x0860ed51 先 `lea -0x44(it)`
    // 入 0x4(%esp)、`lea -0x30(end)` 入 (%esp) ⇒ 原码是 `end() == it`（第一实参 end）。
    if (m_levelPlayMap.end() == it)
    {
        stDungeonPlayInfo info;
        info.m_playCount = playCount;
        info.m_userCount = 1;
        m_levelPlayMap.insert(std::make_pair(level, info));
    }
    else
    {
        stDungeonPlayInfo& info = it->second;
        info.m_playCount += playCount;
        info.m_userCount += 1;
    }
