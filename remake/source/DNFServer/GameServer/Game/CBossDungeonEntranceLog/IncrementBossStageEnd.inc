// ORIG 0x0815662e：命中 → ++stageEndCount(+4)、playTimeSum(+0xc) += playTimeSec；
// 未命中 → insert {stageStartCount=1, stageEndCount=1, playTimeSum=playTimeSec}。
void CBossDungeonEntranceLog::IncrementBossStageEnd(CUser& user,
                                                    unsigned char a,
                                                    unsigned int playTimeSec,
                                                    unsigned char b)
{
    // [round2-L30] 修法（ORIG 0x081564f0/0x0815662e/0x0815678e）：
    //   ①`|` 两侧书写序交换（先 `(job & 0xf)` / `((b & 0xf) << 4)`）⇒ 复现 ORIG
    //     `call; and $0xf,%eax; mov %eax,%edx; and $0xf,%edx` 的右操作数先求值序；
    //   ②删 `unsigned int key = k.key;` 具名局部，find/insert 直接用 `k.key`
    //     （P0/P183：ORIG 无 key 槽，直传 &k）。
    // [E04 2026-09-10 解 N] key = 「32 位容器 + 4 位位域」双视角（同 IncrementBossStageStart：
    //   位域赋值自身产生 ORIG 的两处 `and $0xf,%eax`；union 使 `k.key = 0` 单条 disp 写）。
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
    if (it == m_logMap.end())
    {
        STBossDungeonTowerLog entry;
        std::memset(&entry, 0, sizeof(entry));
        entry.stageStartCount = 1;             // ORIG -0x54 = 1
        entry.stageEndCount = 1;               // ORIG -0x50 = 1
        entry.playTimeSum = playTimeSec;       // ORIG -0x48 = param_3
        m_logMap.insert(std::make_pair(k.key, entry));
    }
    else
    {
        it->second.stageEndCount += 1;         // ORIG ++*(int*)(pair+8)
        it->second.playTimeSum += playTimeSec; // ORIG *(uint*)(pair+0x10) += param_3
    }
}