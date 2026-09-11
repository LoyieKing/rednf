// ORIG T 0x08143996：
//   CBossStage::handleMoveMap 为假 → 0；否则对 4 个槽位中存在且存活的成员
//   调 m_dungeonLog(+0xb00).IncrementBossStageStart(user, (uchar)m_stageCur,
//   (uchar)m_difficulty)；返回 1。
// [b28d-54 N] ORIG 0x08143996：被调 int 返回 ⇒ 调用点 test/sete（P197④/N18b）；
//   je/jne 极性 + 单源尾 nop 岛（P223 负例，4.4 恒合并）。语义等价。
// [b28d-solo-CBossTower 真语义 bug 修正 + AE] 旧重建写作 `if (f()==0) goto BODY;`
//   ——成功/失败反向（handleMoveMap 失败时执行了成功逻辑）；Ghidra C 明确
//   `if (cVar2 == 1) { 循环; return 1; } return 0;`。改回正向守卫 `if(!f()) return 0;`。
//   Main 代落 CBossStage.h:42 handleMoveMap int→bool 后 xor 物化复现（P292），
//   isMemberAlive 头内 inline bool 化消循环体 setne ⇒ 58:58 AE。
bool CBossTower::handleMoveMap(unsigned short* mapIndex)
{
    // ORIG 0x081439ae `call; xor $1,%eax; test %al,%al; je <body>` + 失败路径
    // `mov $0,%eax; jmp end` ⇒ 原码 `if (!f()) return 0;`（P168a①裸形）。
    if (!CBossStage::handleMoveMap(mapIndex))
        return 0;

    for (int i = 0; i < 4; ++i)
    {
        CUser* member = m_party->get_user(i);
        if (member != 0 && isMemberAlive(i))
        {
            m_dungeonLog.IncrementBossStageStart(
                *member,
                static_cast<unsigned char>(m_stageCur),
                static_cast<unsigned char>(m_difficulty));
        }
    }
    return 1;
}