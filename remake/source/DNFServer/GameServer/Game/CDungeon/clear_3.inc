void MazeScript::clear()
{
    // ORIG 0x88f2a76：宽高置 1；三组 vector clear；0x2c..0x54 置 -1；
    // 0x64..0x6c 置 0；0x70 字节清零；ridable clear。
    // ORIG 0x088f2a85 段：宽高置 1 是 `for (i=0;i<=1;++i) arr[i]=1` 索引循环
    // （`movl $0x1,(%eax,%edx,4)` + setle 回边），两条独立直写复现不了 ⇒
    // 经匿名 union 的数组视图 m_dims 写。
    for (int i = 0; i <= 1; ++i)
        m_dims[i] = 1;
    m_grids.clear();
    m_vec14.clear();
    m_vec20.clear();
    m_2c = -1;
    m_30 = -1;
    m_34 = -1;
    m_questType = -1;
    m_50 = -1;
    m_54 = -1;
    m_clearCondition.clear();
    m_64 = 0;
    m_partyCountMin = 0;
    m_partyCountMax = 0;
    m_70 = 0;
    m_ridable.clear();
