void CGameManager::onTimeAdvanceAltar()
{
    __gnu_cxx::hash_map<int, advancealtar::StageControl*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<advancealtar::StageControl*> >::iterator it =
            m_stageMap.begin();
    while (it != m_stageMap.end())
    {
        advancealtar::StageControl* control = it->second;
        // ORIG 0x08295dXX：`call onTimerStageTick; xor $0x1,%eax; test %al,%al;
        // je <it++ 块>` —— `!tick` 为真落 erase、为假跳 it++（P9/P22 物化形，
        // 非 cmp$1/sete；语义与 tick==1→keep 等价）。
        if (!control->onTimerStageTick())
        {
            // ORIG 0x08295eXX：无具名 eraseIt 拷贝对（省 mov 双 qword 4 条）——
            // 后自增返回临时迭代器直接喂 erase（P135 pair 临时槽形）。
            m_stageMap.erase(it++);
            m_stagePool.Free(control);
        }
        else
        {
            it++;
        }
    }
}