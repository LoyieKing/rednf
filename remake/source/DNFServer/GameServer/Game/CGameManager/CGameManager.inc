CGameManager::CGameManager()
    : m_mutex(), m_noGameGuard(), m_gameGuard2(),
      m_userPool(), m_tradeSpacePool(), m_partyPool(), m_pvpRoomPool(),
      m_warRoomPool(), m_deathTowerPool(), m_bossTowerPool(), m_stagePool(),
      m_quickPartyPool(), m_blueMarblePool(), m_userByAccId(), m_userByAccId2(),
      m_pvpRoomMap(), m_partyMap(), m_warRoomMap(), m_deathTowerMap(), m_bossTowerMap(),
      m_stageMap(), m_quickPartyMap(), m_blueMarbleMap()
{
	// ORIG 0x82930c2：标量初始化全部在函数体内（初始化列表只含成员对象）：
	// 成员构造后先 movw $0,0x18（m_uid），随后向 m_noGameGuard（+0x1c）逐条
	// insert 14 个固定账号/IP 字符串（0x8c167b4..0x8c16833 实证，各插一条独立
	// std::string 临时），最后 8 个管理器指针（+0x2fc..+0x318）清零。
	m_uid = 0;
	m_noGameGuard.insert(std::string("testman820"));
	m_noGameGuard.insert(std::string("tilil"));
	m_noGameGuard.insert(std::string("azzuman"));
	m_noGameGuard.insert(std::string("cuwaki"));
	m_noGameGuard.insert(std::string("slainer"));
	m_noGameGuard.insert(std::string("mrsbscom"));
	m_noGameGuard.insert(std::string("arandra"));
	m_noGameGuard.insert(std::string("krucef"));
	m_noGameGuard.insert(std::string("dnftest126"));
	m_noGameGuard.insert(std::string("dnftest127"));
	m_noGameGuard.insert(std::string("dnftest128"));
	m_gameGuard2.insert(std::string("59.10.138.244"));
	m_gameGuard2.insert(std::string("218.145.141.85"));
	m_gameGuard2.insert(std::string("121.134.32.42"));
	m_pQuickPartySystemMgr = 0;
	m_pQuickPartyRewardMgr = 0;
	m_pPremiumLetheMgr = 0;
	m_pSharedServerMessageMgr = 0;
	m_pConditionEventMgr = 0;
	m_pSpecialItemRoutingMgr = 0;
	m_pAuctionAveragePriceMgr = 0;
	m_pCraneMinigameMgr = 0;
