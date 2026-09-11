// ORIG 0x0832ac48 T：遍历 0x14 个 CData* 槽，非空则经 vtable+0x14 槽
//（CData 虚表 D0/deleting 析构）删除对象。
charac_expand::CDataMgr::~CDataMgr()
{
    for (int i = 0; i < 0x14; ++i)
    {
        // ORIG：无判空，直接经虚表 +0x14（D0）删除（null 时 ORIG 崩溃语义照抄）。
        delete m_pad[i];
    }
}