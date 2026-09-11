void CUser::initBingoData()
{
    // initBingoData.md @0x86912be：repeat 0x6a(BingoEvent) 进行中才继续；
    // BingoEvent::getBingoSystem()（全局符号 T 0x080c9fea，定义 BingoEventLink4.cpp）
    // 返回 shared_ptr<CBingo> 槽 {px,pn}，经 px 调 CBingo::initUserData(*bingoData)
    // （本 TU 补真实定义，见下），最后 sendBingoData()（定义 BingoEventLink4.cpp）。
    // ORIG 0x086912d5-0x086912f0：首 call 结果 %eax 免槽直接虚调 IsEventing(0)，
    // `test %al,%al; je END` truthy 正向包裹（技巧6/17）
    // [b22判词·N82] ORIG 42 vs 重建 40：getBingoData 结果 ORIG 用 callee-saved %ebx
    // 跨 getBingoSystem/operator-> 调用缓存（`mov %eax,%ebx`…`mov %ebx,0x4(%esp)`），
    // 因此需要 push %ebx/sub $0x24/add $0x24;pop %ebx 三件套；4.4.7 -O0 改用栈槽
    // -0xc(%ebp)（`mov %eax,-0xc`…`mov -0xc(%ebp),%edx; mov %edx,0x4`）+ leave，帧 0x28。
    // 二者寄存器/栈槽分配与 epilogue 形态属跨版本结构性差异（不可源码复现，禁 register asm）。
    // 控制流/调用点（GetRepeatEvent 虚调 IsEventing(0)、判空、getBingoData、
    // getBingoSystem、shared_ptr<CBingo>::operator->、initUserData(data)、sendBingoData）
    // 逐一对齐；数据地址 0x941f730 与 call 目标为 --no-compile .o 重定位假阳性（§6.3）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x6a)
            ->IsEventing(static_cast<CUser*>(0)))
    {
        // ORIG 0x08691307：二次 GetRepeatEvent 存 -0xc 槽判空（if(event) 正向）
        BingoEvent* event = reinterpret_cast<BingoEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(0x6a));
        if (event != 0)
        {
            // ORIG 0x08691323-0x08691337：getBingoSystem() 返回 shared_ptr<CBingo>*，
            // 再显式 call boost::shared_ptr<CBingo>::operator->()（0x080ce766）得 CBingo*，
            // 作为 initUserData 的 this。ORIG `mov %eax,%ebx` 将 getBingoData 结果升格
            // callee-saved 跨两次 call 保活（P0：无槽往返 ⇒ 右值内联，非具名局部）——
            // P226 裸语句表达式形重试 b22「N82」判词：值经 callee-saved 而非栈槽。
            event->getBingoSystem()->operator->()->initUserData(
                *({ static_cast<BingoData*>(getBingoData()); }));
            sendBingoData();
        }
    }
}