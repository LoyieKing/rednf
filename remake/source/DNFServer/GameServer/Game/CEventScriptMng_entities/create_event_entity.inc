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
