charac_expand::CDataMgr::~CDataMgr()
{
    for (int i = 0; i < 0x14; ++i)
    {
        // ORIG：无判空，直接经虚表 +0x14（D0）删除（null 时 ORIG 崩溃语义照抄）。
        delete m_pad[i];
    }
