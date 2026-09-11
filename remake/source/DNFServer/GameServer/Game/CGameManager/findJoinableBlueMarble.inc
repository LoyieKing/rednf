BlueMarble* CGameManager::findJoinableBlueMarble()
{
    // b28d-solo-CGameManager: ORIG 0x082a3424 段 `call G_CDataManager; mov %eax,(%esp);
    // call getBlueMarbleScript` 无 dm 具名槽（P0 表达式直传）。
    BlueMarbleScriptManager* script =
        (BlueMarbleScriptManager*)((CDataManager*)G_CDataManager())->getBlueMarbleScript();
    if (script == 0)
        return 0;
    int count = script->m_info.m_playerMax;  // ORIG *(script+0x30)
    // b28d-lane12: 真语义修正——ORIG 0x082a3424 外层为**倒计数** `for(;count>0;count--)`
    // （0x082a352a `subl $1,-0x10`），内层条件用**当前** count-1（0x082a34c4
    // `mov -0x10,%edx; sub $1; cmp`），而非恒定 m_playerMax-1。内层为 4.1 for 布局
    // （入口 jmp 底部条件 0x082a3491→0x082a34f5）；marble==0 时不 ++it（0x082a34a8
    // je 内层条件，P171 照抄）；迭代器先默认构造后赋值（P50 复活，0x082a3460 C1）。
    BlueMarble* marble = 0;
    __gnu_cxx::hash_map<int, BlueMarble*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<BlueMarble*> >::iterator it;
    for (; count > 0; count--)
    {
        it = m_blueMarbleMap.begin();
        // b28d-solo-CGameManager 判词：AE（while 形翻正——S154「N82附 槽互换」判词作废；
        // 探针 /tmp/solo_probe5.cpp h_while，同 SendWarRoomList/FindJoinableWarRoom）。
        while (it != m_blueMarbleMap.end())
        {
            marble = it->second;
            if (marble != 0)
            {
                if (marble->checkJoin())
                {
                    if (marble->getWaitUserCount() == count - 1)
                        return marble;
                }
                it++;
            }
        }
    }
    return 0;
}