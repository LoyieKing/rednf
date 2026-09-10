int StageTimeLineParameter::importSummonObject()
{
    bool ok = false;                                // ORIG -0x61，入口置 0
    for (;;)                                        // ORIG 0889a230
    {
        _SummonObject obj;                          // ctor memset 0x24

        obj.m_id = ScanInt(&ok);                    // ORIG 0889a244
        if (!ok)
            return 0;                               // 段尾/EOF（0889a25a: ebx=0）

        std::string enemyTok;
        if (!ScanStr(&enemyTok))                    // ORIG 0889a275
            return 0x20b;
        if (enemyTok == "enemy")                    // ORIG 0889a290
            obj.m_isEnemy = 1;
        else if (enemyTok == "friend")              // ORIG 0889a2b0
            obj.m_isEnemy = 0;
        else
        {
            printf("advancealtar : SummonObject.isEnemy undefined='%s'\n",
                   enemyTok.c_str());
            return 0x218;
        }

        std::string typeTok;
        if (!ScanStr(&typeTok))                     // ORIG 0889a30b
            return 0x21d;
        if (typeTok == "normal")                    // ORIG 0889a326
            obj.m_type = 0;
        else if (typeTok == "named")                // ORIG 0889a349
            obj.m_type = 1;
        else if (typeTok == "boss")                 // ORIG 0889a36c
            obj.m_type = 2;
        else
        {
            printf("advancealtar : SummonObject.Type undefined='%s'\n",
                   typeTok.c_str());
            return 0x22e;
        }

        obj.m_field0c = ScanInt(&ok);               // ORIG 0889a3bf
        if (!ok) return 0x232;
        obj.m_field10 =
            static_cast<unsigned short>(ScanInt(&ok));  // ORIG 0889a3ea（mov %ax）
        if (!ok) return 0x235;
        obj.m_field14 = ScanInt(&ok);               // ORIG 0889a413
        if (!ok) return 0x238;
        obj.m_field18 = ScanInt(&ok);               // ORIG 0889a43b
        if (!ok) return 0x23b;
        obj.m_field1c = ScanInt(&ok);               // ORIG 0889a463
        if (!ok) return 0x23e;
        obj.m_field20 = ScanInt(&ok);               // ORIG 0889a48b
        if (!ok) return 0x241;

        // ORIG 0889a4aa..0x2d4：make_pair(obj.m_id, obj) → map insert。
        m_summonMap.insert(std::make_pair(obj.m_id, obj));
    }
