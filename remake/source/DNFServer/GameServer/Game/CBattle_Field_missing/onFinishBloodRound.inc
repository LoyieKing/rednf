void CBattle_Field::CBloodClearRewardData::onFinishBloodRound(
    bool isClear, short round, CParty* party, const CDungeon* dungeon)
{
    // [b28d-solo-CBattle_Field_missing] ORIG 0x08306fc4：
    // ①无 dungeonName/playTime 具名局部——实参右到左求值：GetDungeonName 落
    //   %ebx 跨 getPlayTime/getBloodRoundClearTime 保活（P52/P226 内联实参）；
    // ②两次 `/1000` 共用编译器临时槽 -0x1c（无源码局部）；
    // ③addPlayTime(getBloodRoundClearTime()) 每用点重调（P48 去缓存）。
    if (isClear)
        setBloodClear(true);
    m_bloodRound = (int)round;
    m_endTick = GlobalData::s_systemTime_.getCurTickCount();
    addPlayTime(getBloodRoundClearTime());
    party->historyBloodRoundClear(m_bloodRound, getBloodRoundClearTime() / 1000,
                                  getPlayTime() / 1000, dungeon->GetDungeonName());
