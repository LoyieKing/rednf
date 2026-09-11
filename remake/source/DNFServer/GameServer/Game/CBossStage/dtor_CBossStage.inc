// Dtor（ORIG 0x0814bdd6）：置 vtable（编译器自动）、m_stageCur=1、
//   m_dungeon=0、m_difficulty=0、Clear、rand×2、析构 MapInfo、基类（编译器自动）。
CBossStage::~CBossStage()
{
    m_stageCur = 1;
    m_dungeon = 0;
    m_difficulty = 0;
    m_stageData.Clear();
    m_stageData.m_assignItemIdx = get_rand_int(60000);
    m_stageData.m_dropItemCnt = get_rand_int(60000);
}