// ============================================================================
// create_event_entity @ 0x0810b402（CEventScriptMng 实体工厂）。
// flag=true → 仅基类实体（operator new 0x1c + CEventEntity ctor，无清零）。
// 否则按记录类型 value-init 对应派生类：operator new(0x20) → 整块清零循环 →
// C1 thunk（全部 12 个类的 ctor 均为编译器隐式合成 W，与 ORIG C1 同形）。
//   0x56→CAttendance        0x5c→CPcRoomPlayTime       0x65→CUsedFatigueGiveItem
//   0x191/0x192→CAccountFirstLogin    0x193→CAddItemCountDB   0x194→CLevelUpReward
//   0x196→CExchangeRandomItemReward   0x197→CAvatarDisjointRandomReward
//   0x199→CFatigueQuantity  0x19a→CPurchaseCashItemBonus
//   0x19b→CObjectBringUp    0x19c→CAccountFirstLoginDaily   其它→0
// ============================================================================
// [b28d-solo-CEventScriptMng_entities] AE（418/418）：7 个实体类 ctor 隐式化
//   （删用户 ctor 声明/定义）⇒ 4.4 对 new T() 前置 value-init 清零，12 个 case 的
//   清零循环与 ORIG 全等（旧显式 ctor 仅 5 个清零 ⇒ 341，gap 77 = 7×11 条；
//   ORIG 全 12 个 case 均含 and $0xfffffffc 清零循环）（ORIG 0x0810b402 sz 0x00000568）
CEventEntity* CEventScriptMng::create_event_entity(int recordType, bool flag)
{
    CEventEntity* entity = 0;

    if (flag)
    {
        // ORIG 0810b41e..450：new 0x1c + 基类 ctor。
        entity = new CEventEntity();
        return entity;
    }

    // 分支体按 ORIG 代码布局顺序排列（ctor 地址升序，gcc -O0 同序生成）。
    switch (recordType)
    {
    case 0x191:                                     // ORIG 08110f88
    case 0x192:
        entity = new CAccountFirstLogin();
        break;
    case 0x193:                                     // ORIG 08110fa4
        entity = new CAddItemCountDB();
        break;
    case 0x194:                                     // ORIG 08110fc0
        entity = new CLevelUpReward();
        break;
    case 0x56:                                      // ORIG 081110fdc
        entity = new CAttendance();
        break;
    case 0x196:                                     // ORIG 08110ff8
        entity = new CExchangeRandomItemReward();
        break;
    case 0x197:                                     // ORIG 08111014
        entity = new CAvatarDisjointRandomReward();
        break;
    case 0x199:                                     // ORIG 08111030
        entity = new CFatigueQuantity();
        break;
    case 0x19a:                                     // ORIG 0811104c
        entity = new CPurchaseCashItemBonus();
        break;
    case 0x19b:                                     // ORIG 08111068
        entity = new CObjectBringUp();
        break;
    case 0x5c:                                      // ORIG 08111084
        entity = new CPcRoomPlayTime();
        break;
    case 0x65:                                      // ORIG 081110a0
        entity = new CUsedFatigueGiveItem();
        break;
    case 0x19c:                                     // ORIG 0811110a
        entity = new CAccountFirstLoginDaily();
        break;
    default:                                        // ORIG 未匹配返回 0（无分配）
        break;
    }
    return entity;
}