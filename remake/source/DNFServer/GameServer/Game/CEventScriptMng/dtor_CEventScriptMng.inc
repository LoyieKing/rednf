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
