int Arad_INTER_kLoadRewardEventItem::dispatch_sig(CUser* user, char* body, int len)
{
    // [b28d-solo-Arad_InternalDispatcher_INTER 判词] AE 闭合（ORIG 0x08198fd0）：190=190 条、帧/槽全等。
    // 真语义 bug 已修：sig 原取自 len（0x14(%ebp) int），ORIG 0x08199007 读
    // 0x10(%ebp)=body(char*) ⇒ 原码解引用的是包体指针而非长度。
    if (user == 0 || user->getCurCharacR() == 0)
        return 0x4d3;

    arad::SigLoadRewardEventItem* sig = (arad::SigLoadRewardEventItem*)body;
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x4d8, "SigLoadRewardEventItem is null.");
        return 0x4d9;
    }

    // [b28d-solo] P52/P134 链式单语句：ORIG 0x08199073/0x08199082 findGameScript 结果
    // 直接经 %eax 传入 receiver 调用（无具名 epdm 槽）；eventReward 的 curSec 在
    // string 构造前求值并 %ebx 保活（0x081990ee）。具名局部形落 4 槽、帧 0x60 vs 0x40。
    ((Arad_EventPeriodDataManager*)Singleton<Arad_DataManager>::Get()->findGameScript(
        std::string("Arad_EventPeriodDataManager")))->addUserRewardData(user, *sig);

    ((Arad_EventPeriodDataManager*)Singleton<Arad_DataManager>::Get()->findGameScript(
        std::string("Arad_EventPeriodDataManager")))->eventReward<FnRewardLethe>(
            (ENUM_REPEAT_EVENT_CODE)0x8f, GlobalData::s_systemTime_.getCurSec(), user);

    // ORIG 无判空直接虚调 vtable+0x34（slot IsEventing）传 NULL。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x9b)->IsEventing(0))
    {
        ((Arad_EventPeriodDataManager*)Singleton<Arad_DataManager>::Get()->findGameScript(
            std::string("Arad_EventPeriodDataManager")))->eventReward<FnRewardMomoji>(
                (ENUM_REPEAT_EVENT_CODE)0x9b, GlobalData::s_systemTime_.getCurSec(), user);
    }
    return 0;
