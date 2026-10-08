// [DNF-FIX 2026-10-08 c36] ORIG SetGridPath compares this result unsigned (seta) => unsigned int return.
unsigned int CDungeon::getTryGenSecondEventMonsterCnt() const
{
    return m_tryGenSecondEventMonsterCnt;
}