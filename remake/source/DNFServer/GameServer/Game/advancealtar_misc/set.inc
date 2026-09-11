// ORIG T 0x088992da（objdump 全文，四段循环逐条转写）。序列化侧 get()
// （advancealtar_DB.cpp）仅落 value 域，因此本逆序列化中各记录的 map key 均
// 取自记录自身的 m_field0/m_byte0/m_int1 字段（与 make_pair 的同址取参一致，
// 见函数体注释地址）。
void AdvanceAltarData::set(const _AdvanceAltarDbData& db)
{
    // [b28d-solo-advancealtar_misc 判词] DIFF→N 封顶（N82 帧/槽族）：
    //   已修：四段循环的 8 个 memcpy 局部 + 3 个 _Slot/_BuyItem/_Reward 默认 ctor
    //   改为 **const 引用视图**直读线格式缓冲（P156 允许），使 p167 差集从
    //   MISSING make_pair<const&,const&>×4 / EXTRA memcpy×8+ctor×3+make_pair<&,&>×4
    //   收敛为 **CLEAR（ORIG 17 = OUR 17，0 missing / 0 extra）**；stages 段删
    //   `const _Stage&` 缓存（P183，ORIG 每处重算 `db.m_stages[i]`）。
    //   残余：帧 0x108 vs ORIG 0xd8（-0x30）——ORIG 对 `db+0x1b0+i*10` 每处重算
    //   （0x088993cc/d1/ee/f3/40b/422/49d/4d5 八处 `add $0x1b0,%eax`，无 rec 缓存）
    //   且 pair/make_pair/insert 的临时槽更少；4.1/4.4 槽复用策略差属 N82 族
    //   （判据：p167 CLEAR + 控制流/调用序全等，仅帧与槽号平移）。
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82 帧/槽族）→ 试=①prett2 缺口 0、
    //   strall 串差 0（仅 base64 假阳性）、p167 本函数 CLEAR(17=17)；②444 后端
    //   与 446h（4.4.6 头集+446 后端）变体四数逐项相同 ⇒ 无路由旋钮；③本波复核
    //   ORIG 0x088992da 首条 `mov 0xc(%ebp),%edx; mov 0x8(%ebp),%eax; movl $0x21,
    //   0x8(%esp)`（4.1 先 data 后 this）vs 4.4 data→0x4(%esp)/this→(%esp) 直写，
    //   及循环内 `mov -0x18; shl $2; add; add; lea (%ecx,%eax,1)` 索引序 ⇒ 维持
    //   （新=DIFF 297/304 gap=-7, blocks=23）。
    m_update = 0;                                                  // [objdump 0x88992e6]
    std::memcpy(this, &db, 0x21);                                  // [objdump 0x8899302]

    // stages：i<=0x31，条件/键均读 db.m_stages[i].m_field0（pack 后 db+0x21+i*8）。
    // b28d-solo-advancealtar_misc：ORIG 每处重算 `db.m_stages[i]` 两级寻址
    //   （0x0889939f `mov -0x1c,%edx; add $4; mov 0x1(%eax,%edx,8),%eax` 与
    //   make_pair 两实参各自 `shl $3; add` 重算）⇒ 原码无具名 `_Stage&` 缓存（P183）；
    //   原引用形使 4.4 把指针落槽（-0x48）并多一对 load/store。
    for (int i = 0; i <= 0x31; ++i)                                // [objdump 0x889939f setle]
    {
        if ((int)db.m_stages[i].m_field0 > 0)                      // [objdump 0x889931c jle]
        {
            m_stages.insert(std::make_pair(
                db.m_stages[i].m_field0, db.m_stages[i]));         // [0x8899353 key/value 同址]
        }
    }

    // slots：i<=0xb；记录基 = db+0x1b0+i*10（元素实际起 0x1b1）：
    //   key = 记录.m_byte0（rec+1）、cond int = m_int1（rec+2）、
    //   value = 从 rec+6 起的 10 字节（跨到下一记录头部，忠实 ORIG）
    // b28d-solo-advancealtar_misc：p167 差集（ORIG 17 calls / 我方 28）显示 ORIG 的
    //   make_pair 实例全为 **const 引用**形（`make_pair<unsigned char const&,_Slot const&>`
    //   等 4 个）且无 _Slot/_BuyItem/_Reward 默认 ctor、无 8 个 memcpy ⇒ 原码用
    //   **const 引用视图**直读线格式缓冲（P156「线格式序列化缓冲」允许偏移视图），
    //   原 memcpy 到非 const 局部形多 ctor+memcpy 且实例化非 const make_pair。
    for (int i = 0; i <= 0xb; ++i)                                 // [objdump 0x889952c]
    {
        // [L13-group6 判据⑩/P0 实验] ORIG 槽表无 rec/slotType/slotKey/slotValue 任何
        // 具名槽（我方曾多 -0x40..-0x34 四槽）；`*reinterpret_cast<const T*>` 本身
        // 即左值 ⇒ make_pair 仍实例化 const 引用形（p167 不变），全部内联重算。
        if (*reinterpret_cast<const int*>(                            // [objdump 0x88993d1 +2]
                reinterpret_cast<const char*>(&db) + 0x1b0 + i * 10 + 2) == 0)
        {
            continue;                                              // [objdump 0x88993d6 je]
        }
        if (*reinterpret_cast<const int*>(                            // [objdump 0x88993f6]
                reinterpret_cast<const char*>(&db) + 0x1b0 + i * 10 + 2) == 1)
        {
            m_slots1.insert(std::make_pair(                        // [objdump 0x8899461 target +0x84]
                *reinterpret_cast<const unsigned char*>(           // key=rec+1
                    reinterpret_cast<const char*>(&db) + 0x1b0 + i * 10 + 1),
                *reinterpret_cast<const _Slot*>(                   // +6
                    reinterpret_cast<const char*>(&db) + 0x1b0 + i * 10 + 6)));
        }
        else if (*reinterpret_cast<const int*>(                       // [objdump 0x88994a5]
                     reinterpret_cast<const char*>(&db) + 0x1b0 + i * 10 + 2) == 2)
        {
            m_slots2.insert(std::make_pair(                        // [objdump 0x8899503 target +0x9c]
                *reinterpret_cast<const unsigned char*>(
                    reinterpret_cast<const char*>(&db) + 0x1b0 + i * 10 + 1),
                *reinterpret_cast<const _Slot*>(
                    reinterpret_cast<const char*>(&db) + 0x1b0 + i * 10 + 6)));
        }
    }

    // buyItems：i<=0x31；key/shopType 读 m_buyItems[i].m_field0（db+0x229+i*12），
    // 计数条件读 m_field4（db+0x22d+i*12），value 取自 m_field4 起 12 字节。
    for (int i = 0; i <= 0x31; ++i)                                // [objdump 0x8899614]
    {
        // [L13-group6 判词⑩/P0 实验·续] 同 slots 段：ORIG 无 rec/count/itemKey/
        // itemValue 具名槽（仅保留 target = getBuyItemMap 单次调用结果，见 p167
        // CLEAR 的调用次数约束），逐处内联重算。
        if (*reinterpret_cast<const int*>(                         // [objdump 0x889955e +0xd→abs 0x22d]
                reinterpret_cast<const char*>(&db.m_buyItems) + i * 0xc + 4) <= 0)
        {
            continue;                                              // [objdump 0x8899563 jle]
        }
        std::map<int, _BuyItem>* target =
            getBuyItemMap((AdvanceAltarShopType::T)                // [objdump 0x889958d, +0x9→abs 0x229]
                          *reinterpret_cast<const int*>(
                              reinterpret_cast<const char*>(&db.m_buyItems) + i * 0xc));
        if (target == 0)
        {
            continue;                                              // [objdump 0x8899599 je]
        }
        target->insert(std::make_pair(                             // [objdump 0x88995c6 value=+0xd]
            *reinterpret_cast<const int*>(
                reinterpret_cast<const char*>(&db.m_buyItems) + i * 0xc),
            *reinterpret_cast<const _BuyItem*>(
                reinterpret_cast<const char*>(&db.m_buyItems) + i * 0xc + 4)));
    }

    // rewards：i<=0x1d（数组 29 项 + 第 30 次读取落入尾部 pad，忠实 ORIG 上限）；
    // key/value 同址取 m_rewards[i].m_field0。
    for (int i = 0; i <= 0x1d; ++i)                                // [objdump 0x88996c6]
    {
        if (*reinterpret_cast<const int*>(                         // [objdump 0x8899644 +1→abs 0x481]
                reinterpret_cast<const char*>(&db) + 0x480 + i * 5 + 1) <= 0)
        {
            continue;                                              // [objdump 0x8899649 jle]
        }
        m_rewards.insert(std::make_pair(                           // [objdump 0x889683]
            *reinterpret_cast<const int*>(
                reinterpret_cast<const char*>(&db) + 0x480 + i * 5 + 1),
            *reinterpret_cast<const _Reward*>(
                reinterpret_cast<const char*>(&db) + 0x480 + i * 5 + 1)));
    }
}