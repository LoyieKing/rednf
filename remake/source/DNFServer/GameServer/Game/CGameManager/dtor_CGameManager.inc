CGameManager::~CGameManager()
{
    // b28d-solo-CGameManager 判词：AE（compare_tu 无 DIFF/NEAR；清单 ndiff 1 已消，
    // 余量为 nop 对齐豁免 §6.1）。ORIG 0x08293d1a；无改码。
    //（crane +0x318 不置空），其余容器/池成员（hash_map、StaticPool、
    // std::map/set、Mutex）按声明逆序自动析构。
    if (m_pQuickPartySystemMgr != 0)
        delete m_pQuickPartySystemMgr;
    m_pQuickPartySystemMgr = 0;
    if (m_pQuickPartyRewardMgr != 0)
        delete m_pQuickPartyRewardMgr;
    m_pQuickPartyRewardMgr = 0;
    // ORIG 0x08293d9b `je 08293dc7`（跳过 0x08293dba 的置零）⇒ 本字段的置零在 if
    // 块**内**（与 0x2fc/0x300 的块外置零不同）；jcc 目标下标判据实证。
    if (m_pAuctionAveragePriceMgr != 0)
    {
        delete m_pAuctionAveragePriceMgr;
        m_pAuctionAveragePriceMgr = 0;
    }
    if (m_pCraneMinigameMgr != 0)
        delete m_pCraneMinigameMgr;
    // 其余容器/池成员（hash_map、StaticPool、std::map/set、Mutex）按声明逆序自动析构。
