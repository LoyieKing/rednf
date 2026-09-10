CBossStage::~CBossStage()
{
    m_stageCur = 1;
    m_dungeon = 0;
    m_difficulty = 0;
    m_stageData.Clear();
    m_stageData.m_assignItemIdx = get_rand_int(60000);
    m_stageData.m_dropItemCnt = get_rand_int(60000);
