// ORIG 0x8639b20（0x2ea 字节）逐分支转写：遍历
// m_serverParameter.m_dungeonInoutc8（CDataManager+0x130），对每个副本进出
// 开放窗口注册下一次 msg=0x93 定时器（OBJ_TYPE=2，param2=0，param3=副本 idx）：
//   当前处于窗口内（startH<=H<=endH && startM<=M<=endM）→ 定到今天 00:00:00；
//   未进窗（开始前，或同小时内未到/已过起止分）→ 定到今天 start
//   （start 分已过则下一小时的 start 分）；
//   其余（已开始但窗口判定未命中的边缘）→ mktime(start)+0x15180（明天 start）。
//   delay = 目标 - now，负值钳 0。ORIG 末尾恒返回 eax=1（本模型 void，mangling 同）。
// [b28d-28] DIFF | ORIG 0x08639b20 | P167 MISSING 14×`_Rb_tree_iterator::ptEv` +
// G_CDataManager：ORIG 每轮迭代重新解引用（我方缓存），语义缺口待补。
// [b28d-solo-GameMain] N95/N82 封顶 | 211/213 | p167 29/29 CLEAR | 修法：逐字段
// `it->second.m_xxx`（14× operator->）+ localtime_r 返回指针（P43）+ begin/end
// 各自独立调 G_CDataManager（P183）+ key 具名槽；残差帧/槽平移（N95）。
int Timer_DungeonInoutOpenTime::RegistNextTimer()
{
    std::time_t now = static_cast<std::time_t>(s_systemTime_.getCurSec());
    std::tm t;
    std::tm* p = localtime_r(&now, &t);   // P43：ORIG 经返回值指针读字段
    // [N3Free171to190·GameMain] AE 修法（P339 声明序=槽序）：`next` 必须在 curHour
    // **之前**声明 —— ORIG 槽序 next(-0x1c) < curHour(-0x18) < curMin(-0x14) <
    // key(-0x10) 与声明序一致（4.4 先声明先分配深槽）。旧形在循环内声明，四槽整体
    // 轮换（我方 curHour/curMin/key/next vs ORIG next/curHour/curMin/key）。
    std::time_t next;
    const int curHour = p->tm_hour;
    const int curMin = p->tm_min;
    // P183：ORIG 无 table 引用缓存——begin/end 各自独立调 G_CDataManager()
    // （p167 差集 MISSING 1×G_CDataManager 即此）。
    // [H10回滚 0x08639b9a `it = begin()` 先声明后赋值：4.4 追加 iterator 默认
    // 构造 call（+3 条，215/213）且临时槽跑到 -0x5c，形态更差 ⇒ 不用默认构造形。]
    // ORIG 0x08639b9a `mov -0x28(%ebp),%eax; mov %eax,-0x30(%ebp)`：begin 结果落
    // 具名临时槽再拷贝进 it（3 槽 -0x28/-0x2c/-0x30）⇒ 用具名 b + 拷贝构造。
    std::map<int, STDungeonInoutTime>::iterator b =
        G_CDataManager()->m_serverParameter.m_dungeonInoutc8.begin();
    std::map<int, STDungeonInoutTime>::iterator e =
        G_CDataManager()->m_serverParameter.m_dungeonInoutc8.end();
    std::map<int, STDungeonInoutTime>::iterator it = b;
    for (; it != e; ++it)
    {
        int key = it->first;   // ORIG -0x10 槽（循环头一次性读取）
        // P157/P183：ORIG 每个字段访问都重新 `call operator->`（14×，p167 差集
        // MISSING 14×ptEv）——禁 `const T& win = it->second` 引用缓存。
        if (curHour >= it->second.m_startHour &&
            curHour <= it->second.m_endHour &&
            curMin >= it->second.m_startMin &&
            curMin <= it->second.m_endMin)
        {
            p->tm_hour = 0;
            p->tm_min = 0;
            p->tm_sec = 0;
            next = mktime(p);
        }
        else
        {
            // P0/P183：ORIG 0x08639cac `test %al,%al; je`（无 bool 具名槽）
            // —— 条件全表达式直用，禁 `const bool beforeStart = ...` 中间量。
            // ORIG 0x08639c52/0x08639c65/0x08639c78/0x08639c8b/0x08639c9e 五检
            // 依次读 startHour(0x4)/endHour(0x8)/startMin(0xc)/endHour(0x8)/
            // endMin(0x10) ⇒ 三段 OR（各段独立短路，第二段 endHour 复检；非嵌套
            // ——嵌套形令 startMin 复检、endHour 只检一次，字段序不符）。
            if (it->second.m_startHour >= curHour ||
                (it->second.m_endHour >= curHour && it->second.m_startMin > curMin) ||
                (it->second.m_endHour >= curHour && it->second.m_endMin < curMin))
            {
                // ORIG 0x08639cc5/0x08639cf3：`cmp curHour; setg %al; test; je`
                // —— 条件写作 startHour > curHour（P225 物化方向）。
                if (it->second.m_startHour > curHour)
                    p->tm_hour = it->second.m_startHour;
                else if (it->second.m_startMin > curMin)
                    p->tm_hour = curHour;
                else
                    p->tm_hour = curHour + 1;
                p->tm_min = it->second.m_startMin;
                p->tm_sec = 0;
                next = mktime(p);
            }
            else
            {
                p->tm_hour = it->second.m_startHour;
                p->tm_min = it->second.m_startMin;
                p->tm_sec = 0;
                // ORIG 0x08639d7a/0x08639d7d `mov %eax,-0x1c; addl $0x15180,-0x1c`
                // （先落槽再原地加，非 `add` 后存）⇒ 拆两条语句。
                next = mktime(p);
                next += 0x15180;  // +86400：明天同一开放点
            }
        }
        int delay = static_cast<int>(next) - static_cast<int>(now);
        if (delay < 0)
        {
            delay = 0;
        }
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                    (TIMER_MESSAGE)0x93, delay, 0,
                                    (unsigned int)key);
    }
    // ORIG 0x08639e00 `mov $0x1,%eax`（Ghidra `return 1`）—— 返回类型由 H9 在
    // TimerDispatcher.h 改 `static int`（本 .inc 定义端已同步）。
    return 1;
}