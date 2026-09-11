// ---------------------------------------------------------------------------
// CUser::RewardTutorial(unsigned int)
// ORIG T 0x086475d8 sz 0x245，报告 docs/class_func_reports/CUser/RewardTutorial.md。
// 流程：取 ServerParameterScript::GetEscaladeTutorialReward(flag) 奖励清单；
// 非空时经 CInventory::insert_event_items 入包（moneyReason=3、itemReason=8、
// bLog=true、两个空串日志标签），随后 put_header(1,0x92) 包回执：byte(1)、
// byte(入包数)；>0 时对返回的槽位三元组逐个 put_byte/put_int/put_int，
// finalize(true) 后 Send。
// ---------------------------------------------------------------------------
void CUser::RewardTutorial(unsigned int flag)
{
    std::vector<int> outSlots;

    // P0：ORIG 0x086475eb `call G_CDataManager; lea 0x68(%eax),%edx; mov %edx,(%esp)`
    // 全程无槽往返 ⇒ 原码是内联表达式（无 serverParameter 具名局部）；
    // 同理 0x08647615 getCurCharacInvenW 结果留 %eax 直传 insert_event_items，
    // 源码须把 receiver 写成调用表达式（禁具名 inven 局部）。
    std::vector<std::pair<int, int> >* rewards =
        G_CDataManager()->m_serverParameter.GetEscaladeTutorialReward(flag);
    if (rewards == 0)
        return;

    int insertedCount = getCurCharacInvenW()->insert_event_items(
        *rewards, outSlots, (eMoneyAddReason)3, (eItemAddReason)8, true, "", "");

    PacketGuard packet;
    packet.put_header(1, 0x92);
    packet.put_byte(1);
    packet.put_byte(insertedCount);
    if (insertedCount > 0)
    {
		// ORIG 循环每次消费 3 个 int（槽位三元组）：short/int/int；
		// P28：ORIG 0x086476fb/0x08647734/0x0864776d 三处增量段恒调后置
		// `__normal_iterator::operator++(int)`（ppEi：`movl $0x0,0x8(%esp)` dummy
		// + sret 临时 -0x1c/-0x18/-0x14）⇒ 源形必须写 `it++`；`++it` 走 ppEv
		// 免临时槽（每处少 4 条，gap -8 主因）。
		std::vector<int>::iterator it = outSlots.begin();
		while (it != outSlots.end())
		{
			packet.put_short(*it);
			it++;
			packet.put_int(*it);
			it++;
			packet.put_int(*it);
			it++;
		}
	}
    packet.finalize(true);
    Send(packet);
}