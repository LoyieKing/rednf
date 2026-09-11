// Ctor（ORIG 0x0814bd0c）：CBossPlay 基类、置 vtable（编译器自动）、构造
// MapInfo、Clear、get_rand_int(60000) 写 m_assignItemIdx/m_dropItemCnt，
// 函数体内依次 m_stageCur=1、m_dungeon=0、m_difficulty=0。
CBossStage::CBossStage()
{
    m_stageCur = 1;
    m_dungeon = 0;
    m_difficulty = 0;
    m_stageData.Clear();
    m_stageData.m_assignItemIdx = get_rand_int(60000);   // ORIG +0xa30
    m_stageData.m_dropItemCnt = get_rand_int(60000);     // ORIG +0xa34
}