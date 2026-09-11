// [L11·round1 判词更新] _ZN9GameWorld17send_AllBasicInfoEP5CUser：ndiff 33→**10**
// （172/172，p167 28=28 CLEAR）。本轮修法 = P221/P227 两处具名 `cMyTrace trace(f,l,v);
// trace(...)`（ORIG 无独立具名对象，实参在 ctor 前右起求值并跨 ctor 保活）改**全表达式
// 临时链** `cMyTrace(f,l,v)(...)`：帧与主槽组一次对齐（PacketGuard -0x24→-0x44 ✓、
// it -0x2c→-0x4c ✓、begin 临时 -0x5c ✓、cMyTrace 两对象 -0x38/-0x28 就位）。
// 残余 4 处（**不可造，N 封顶**，均附 ORIG 0x086c83fc 段实证）：
//  ① O[51:55] `make_basic_info` 实参装载寄存器 %eax vs %edx（ORIG lea→%eax 直压 0x4(%esp)
//     再装 info；4.4 需 %edx 中转）；
//  ②/③ 两个 cMyTrace 槽 -0x34/-0x24 vs 我方 -0x38/-0x28（差 4）；
//  ④ `m_UserInWorld.end()` sret 临时 ORIG -0x38 vs 我方 -0x18（差 0x20）。
// ②③④ 同根：4.1 在**序言**一次性分配匿名临时槽（end 临时最深），4.4 恒在**使用点**
// 分配（end 出现在循环条件 ⇒ 最浅）。实测两形均不翻：显式 `iterator(m_UserInWorld.end())`
// 拷贝临时形（ndiff 10→10）、操作数交换 `end() != it`（10→11）⇒ 判 N（P75/N82 匿名临时区）。
// 既有修法（保留）：P50 复活
// （ORIG 0x086c842a `_Rb_tree_iterator::C1Ev` 0x086d3826 + 0x086c8466 begin sret
// -0x5c → 拷贝进 it(-0x4c) ⇒ 先声明后赋值）+ P197③ info 预清（0x086c842f
// `movl $0x0,-0x10`）+ P283 goto 尾条件 + P171 照抄 error 分支 `jmp COND`
// （跳过 ++it，原码即如此）。
// [H8B 2026-09-11 复核·终判] 172/173（我方多 1）+ p167 28/28（仅 cMyTrace C1/C2 ×2、
// PacketGuard C1/C2·D1/D2、iterator C1Ev 等同址别名）⇒ 无真缺口；diff 4 hunk 净 +1。
// 残余即判词 4 处（实参装载 ORIG 0x086c84xx `lea → %eax` 直压 0x4(%esp) vs 4.4 %edx 中转；
// cMyTrace 槽 -0x34/-0x24 vs 我方 -0x38/-0x28；`m_UserInWorld.end()` sret 临时 ORIG -0x38
// vs 我方 -0x18）已实测两形不翻：显式 `iterator(end())` 拷贝临时（ndiff 10→10）、操作数交换
// `end() != it`（10→11）⇒ N（P75/N82 匿名临时区）封顶。[H8B 本轮实测] 172/173 + p167
// calls 28/28 仅别名，与上述一致。
void GameWorld::send_AllBasicInfo(CUser* user)
{
    PacketGuard packet;
    int countIndex = _makeBasicInfoHeader(packet);
    int count = 0;

    std::map<unsigned short, CUser*>::iterator it;
    CUserCharacInfo* info = NULL;
    if (m_UserInWorld.size() != 0)
    {
        it = m_UserInWorld.begin();
        goto COND;
    LOOP:
        info = it->second;
        // ORIG 0x086c848a `cmpl $0,-0x10; je 086c859d`（info==0 前向跳到函数后部的
        // trace 块）⇒ 正极性 `if (info != NULL) {visible 处理} else {trace}` 布局。
        if (info != NULL)
        {
            if (info->IsCurCharacVisible())
            {
                int index = packet.get_index();
                // [Main 2026-09-11 广播 · 接收者去 cast 范式 实测反劣已回滚] 本件 cast
                // 接收者 `((CUser*)info)->` 改具名 `CUser* charac = (CUser*)info;` 后
                // ndiff 8→38、gap −1→−3（多 2 条 eax 中转）⇒ 本件属 P75/N82 未知临时区，
                // 非 Main 范式适用画像（该范式两例为派生类转换，此处为同类指针）。
                if (((CUser*)info)->make_basic_info((char*)&packet, 0) != 1)
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0xbe3, 5)(
                        "[%s][%d]", "void GameWorld::send_AllBasicInfo(CUser*)", 0xbe3);
                }
                if (packet.getLastError() != 0)
                {
                    packet.set_index(index);
                    packet.put_short(countIndex, count);
                    packet.finalize(true);
                    user->Send(packet);
                    countIndex = _makeBasicInfoHeader(packet);
                    count = 0;
                    goto COND;
                }
                else
                {
                    count++;
                }
            }
        }
        else
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xbf6, 5)(
                "[%s][%d]", "void GameWorld::send_AllBasicInfo(CUser*)", 0xbf6);
        }
        ++it;
    COND:
        if (it != std::map<unsigned short, CUser*>::iterator(m_UserInWorld.end()))
        {
            goto LOOP;
        }
    }

    packet.put_short(countIndex, count);
    packet.finalize(true);
    user->Send(packet);
}