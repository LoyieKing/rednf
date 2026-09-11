// ORIG 0x085ca4fe（报告 ~CPrivateStoreMgr.md 逐指令）：遍历 zone 表 delete 每个
// list<CPrivateStore*>；随后按声明逆序自动析构 zone 表/买家表/卖家表/+0x68 池/
// +0x04 嵌入 store。
// [b28d-24] DIFF 残1: P167 已改后置 it++（P28 ppEi）；余 ORIG 死局部
// `movl $0x0,-0xc(%ebp)`（P171 照抄不写）。ORIG 0x085ca4fe
CPrivateStoreMgr::~CPrivateStoreMgr()
{
    for (std::map<PrivateStoreZone, std::list<CPrivateStore*>*>::iterator it =
             m_zoneStores.begin();
         it != m_zoneStores.end(); it++)   // ORIG 用 ppEi 后置形（P28）
    {
        std::list<CPrivateStore*>* pList = it->second;
        if (pList != 0)
        {
            delete pList;
            // P171 死写入照抄：ORIG 0x085ca524 `movl $0x0,-0xc(%ebp)`（后续不再读）。
            pList = 0;
        }
    }
}