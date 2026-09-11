// ================= CBattle_Field::CBattle_Field() — ORIG 0x082a44fe =================
//
// 子对象（pair[2..9]、map/vector/Mutex/CRidable/CClearCondition/嵌套奖励等）
// 由成员声明序自动构造；此处仅复刻 ORIG 的标量初值与容器清理操作。
CBattle_Field::CBattle_Field()
{
    // [round5 收口实测] ndiff=11（346/347）：正文前 198 条逐条一致；唯一残差是
    // 「正常路径 epilogue 位置」（ORIG 就地 ret、4.4 epilogue-threading 挪到函数尾
    // 并改 `jmp <尾>`）⇒ N121/N104 族（叠加 N91 ctor EH cleanup 残段）封顶。
    // [b28d-solo-CBattle_Field_missing 判词 · N91] ORIG 0x082a44fe：CBattle_Field.h
    // 已按 ORIG 修正（m_endX/m_endY/m_field14/m_field18 + m_curPos[4] +
    // m_lastPos[4]，R2 已广播）后，前 190 条指令与 ORIG 逐条一致（两组 4 次
    // pair C1Ev、m_party=0 首写、链式 -1、清零循环、标量初值、SetCurSeatNo 全对齐，
    // 帧同为 sub $0x20）。残余 = 4.4 为 ctor 内非平凡成员恒发的 EH cleanup 残段
    //（_Unwind_Resume 链，ORIG 4.1 零 cleanup）；P216 突破口需给成员类 ctor/dtor
    // 批量加 throw()（跨多文件），本波未做。
    // —— 位置链：起点/终点槽对夫妇构完后按 ORIG 置初值 ——
    // ORIG：m_lastY(+0x6c) = -1，再沿 +0x68/+0x64/+0x60/+0x18/+0x14/+0x10/+0x0c/
    // +0x08/+0x04 级联，故 m_startX/m_startY、m_curX/m_curY、m_lastX/m_lastY、
    // m_endX/m_endY(+0xc/+0x10)、m_field14/m_field18(+0x14/+0x18) 均为 -1。
    // [b28d-solo-CBattle_Field_missing] 4.4 链式赋值自**最右端**起 store ⇒ 把
    // ORIG 首写端放最右：m_lastY(0x6c)→0x68→0x64→0x60→0x18→0x14→0x10→0xc→
    // 0x8→0x4。m_curPos/m_lastPos 由隐式 C1 构造（pair 默认 0），ctor 体不写
    //（ORIG 亦只发两组 C1Ev）。
    m_party = NULL;                        // +0x00（ORIG 首写，在链式 -1 之前）
    m_startX = m_startY = m_endX = m_endY = m_field14 = m_field18 =
        m_curX = m_curY = m_lastX = m_lastY = -1;
    // ORIG C1Ev 构造后再显式清零两组 pair[4]（0x0830a4xx 段 `movl $0x0,
    // 0xc/0x10(%eax,%edx,8)`，索引 edx=i+2 / i+6，循环 i<=3）。
    for (int i = 0; i < 4; ++i)
    {
        m_curPos[i].first  = 0;
        m_curPos[i].second = 0;
        m_lastPos[i].first  = 0;
        m_lastPos[i].second = 0;
    }

    // —— 标量初值（逐字段对应 ORIG ctor 汇编）——
    m_firstEnterDungeon = 1;                // +0x5c  movb $0x1,0x5c
    m_mapHeight        = 0;                 // +0x190
    m_mapWidth         = m_mapHeight;       // +0x18c = (+0x190)
    m_ultimateDifficulty = 0;               // +0x2ab
    m_villageMonsterId = 0;                 // +0x128  movw $0x0（ushort）
    m_standardDimensionLevel   = 0;                 // +0x238
    m_bloodState       = 0;                 // +0x1b4
    m_worldMapTowerArea = 0;                // +0x23c

    // —— 容器清理 ——
    m_tournamentRoundMap.clear();           // +0x13c
    m_tournamentReward.ResetReward();       // +0xc98
    m_tournamentCurRound = 0;               // +0x154
    m_layeredMapIndex    = -1;              // +0x10c
    m_questMaze          = 0;               // +0x110
    SetCurSeatNo(0xff);                     // 082a43cc SetCurSeatNo(0xff)
}