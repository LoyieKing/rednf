int CIPGHelper::Initialize(unsigned int param1, unsigned int param2)
{
    if (m_mgr != 0)
    {
        Finalize();
    }
    m_dataTableA[0] = 0x2b0;
    m_dataTableA[1] = 0x1cb;
    m_dataTableA[2] = 0x1fc;
    m_dataTableA[3] = 0x1d4;
    m_dataTableA[4] = 0x99;
    m_dataTableA[5] = 0x1d4;
    m_dataTableA[6] = 0x90;

    // ORIG 未缓存 G_CEnvironment() 返回值，条件与拷贝各调一次（P48）；
    // env+0x2b8 = Server_Envir::m_ipgIP（CEnvironment.h 权威布局 +0x198+0x120）。
    // [b28d-solo-W6_IPGDeps] 原判 N81/N102 作废：ORIG 0x08100ba7 两步
    // `add $0x2b8,%eax; movzbl (%eax),%eax` 可由「取数组成员地址 + 转 uchar* +
    // 下标」形复现（P311 同族；探针 /tmp/init_probe2.cpp f1 折单条 vs f15/f18
    // 出两步且零槽）。仍为具名成员访问，非裸偏移。
    if (((unsigned char*)&G_CEnvironment()->m_serverEnvir.m_ipgIP)[0] == '\0')
    {
        return -1;
    }
    strncpy(m_str24, G_CEnvironment()->m_serverEnvir.m_ipgIP, 0x14);   // ORIG n=0x14 > m_ipgIP[0x10]【越界读】
    m_dataTableB[0] = 0x5a3c;
    m_dataTableB[1] = 0x5aa0;
    m_dataTableB[2] = 0x5b04;
    m_dataTableB[3] = 0x5b68;
    m_dataTableB[4] = 0x5bcc;
    m_dataTableB[5] = 0x232e;
    m_dataTableB[6] = 0;
    m_mgr = new CIPGMgr(this);           // ORIG operator new(0xf0)+ctor
    return m_mgr->Initialize(param1, param2);
