unsigned char CParty::DestroyPassiveObject(int param)
{
    // [ORIG 0x085a3b6a 物化形] 声明初始化 0 + call 后赋值两段（movb$0 再
    // movzbl reload）+拆&&嵌套。b28d-solo-CPartyOnlyOrig：ORIG O@0x25 为
    // `movzbl -0x9,%eax; test %al,%al; je`（零扩展回读 + 直测，P292 字段/局部
    // bool 形）⇒ 局部 bool + volatile（volatile 令 4.4 每个使用点重载并保留
    // movzbl/test 直测形，探针 dp3 v9）。
    volatile bool destroyed = false;
    destroyed = static_cast<bool>(
        reinterpret_cast<CBattle_Field*>(&m_battleField)
            ->DestroyPassiveObject(param));
    if (destroyed)   // ORIG O@0x25 movzbl reload+test
    {
        if (reinterpret_cast<CBattle_Field*>(&m_battleField)
                ->ClearCondition(
                    static_cast<ENUM_DUNGEON_CLEAR_CONDITION_TYPE>(0), param))
        {
            ClearDungeon();
        }
    }
    return static_cast<unsigned char>(destroyed);
