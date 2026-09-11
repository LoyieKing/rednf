CMultiMatchRoomList::CMultiMatchRoomList()
{
    // b28d-solo-MatchRoomList VERDICT: AE（447/446 均离开 DIFF 列表）| 修法：
    // 去 playCount 局部（条件每轮重调 GetPlayCountPerOneTeam）、make_pair 第二参
    // 具名左值（复现 make_pair<int,CWaitingRoomList&> 实例）、(unsigned char) cast
    // 复现 ORIG movzbl 零扩展 | p167 的 IRoomList C1/C2 为同址别名（P93 豁免）
    // b28d-solo-MatchRoomList: 还原 ORIG 0x08566efc 形——
    // ①无 playCount 局部槽：条件每轮重调 GetPlayCountPerOneTeam（`call;movzbl;cmp;
    //   setg;test;jne`）；
    // ②make_pair 第二参为具名左值 waiting（ORIG 实例 `make_pair<int,
    //   CWaitingRoomList&>`，实参 &-0x68 循环外构造/循环后析构一次）；
    // ③i+1 物化临时槽后取址传引用。
    // 非静态成员经 NULL this 调用（ORIG 恒返 1，不访问 this），同 MatchRoomLCB.cpp。
    CWaitingRoomList waiting;
    for (int i = 0;
         i < (unsigned char)(((fair_pvp::CFairMatch*)0)->GetPlayCountPerOneTeam());
         ++i)
        m_roomMap.insert(std::make_pair(i + 1, waiting));
}