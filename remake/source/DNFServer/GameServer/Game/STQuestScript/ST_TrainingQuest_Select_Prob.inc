ST_TrainingQuest_Select_Prob::ST_TrainingQuest_Select_Prob()
{
    m_count = 0;
    m_pairs.clear();   // ORIG 正常路径仅 m_count=0 + m_pairs.clear()；数组由隐式构造初始化
}