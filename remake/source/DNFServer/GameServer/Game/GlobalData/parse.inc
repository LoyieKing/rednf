// [b28d-18 DIFF] p167 MISSING G_CEnvironment×1（真缺口未闭合）；余槽位（N82） | ORIG 0x0829d8e8
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（42/42）；残差=N82 槽位/寄存器 + 行解析展开。
// [SweepGlobalData] trim 返回值丢弃（ORIG 0x0829d928/0x0829d933 无回写）后 289 vs 283；
// 残余 span 差 = N18a/P211 装载序（G_CEnvironment/make_pair 段各 1~4 条）。
bool CDBEnviroment::parse(char* buf)
{
    const char* sep = "=";
    char* p1 = strtok(buf, sep);
    char* p2 = strtok(0, sep);
    // ORIG 0x0829d928/0x0829d933：trim 返回值被丢弃（`call trim` 后直接
    // `mov -0x48(%ebp),%eax` 复用 p1/p2 槽，无 `mov %eax,-0x48` 回写）⇒
    // 源形是语句调用 `trim(p1);`（trim 就地改缓冲，指针不变）。
    trim(p1);
    trim(p2);
    if (strcmp(p1, "game_db_type") == 0)
    {
        if (strcmp(p2, "cain") == 0)
            m_dbType = (ENUM_SERVER_GROUP)1;
        else if (strcmp(p2, "diregie") == 0)
            m_dbType = (ENUM_SERVER_GROUP)2;
        else if (strcmp(p2, "siroco") == 0)
            m_dbType = (ENUM_SERVER_GROUP)3;
        else if (strcmp(p2, "prey") == 0)
            m_dbType = (ENUM_SERVER_GROUP)4;
        else if (strcmp(p2, "casillas") == 0)
            m_dbType = (ENUM_SERVER_GROUP)5;
        else if (strcmp(p2, "hilder") == 0)
            m_dbType = (ENUM_SERVER_GROUP)6;
        else if (strcmp(p2, "ruke") == 0)
            m_dbType = (ENUM_SERVER_GROUP)7;
        else if (strcmp(p2, "seria") == 0)
            m_dbType = (ENUM_SERVER_GROUP)8;
        else if (strcmp(p2, "anton") == 0)
            m_dbType = (ENUM_SERVER_GROUP)9;
        else if (strcmp(p2, "roxy") == 0)
            m_dbType = (ENUM_SERVER_GROUP)0xa;
        else if (strcmp(p2, "bakal") == 0)
            m_dbType = (ENUM_SERVER_GROUP)0xb;
        else if (strcmp(p2, "ozma") == 0)
            m_dbType = (ENUM_SERVER_GROUP)0xc;
        else if (strcmp(p2, "khazan") == 0)
            m_dbType = (ENUM_SERVER_GROUP)0xd;
        else if (strcmp(p2, "first1") == 0)
            m_dbType = (ENUM_SERVER_GROUP)0x63;
        else if (strcmp(p2, "first2") == 0)
            m_dbType = (ENUM_SERVER_GROUP)0x64;
        else if (strcmp(p2, "starter_server") == 0)
            m_dbType = (ENUM_SERVER_GROUP)0x11;
        else
        {
            // ORIG 0x0829dbdd：p2 在 cMyTrace ctor 之前读入 %ebx 并跨 ctor 保活
            // ⇒ 源形为单语句全表达式临时链（P37/P227/P328⑩）。
            cMyTrace(__PRETTY_FUNCTION__, 0x2032, 5)("Unknown value (%s)", p2);
            return 0;
        }
        std::map<std::string, std::string> tmp;
        // ORIG 0x0829dc2f..0x0829dc4a：m_dbType 先值拷贝到具名局部（-0x34）再取址
        // 传 operator[](const ENUM_SERVER_GROUP&)（P0 槽往返 + ㉕实参来源判据）。
        // [L8 2026-09-10 判据⑩ 实验] 声明序对调（dbType 先于 tmp）后我方槽集合变为
        // {…-0x3c,-0x40,-0x48(map),-0x60,-0x64…}（map 由 -0x5c 移 -0x48），与 ORIG
        // {…-0x30,-0x34,-0x48,-0x4c,-0x64(map)…} 仍不匹配且 diff 未降 ⇒ 已回滚。
        // 结论：4.1 跨块槽分配与 4.4 的「声明序」旋钮方向不一致（P112 明文），判 N。
        ENUM_SERVER_GROUP dbType = m_dbType;
        G_CEnvironment()->m_serverGroupMap[dbType] = tmp;
    }
    else
    {
        // ORIG 0x0829dc9a/0x0829dcbb：find 的 receiver 与 end 的 receiver 各自
        // 独立调用 G_CEnvironment()（P48 无 CSE + P157 逐字段重读，禁 env 缓存；
        // 实证旧写缺 G_CEnvironment×1）。
        // ORIG 0x0829dc8e..0x0829dca5：m_dbType 同样先值拷贝到具名局部（-0x30）。
        // [G7-sub] 本轮复核：ORIG 289 / 我方 289、帧 0x70==0x70（0x0829d8ed
        //   `sub $0x70,%esp`）；p167 仅 map/pair/cMyTrace 的 C1↔C2 / D1↔D2 别名。
        //   残差 = **同一帧内局部槽基差 8**：sep ORIG -0x10==我方 -0x10，但
        //   p1/p2 ORIG -0x48/-0x4c vs 我方 -0x40/-0x44、dbType 副本 ORIG
        //   -0x34/-0x30 vs 我方 -0x60/-0x64、tmp map ORIG -0x64 vs 我方 -0x5c
        //   ⇒ N126（4.1 首槽 -0x4 / 4.4 -0xc 版本常数）封顶；声明序对调已实测
        //   回滚（见上条判词）。
        ENUM_SERVER_GROUP dbType = m_dbType;
        std::map<ENUM_SERVER_GROUP, std::map<std::string, std::string> >::iterator it =
            G_CEnvironment()->m_serverGroupMap.find(dbType);
        if (it != G_CEnvironment()->m_serverGroupMap.end())
        {
            // ORIG 0x0829dcf5..0x0829dcfd：先 `it.operator->()` + `add $0x4,%eax`
            // 把 &it->second 缓存进具名局部（-0xc），之后才求 make_pair 实参并
            // insert（P183 反向警告：二次访问处 ORIG 保留具名缓存；单表达式
            // `it->second.insert(...)` 4.4 会先算实参后算 receiver ⇒ 顺序反）。
            std::map<std::string, std::string>& second = it->second;
            second.insert(std::make_pair(p1, p2));
        }
    }
    return 1;
}