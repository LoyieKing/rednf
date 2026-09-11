// ---------------------------------------------------------------------------
// ORIG 0x081cdc9a（报告 Dispatcher_RegisterToBlackList/process.md）：
// check_error → param+4；>0 返回 0；>=0 时组包转发 Monitor/Guild 代理；
// 韩服（serverGroup==3）且名单命中两个 GM 名串（EUC-KR，0x1d 字节逐字节比较）
// → param+4 = 0x7fffffff；<0 → LineFunc(0x269f,...)。
// [b28d-D33 N7/N109 封顶] ORIG 0x081cdc9a：115 vs 111 条；ORIG push ebx +
// sub 0x54 vs 4.4 sub 0x58（callee-saved 需求差，N7/N109）。p167 CLEAR 15=15。
int Dispatcher_RegisterToBlackList::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stRegisterToBlackListParam& out = static_cast<stRegisterToBlackListParam&>(param);
    // [P1T4 #284-303 负结论·残差①] ORIG 0x081cdcc0-0x081cdcc5 store 序列为 3 条
    // （`mov %eax,%edx; mov -0x10(%ebp),%eax; mov %edx,0x4(%eax)`；先 RHS 后 LHS 地址），
    // 我方 4.4 恒 2 条（`mov -0x10(%ebp),%edx; mov %eax,0x4(%edx)`）。**非源形问题**：
    // 同 TU 同写法的 Dispatcher_MouseRegister::process（ORIG 0x08263249）在 ORIG 里
    // 恰为 2 条、我方逐条相同 ⇒ 4.1 在同一 TU 内对同一写法因目标类型/上下文给出两种
    // expand 顺序 ⇒ 无源码旋钮可造。已试：调用点 `static_cast<int>(...)` 形 → 无变化。
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
        return 0;
    // [P1T4 #284-303] ORIG 0x081cdcdc-0x081cdd13：`test; jns 0x081cdd18`（非负 →
    // 跳正体）后**紧接负体**（get_acc_id 内联实参 + LineFunc + `jmp` 共享出口）⇒
    // 原码把 `<0` 错误日志支放在正体**之前**（else-if 形），且 accId 无具名局部
    // （ORIG 0x081cdcf1 直接用 `mov -0x10(%ebp),%edx; mov 0x4(%edx),%edx` 取 out.m_4，
    // 无 accId 槽）。旧形 `if (>=0){…} …; unsigned int accId=…; LineFunc(…);` 使 4.4 反向
    // 生成 `js`（负跳尾）+ 正体落地，并多 accId 槽致后续槽整体下移。
    if (out.m_4 < 0)
    {
        // [P1T4 #284-303 负结论·残差②] ORIG 0x081cdd0e-0x081cdd13：
        // `call LineFunc` 后**直接** `jmp 0x081cde30`（epilogue，跳过 0x081cde2b 的
        // `mov $0x0,%eax`）⇒ 负体不设返回值；我方 4.4 在负体 return 处恒发
        // `mov $0x0,%eax; jmp <T>`（+1 条，ours 111 vs orig 111 与该残差①互补）。
        // 唯二可造法：①删负体 `return 0`（致 int 函数落尾 UB，原码即此但**不引入**）；
        // ②`if/else` 落尾形（探针 /tmp/nb_probe.cpp q2 复现 ORIG 形态）—— 两者都
        // 使负体路径返回值不确定，而调用点 PacketDispatcher.h:264
        // `if ((result = process(...)) > 0) return result;` **消费返回值** ⇒ 属语义轴，
        // 按纪律不落（保持确定性返回 0）。残差①见上方注释（无源形解）。
        // [Q7S5 2026-09-11 语义轴·落] 「返回值不确定」的判词作废：LineFunc 的**真实
        //   返回类型是 int 且返回第 1 参 line**（CompoundEmblemDeps.cpp:125
        //   `return line;`，ORIG 0x085908d7 同形）⇒ ORIG 0x081cdd0e-0x081cdd13
        //   `call LineFunc; jmp <epilogue>` 的负体返回值 = 0x269f(9887)，**完全确定**。
        //   调用点 PacketDispatcher.h:264 `if ((result=process(...))>0) return result;`
        //   消费该值 ⇒ 原 `return 0` 使负体路径落 `if (result<0) return 0;` 之后继续
        //   （dispatch 结果 0），ORIG 则直接返回 9887。
        //   改 `return LineFunc(...)`：无 UB、汇编形与 ORIG 同为 `call; jmp epilogue`
        //   （返回值直落 eax，无 `mov $0x0,%eax`）。
        return LineFunc(0x269f, __PRETTY_FUNCTION__, out.m_4, user->get_acc_id());
    }

    // strncpy(msg+0x14, len=msg+0x10)（MSG_REGISTER_TO_BLACKLIST 名字字段）
    // [P1T4 #284-303] ORIG 0x081cdd18 `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)`
    // 出现在 packet 构造 `call C1Ev`（0x081cdd24）**之前** ⇒ req 引用声明先于 packet；
    // ORIG 0x081cdd3a 为 `mov 0x10(%eax),%eax`（具名字段常量偏移直取、无 add）⇒
    // 用 PacketTypes.h 已还原的 req.m_nameLen(+0x10)/req.m_characName(+0x14)。
    // 旧 `*(unsigned int*)((char*)&req + 0x10)` 形恒多 `add $0x10; mov (%eax)`。
    MSG_REGISTER_TO_BLACKLIST& req =
        reinterpret_cast<MSG_REGISTER_TO_BLACKLIST&>(msg);

    Packet_Register_To_BlackList packet;
    packet.m_accId = user->get_acc_id();

    strncpy(packet.m_name, req.m_characName, req.m_nameLen);
    // ORIG 韩服判定：G_CEnvironment+0x378 == 3（serverGroup==3）才做 GM 名单比较。
    // ORIG 直接读 m_serverEnvir.m_serverGroup（+0x378 内联读取，无 get_server_group 调用）。
    if (G_CEnvironment()->m_serverEnvir.m_serverGroup == 3)
    {
        // [OurOnly33 符号名轴 2026-09-11] ORIG 全 ELF 无 kGmName1/kGmName2（我方发射
        // `_ZZN30Dispatcher_RegisterToBlackList7process…E8kGmName1/2`）：ORIG 用匿名
        // .rodata 串区（DAT_08bc17d6 / DAT_08bc17dd，各 0x1d 字节精确比较）
        // ⇒ 删 local static、使用点内联字节转义字面量（字面量含结尾 NUL 但 strncmp 只取
        //   前 0x1d 字节，与定长数组逐字节等价；EUC-KR 字节保持原样）。
        if (strncmp(packet.m_name,
                    "\xbb\xfe\xbf\xc0\xc0\xaf\x00\xbb\xfe\xc0\xaf\xb8\xae\x00SKILL_COMMAND_L",
                    0x1d) == 0 ||
            strncmp(packet.m_name,
                    "\xbb\xfe\xc0\xaf\xb8\xae\x00SKILL_COMMAND_LOG : CU", 0x1d) == 0)
        {
            out.m_4 = static_cast<int>(0x7fffffff);
            return 0;
        }
    }

    // [b23S3C] P48：ORIG 两代理点各调 user->GetServerGroup() 一次（p167
    // MISSING -1 即此），无 group 槽。
    // [P1T4 #284-303] ORIG 0x081cddc3/0x081cdde3 与 0x081cddf7/0x081cde1f：代理指针
    // 无具名局部（无 `mov %eax,-N(%ebp)` 槽，GetServerProxy 返回直接 `mov %eax,(%esp)`）；
    // 且 &packet 提前算入 callee-saved %ebx（0x081cddc3 `lea -0x3c(%ebp),%ebx` 在
    // `call GetServerGroup` 之前）⇒ 旧 monitor/guild 具名局部多 2 槽 + 2 组 load/store，
    // 并致 4.4 放弃 %ebx 缓存（帧 push %ebx/sub 0x54 变 sub 0x58）。语义等价。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x2c);
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x2c);
    return 0;
}