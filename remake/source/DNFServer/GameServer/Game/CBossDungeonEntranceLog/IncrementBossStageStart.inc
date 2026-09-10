void CBossDungeonEntranceLog::IncrementBossStageStart(CUser& user,
                                                      unsigned char a,
                                                      unsigned char b)
{
    // [E04 2026-09-10 解 N]（旧 round2-L30 判词「残余 gap +2 ⇒ N18a/P292 跨版本族
    //   封顶」作废）key 是「32 位容器 + 4 位位域」双视角：byte0..2 为普通 uchar 成员、
    //   byte3 为两个 4 位位域 —— 位域赋值自身产生 ORIG 的两处
    //   `and $0xf,%eax; mov %eax,%edx; and $0xf,%edx`（值截断 + 赋值截断）；
    //   union 版使 `k.key = 0` 复现单条 `movl $0x0,-0x40(%ebp)`（`*(unsigned int*)&k`
    //   形会多发 lea+mov）。探针 /tmp/qts/cb.cpp：93 = 93 逐条全等。
    union KeyBits {
        unsigned int key;
        struct {
            unsigned char m_field0;
            unsigned char m_level;
            unsigned char m_growType;
            unsigned char m_job : 4;
            unsigned char m_channel : 4;
        };
    };
    KeyBits k;
    k.key = 0;                                                // ORIG movl $0x0,-0x40
    k.m_field0 = a;                                           // ORIG movzbl -0x5c → -0x40
    k.m_level = (unsigned char)user.get_charac_level();       // → -0x3f
    k.m_growType = (unsigned char)user.getCurCharacGrowType();// → -0x3e
    k.m_job = (unsigned char)user.get_charac_job();
    k.m_channel = b;

    std::map<unsigned int, STBossDungeonTowerLog>::iterator it = m_logMap.find(k.key);
    // ORIG 布局：未命中块 fall-through、命中块在尾部（`call eq; test %al; je <命中>`）
    //   ⇒ 条件取反写 `if (it == end) { insert } else { ++ }`。
    if (it == m_logMap.end())
    {
        STBossDungeonTowerLog entry;           // ORIG memset(local,0,0x10)
        std::memset(&entry, 0, sizeof(entry));
        entry.stageStartCount = 1;             // ORIG *(int*)local = 1
        m_logMap.insert(std::make_pair(k.key, entry));
    }
    else
    {
        it->second.stageStartCount += 1;      // ORIG ++*(int*)(pair+4)
    }
