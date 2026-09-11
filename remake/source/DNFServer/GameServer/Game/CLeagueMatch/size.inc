// [b28d-solo-CLeagueMatch] DIFF 23/23·N-STRIDE：stride 0x1c 的 i*4 ORIG 用
// lea(,%eax,4)、4.4 恒 shl $2（S180 探针 11 形 + 7 索引类型 + 4.4.6 + 8 旗标
// 全败），语义等价。
int TeamInfo::size() const       // ORIG T 0x0855cd26
{
    /* ORIG @0855cd42 乘法链 lea(,%eax,4)/lea(,%edx,8)/sub %edx（PlayerInfo
     * stride 0x1c）；4.4 恒把 i*4 折成 shl $2（probe：显式 unsigned long
     * 两步 stride 同产 shl），指令选择成本模型差，N 类不可翻，保持具名
     * 字段下标访问【§4.3】 */
    int n = 0;
    for (int i = 0; i <= 3; ++i)
        if (m_players[i].m_flag != 0)
            ++n;
    return n;
}