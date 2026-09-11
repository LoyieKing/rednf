// ============================================================================
// saveEventData —— ORIG T 0x08166d38（saveEventData.md 全文转写）。
// 判活 → user 判空 → charac_no 非 0 → m_values.find(charac_no) 不存在则 log
// 0x269 返回；存在则逐 HeroMissionValue getStringToken 拼 "," 分隔串；
// Acquire(0x271) → CStreamGuard(true) → Taiwan::internal_stream(guard, T 0x11,
// user->GetUID()) → GetInBuffer<SigSaveHeroMissionData>()；空 → log 0x276；
// 否则 sig->init()；*(int*)sig = charac_no；strncpy(sig+0x04, data, len)；
// s_msgq_mgr->put(QUEUE_IDX(2), guard)；removeData 为真再 erase(charac_no)。
// （ORIG try/catch 形态由栈对象自动析构等价承载。）
// ============================================================================
// saveEventData @ 0x08166d38 —— [b28d-solo-HeroMissionEvent · N95 封顶]
//   insn 225=225、帧 0x60 相同、p167 CLEAR；全部局部槽整体深 4
//   （ORIG -0xc/-0x20/-0x24/-0x2c/-0x30 ↔ OURS -0x10/-0x24/-0x28/-0x30/-0x34），
//   属 4.4 局部槽基/分配序跨版本（N95）。P259 删 stream 局部实测 gap 0→2 反劣
//   （已回退）；data/characNo 两声明序均试不闭合。
// [L3EHeroItemUpgrade 2026-09-11] 复核前述 P259 结论并**翻案**（判据改按 ndiff）：
//   ① 0x08166f0d..0x08166f24：StreamPool::Acquire 返回值 `%eax` **直传**
//      CStreamGuard ctor 第 2 实参（`movl $0x1,0x8(%esp); mov %eax,0x4(%esp)`）
//      ⇒ 删 `Stream* stream` 具名局部；
//   ② 0x08166fda..0x08166fe8：strncpy 实参处才 `mov -0xc(%ebp),%edx; add $0x4,
//      %edx`（-0xc 即 sig 本身）⇒ 删 `char* dst` 具名局部。
//   实测：our 227 → **223**、**ndiff（= 工具 n_diff 编辑距离口径）57 → 19**、
//   ratio .771 → .929，TU 四数不变 ⇒ 按「gap 或 ndiff 严格改善」判据**保留**
//   （gap 由 0 变 +4 属 insn 计数差，非结构劣化：残余仅 logFormat 串在 .o 中为
//   .rodata 重定位（工具渲染差）、0x08166f93 `mov $0x0,%ebx` 与 0x08166ffe
//   `mov %eax,%edx` 系 4.1 返回/寄存器约定（N7）、尾对齐 nop）。
void HeroMissionEvent::saveEventData(CUser* user, bool removeData)
{
    // ORIG 0x08166d5e `call *%edx`：虚调用 CEventBase::IsEventing(NULL)（§4.5）。
    if (IsEventing(0) == 0)
    {
        return;
    }
    if (user == 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__, 0x25e,
            "[HeroMission] User is null.");
        return;
    }
    if (user->get_charac_no(-1) == 0)
    {
        return;
    }

    // [b28d-solo-HeroMissionEvent] ORIG 0x08166d8e 先构造 `std::string data`
    // （-0x20 槽，call _ZNSsC1Ev）再 call get_charac_no；槽序 characNo(-0x1c)
    // 浅于 data(-0x20) ⇒ 4.4「先声明浅」要求 characNo 先声明、赋值分两句。
    unsigned int characNo;
    std::string data;
    characNo = (unsigned int)user->get_charac_no(-1);
    std::map<unsigned int, std::vector<HeroMissionValue> >::iterator it =
        m_values.find(characNo);
    if (it == m_values.end())
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__, 0x269,
            "[HeroMission] Not exist mission value. (charac:%u)",
            user->get_charac_no(-1));
        return;
    }

    for (std::vector<HeroMissionValue>::iterator vit = it->second.begin();
         vit != it->second.end(); ++vit)
    {
        vit->getStringToken(data);
    }

    // [L3EHeroItemUpgrade 2026-09-11] ORIG 0x08166f0d..0x08166f24：Acquire 返回值
    // `%eax` 直传 CStreamGuard ctor 的第 2 实参（`movl $0x1,0x8(%esp);
    // mov %eax,0x4(%esp)`），**无具名 stream 槽** ⇒ 删具名局部（P183/P48）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire(
            "localtaiwan/Event/EventHeroMission.cpp", 0x271),
        true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(0x11),
                            static_cast<int>(user->GetUID()));
    Taiwan::SigSaveHeroMissionData* sig =
        guard.operator->()->GetInBuffer<Taiwan::SigSaveHeroMissionData>();
    if (sig == 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__, 0x276,
            "[HeroMission] SigSaveHeroMissionData is null.");
    }
    else
    {
        sig->init();
        // [b28d-solo-HeroMissionEvent] ORIG 0x0816701e/0x08167037 各再调一次
        // user->get_charac_no(-1)（写 sig 与 erase 键），非复用局部 characNo。
        *reinterpret_cast<unsigned int*>(sig) = user->get_charac_no(-1);
        // [L3EHeroItemUpgrade 2026-09-11] ORIG 0x08166fda..0x08166fe8 在 strncpy
        // 实参处才 `mov -0xc(%ebp),%edx; add $0x4,%edx`（-0xc 即 sig 本身）⇒
        // 源码无具名 dst 局部（P183）。
        strncpy(reinterpret_cast<char*>(sig) + 4, data.c_str(), data.length());
        GlobalData::s_msgq_mgr->put(
            static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
        if (removeData)
        {
            m_values.erase(user->get_charac_no(-1));
        }
    }
}