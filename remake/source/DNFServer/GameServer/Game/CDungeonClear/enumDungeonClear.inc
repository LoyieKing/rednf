// [P8S3 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
void CDungeonClear::enumDungeonClear(CUser* pUser, bool (*fn)(short, char, char*), ...) const
{
    // ORIG 0x084846e2：变参起始指针直接作为回调第三参（char*）。
    // b28d-60：去 `char* va` 二级槽（ORIG 仅 -0x20 一槽）、dungeonIdx 改 int
    //（ORIG `movzwl; cwtl; mov %eax,-0x1c` 四字节存 + 回调处 cwtl 截断）、
    // 去 pDataMgr 局部（ORIG 嵌套实参直传）、回调结果不落 bool 槽（ORIG 直接
    // `call *%eax; xor $1,%eax; test`）、end 缓存进 for 头（P13）。
    va_list ap;
    va_start(ap, fn);

    for (std::map<short, char>::const_iterator it = m_map.begin(),
                                               e = m_map.end();
         it != e; ++it)
    {
        int dungeonIdx = it->first;
        char diff = it->second;

        CDungeon* pDungeon = G_CDataManager()->find_dungeon(dungeonIdx);
        char questCleared = 0;
        // [L6S3] 同 AradfromString：int 同型 + 调用点一次 int→unsigned 转换 ⇒
        // 复现 ORIG `0848477e mov -0xc(%ebp),%ebx`（跨 receiver call 保活）与
        // `0848478c add $0x4,%eax`（探针 /tmp/probe_ebx.cpp f6 定形）。
        int questId = 0;
        if (pDungeon)
            questId = pDungeon->getHeroesRequiredQuest();
        if (questId != 0)
        {
            // UserQuest::m_cleared @+0x0004（CUser.h 权威布局）——具名访问，
            // 禁裸偏移（ORIG 0x0848478c `add $0x4,%eax` 即 &m_cleared）。
            questCleared = pUser->getCurCharacQuestR()->m_cleared.isClearedQuest(questId);
            if (questCleared != 0)
                diff = 4;
        }

        if (!fn((short)dungeonIdx, diff, ap))
            break;
    }
    va_end(ap);
}