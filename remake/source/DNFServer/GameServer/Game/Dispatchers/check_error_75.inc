// ORIG 0x081e2280（报告 Dispatcher_ImageCommunicationEquipmentUse/check_error.md）：
// !user → -1；state<=2 → -2；getCurCharacMoney < GetImageCommnicationData[0] → 10。
int Dispatcher_ImageCommunicationEquipmentUse::check_error(CUser* user, MSG_BASE& msg)
{
    (void)msg;
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)
    {
        return -2;
    }
    // ORIG 0x081e22bd：price 跨 call 缓存 %ebx（`mov (%eax),%ebx` 单级 deref），
    // 比较 `cmp %eax,%ebx; setg`（price>money 方向，技巧22/32 register asm）。
    // 【batch-19 实测】register 版帧/槽与 ORIG 全等(push %ebx,0x14)但多指令：
    // 装载经 %eax 中转 + setg<->setl fold(compare方向互换,P196 非真反)；
    // 去 register 省指令(38→33)却失帧(push%ebx,0x28)。取 register 版记 N。
    // [b28c-34] P0 清绑：原 `register int price asm("ebx")` 删除（N96）；
    // 纯 register 具名局部 38 条（装载经 eax 中转 + 比较前双搬运）；裸内联实参
    // 令值跨 call 自然升格 %ebx 单条直落（P0 表达式直读 / P226 方向）。
    if (static_cast<ImageCommunicationData*>(G_CDataManager()->GetImageCommnicationData())->m_charge
        > user->getCurCharacMoney())
    {
        return 10;
    }
    return 0;
}