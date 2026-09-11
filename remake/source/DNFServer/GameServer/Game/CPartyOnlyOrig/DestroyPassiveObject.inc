// ORIG T 0x085b6540（DestroyPassiveObject.md）：摧毁被动物件并在触发清除条件
// 时结算整场。
// b28d-solo-CPartyOnlyOrig 判词：DIFF→修复 @0x085b6540（gap 1→0）。4.4 对普通
// bool 局部条件恒折 `cmpb $0,-0x9(%ebp); je`，ORIG 4.1 为 `movzbl -0x9(%ebp),%eax;
// test %al,%al; je`（P322② volatile 强占形）。探针 /tmp/probe_hnq/dp3.cpp v9
// （`volatile bool destroyed` + `if (destroyed)` + `return (unsigned char)destroyed`）
// 逐条复现 ORIG 29 条（含尾部 movzbl 返回）；普通 bool（v11）仍产 cmpb。
// 旧判词「uchar/bool/int 局部 × 五种条件形全产 cmpb」（探针 d1..d5/p23 f1..f5）
// 对 volatile 形不成立，已作废。
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
}