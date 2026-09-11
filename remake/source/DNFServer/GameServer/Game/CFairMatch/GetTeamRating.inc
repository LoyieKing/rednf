// ORIG GetTeamRating 0x85627b2：非零 rating 求平均，全零返 0。
// [b28 判词·N85] AE-ndiff≈4：唯一真差 = `rating != 0.0f` 的 x87 比较 4.1 发
// `fucompp; fnstsw %ax; sahf`（3 条）vs 4.4 恒 `fucomip; fstp`（2 条），负例 N85
// 「x87 比较两向跨版本」定案（GetResult/BuyAuctionItem 族）；源形不可择。
// 其余 43/43 逐条同（含 mov $0 整型零初始化 sum、fildl/fdivp 平均段、-0x14 中转槽）。
// [b28d-solo-CFairMatch] N85→AE | x87 比较 lowering：ORIG `fucompp; fnstsw %ax; sahf`
// vs 4.4.7 `fucomip; fstp`（0x085627fa 段），其余 43/43 逐条同。本机实测
// -march=i486 -mtune=generic 使该件归零（GCC 4.4 无 x86 函数级 target 旋钮，
// `#pragma GCC target("i486")` 实测报 attribute 未知）⇒ 已上报 Main；Main 已将
// *Game/CFairMatch.cpp 加入 i486 路由组（dnf_game_cxx.sh L355，P230 补先例），
// check_tu_routed 复验该件 AE（TU ae 24→25 / diff 3→2）。
float TeamInfoThird::GetTeamRating()
{
    float sum = 0.0f;
    int count = 0;
    for (int i = 0; i < 4; ++i)
    {
        sum += m_players[i].m_rating;
        if (m_players[i].m_rating != 0.0f)
            ++count;
    }
    if (count != 0)
        sum = sum / (float)count;
    return sum;
}