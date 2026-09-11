int Arad_VillageAttackRewardItem::parsing(char* data)
{
    // ORIG 0x81a3b48：Load 失败分别返回 0x232/0x24c；GetRowNum()==0 提前返回 0；
    // 循环条件每轮重调 GetRowNum()。原 if 包裹写法吞掉了错误码（真语义差）。
    Arad_Script script;
    if (!script.Load(data, "reward_item"))
        return 0x232;
    if (script.GetRowNum() == 0)
        return 0;
    for (int r = 0; r < script.GetRowNum(); ++r)
    {
        // ORIG 0x81a3bbc：先构造 d（-0x1c）再构造 out（-0x28），GetInt 填 out 后拷 d，
        // find 用 d、logFormat/push_back 用 out。按此形态书写以匹配字节。
        Data d;
        Data out;
        out.winPoint = script.GetInt(r, "win_point");
        out.itemIndex = script.GetInt(r, "item_index");
        out.itemCount = script.GetInt(r, "item_count");
        d = out;
        if (find_reward_item(d.winPoint, d))
        {
            LogManager::logFormat(
                1, "localjapan/Arad_Script_Loader.cpp",
                __PRETTY_FUNCTION__,
                0x244,
                "[ERROR] Already exist data. (point:%d, item:%u, count:%d)",
                out.winPoint, out.itemIndex, out.itemCount);
            return 0x245;
        }
        m_rewardList.push_back(out);
    }
    if (!script.Load(data, "reward_ranker"))
        return 0x24c;
    if (script.GetRowNum() == 0)
        return 0;
    for (int r = 0; r < script.GetRowNum(); ++r)
    {
        RankData d;
        d.itemIndex = script.GetInt(r, "item_index");
        d.itemCount = script.GetInt(r, "item_count");
        d.itemIndex2 = script.GetInt(r, "item_index2");
        d.itemCount2 = script.GetInt(r, "item_count2");
        m_rankList.push_back(d);
    }
    return 0;
}