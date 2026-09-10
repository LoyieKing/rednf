int StackableAction::Action::proc_bingo(input_param& p)
{
    // ORIG 0x0827fb14：BingoEvent(0x6a) IsEventing(0) 检查后转发 reqBingoMark；
    // 未开启返回 0x13。
    // 0x0827fae6 区：GetRepeatEvent 结果 %eax 直作虚调用 this（`mov (%eax),%edx;
    // add $0x34,%edx; mov (%edx),%edx; mov %eax,(%esp); call *%edx`，无 spill
    // 槽 ⇒ 技巧 37 免槽直用）；判真伪为正向 `test %al,%al; je 0x13-岛`
    // （技巧 1：`!boolcall()` 会多 xor ⇒ 此处 ORIG 无 xor ⇒ 源码为正向 if）。
    // 0x0827fb14 区：bingoEvent 槽 -0x10、返回 temp 槽 -0xc（技巧 21 后声明占
    // 近槽）；`cmpl $0x0,-0x10; je 0x13-岛` ⇒ 内层也是正向 if（技巧 17：
    // fall-through 块=真路径在前，0x13 尾岛）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x6a)->IsEventing(0))
    {
        // BingoEvent 完整类型已建模（BingoEvent.h 权威头，OrigBingoEvt 批次）；
        // ORIG 0x0827fb4f 直调 _ZN10BingoEvent12reqBingoMarkERN15StackableAction11input_paramE。
        BingoEvent* bingoEvent = static_cast<BingoEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(0x6a));
        if (bingoEvent != 0)
        {
            int ret = bingoEvent->reqBingoMark(p);
            return ret;
        }
    }
    return 0x13;
