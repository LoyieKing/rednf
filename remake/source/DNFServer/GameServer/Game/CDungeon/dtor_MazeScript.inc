MazeScript::~MazeScript()
{
    // ORIG D1/D2 0x08376f00（合成形态）：逆序析构 m_clearCondition(+0x58)、
    // m_ridable(+0x38)、m_vec20/m_vec14/m_grids；由编译器按成员声明生成。
}