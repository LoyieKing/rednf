void GameWorld::UpdateServerSnapShot()
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        // [b28d-solo-GameWorld] P225 推广：ORIG 0x086d14d4 段两条件各发
        // `setne %al; test %al,%al; je` ⇒ 拆嵌套 if（合并 && 被 4.4 折直跳）。
        if (user != 0)
        {
        if (user->getCurCharacR() != 0)
        {
            CParty* party = (CParty*)user->GetParty();
            // ORIG 0x086d1541 `movl $0x0,-0x1c` + 0x086d1559 `movzwl %ax,%eax;
            // mov %eax,-0x1c` ⇒ dungIdx 是 int（4 字节清零 + ushort 零扩展存回）。
            int dungIdx = 0;
            if (party != 0)
            {
                dungIdx = party->getDungIndex();
            }
            // ORIG 用 boost::posix_time::second_clock::local_time()
            // 生成 iso_extended(date) 与 to_simple_string(time)，再以 "%s %s" 写入快照。
            // [b28d-solo-GameWorld] N-DEP 复验（b28d-solo 二次实测）：ORIG 0x086d4419/
            // 0x086d28b5/0x086d288c/0x080cde16/0x080ce4bc 确为 **W 弱符号**（nm 实证：
            // boost 头内模板实例，非外部库；仓内 Boost 1.48 头在 -I 中）。真 boost 形
            // 编译通过、本件 p167 30=30 CLEAR（旧 22/30，8 MISSING 归零）、本件 186/187，
            // 但 TU 级 missing_in_game 135→222（+87，其中 82 个为 gnu++0x 下 boost 类
            // 的 C++11 ABI 别名 C5/D5，ORIG 全库仅 2 个同类符号）⇒ 违反「仅NEW 不得
            // 上升」+ 禁发明符号纪律 ⇒ 回退等价替代（localtime_r+strftime 同值语义：
            // %Y-%m-%d / %H:%M:%S）。判词 N-DEP 维持，本函数余寄存器/槽差（N82附）。
            time_t now = GlobalData::s_systemTime_.getCurSec();
            struct tm local;
            localtime_r(&now, &local);
            char dateStr[16];
            char timeStr[16];
            strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", &local);
            strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &local);
            cMyTrace(__PRETTY_FUNCTION__, 0x1b61, 8)(
                "%u,%d,%u,%d,%d,%d,%d,%d,%s %s",
                  user->get_acc_id(),
                  G_CEnvironment()->get_server_group(),
                  user->getCurCharacNo(),
                  (int)user->getCurCharacVill(),
                  user->get_area(false),
                  dungIdx,
                  user->getCurCharacExp(),
                  user->getCurCharacMoney(),
                  dateStr, timeStr);
        }
        }
    }
