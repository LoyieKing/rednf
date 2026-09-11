// [b28d-solo-CEventScriptMng] ~CEventScriptMng：AE。
// ORIG 0x0810b218：两个 map 循环各自 begin/end 缓存（P13），修复前每轮重调 end()
// 多占 sret 临时槽并顶偏全帧 ⇒ 缓存后槽位逐条对齐、compare AE。
// p167 余 2×D1Ev↔2×D2Ev 为同址别名（ORIG 0x08110f24/0x08110f60 与我们的 .o 均
// 同址 W 双名，P93 伪差集），compare 判 AE，语义闭合。
CEventScriptMng::~CEventScriptMng()
{
    // ORIG 0x0810b218：end() 在循环初始化区求值一次并缓存（P13）——
    // begin 槽 -0xc / end 槽 -0x10（脚本 map），begin 槽 -0x14 / end 槽 -0x18
    // （实体 map）；`it != m.end()` 每轮重调 end() 会多出 end/临时槽并使全帧漂移。
    for (std::map<std::string, InGameEventScript*>::iterator it =
             m_scripts.begin(), endIt = m_scripts.end();
         it != endIt; ++it)
    {
        if (it->second != 0)
            delete it->second;
        it->second = 0;
    }

    for (std::map<int, CEventEntity*>::iterator it = m_entities.begin(),
             endIt = m_entities.end();
         it != endIt; ++it)
    {
        if (it->second != 0)
            delete it->second;
        it->second = 0;
    }
}