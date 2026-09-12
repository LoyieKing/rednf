bool CItemGeneratorMgr::initialize()
{
    // ORIG 0x08533bc8 逐段转写（docs/class_func_reports/WongWork__CItemGeneratorMgr/
    // initialize.md）：new 八个生成器 → 前 5 个 setGeneratorType(0..4) → 依序装载
    // 脚本（任一返回行号非 0 即 throw int(line)）→ 8 生成器共享内嵌公共数据 →
    // memset 头部 0x3c0 → 注册统计定时器 → setDropRatio(1.0)。catch 打印后返 0。
    // b28d-lane12: P225 `while (line != 0) { throw line; break; }` 复现 ORIG 8 处
    //   `cmpl $0,-0x24(%ebp); setne %al; test %al,%al; je`（0x08533eb9 / 0x08533f18 /
    //   0x08533f77 / 0x08533fd1 / 0x08534030 / 0x0853408a / 0x085340e4 / 0x08534135
    //   实证；4.4 裸 if 产直 je，探针 /tmp/d12_setne_probe.cpp fB 逐条同形）⇒
    //   ndiff 178→130、gap 24→8。残余 N 类封顶（P167 差集仅 _Unwind_Resume×8，
    //   业务调用 0 缺口；ORIG 432 insn / ours 424）：
    //   ① 8 处 new 失败 cleanup ORIG 独立 `mov %eax,(%esp); call _Unwind_Resume`
    //      （0x08533c03 / 0x08533c4a / 0x08533c91 / 0x08533cd8 / 0x08533d1f /
    //      0x08533d66 / 0x08533dad / 0x08533df4），4.4.7 合并为共享 landing pad
    //      `jmp <0x8d6>`（cmp $1,%edx / je catch）——N104 族 EH 块合并，无源码形可控；
    //   ② 4 处虚调用目标寄存器 ORIG `mov (%eax),%edx … call *%edx`
    //      （0x08533efd / 0x08533f5c / 0x08534015 / 0x0853404b），4.4.7 恒 %eax——
    //      N7（P230：4.4.6 后端目标用 %edx，本 TU 未路由；换路由属 Main 决策）；
    //   ③ catch handler 相对序：ORIG 居中（0x0853416f）正常尾块置末，4.4 尾置——
    //      N104；
    //   ④ for-i 与 catch-line 槽互换（ORIG i=-0x1c / line=-0x20，4.4 相反）——
    //      N93/N98 首次使用序；i 提函数头/try 块头两形实测 130→308 劣化。
    // R18a07 ④ 修复：ORIG 的 try 只覆盖脚本装载段（8 处 new 的 cleanup landing
    // 0x08533c03/c4a/c91/cd8/d1f/d66/dad/df4 均为 `mov %eax,(%esp); call
    // _Unwind_Resume`，cleanup-only、无类型过滤）⇒ new/setGeneratorType 段必须在
    // try 之外，否则 new-expression 抛 int 时会被 catch(int) 吃掉并 return false。
    m_generators[0] = new CMonsterDrop(3);   // operator new 0x2593a4
    m_generators[1] = new CObjectDrop(3);    // 0x2593a4
    m_generators[2] = new CClearReward(5);   // 0x23b84
    m_generators[3] = new CWorldDrop(2);     // 0x1f80
    m_generators[4] = new CDungeonDrop(2);   // 0x18
    m_generators[5] = new CWorldDrop(2);     // 0x1f80
    m_generators[6] = new CWorldDrop(2);     // 0x1f80
    m_generators[7] = new CWorldDrop(2);     // 0x1f80

    m_generators[0]->setGeneratorType(0);
    m_generators[1]->setGeneratorType(1);
    m_generators[2]->setGeneratorType(2);
    m_generators[3]->setGeneratorType(3);
    m_generators[4]->setGeneratorType(4);

    try
    {

        // 怪物掉落三脚本（ORIG 文件名「Monseter」拼写如此，忠实保留）。
        // 8 处 loadScript 检查 ORIG 均为 cmpl $0,-0x24(%ebp); setne %al; test %al,%al;
        // je（0x08533eb9 / 0x08533f18 / 0x08533f77 / 0x08533fd1 / 0x08534030 /
        // 0x0853408a / 0x085340e4 / 0x08534135 实证）；4.4 裸 if 产直 je，P225 的
        // `while (line != 0) { throw line; break; }` 精确复现 setne 物化形。
        int line = static_cast<CMonsterDrop*>(m_generators[0])->loadScript(
            "Etc/ItemDropInfo_Monseter.etc",
            "Etc/ItemDropInfo_Monster_Hell.etc",
            "Etc/ItemDropInfo_Monseter_Extra.etc");                       // T 0x085380fa 直呼
        while (line != 0)
        {
            throw line;
            break;
        }
        line = m_generators[1]->loadScript("Etc/ItemDropInfo_Object.etc");        // 虚槽 +0x04
        while (line != 0)
        {
            throw line;
            break;
        }
        line = m_generators[2]->loadScript("Etc/ItemDropInfo_ClearReward.etc");   // 虚槽 +0x04
        while (line != 0)
        {
            throw line;
            break;
        }
        line = static_cast<CWorldDrop*>(m_generators[3])->loadScript(
            "Etc/WorldDrop.etc", "Etc/WorldDropWarArea.etc");             // T 0x0853855c 直呼
        while (line != 0)
        {
            throw line;
            break;
        }
        line = m_generators[4]->loadScript("");   // DAT_08c99500 == ""（虚槽 +0x04）
        while (line != 0)
        {
            throw line;
            break;
        }
        line = static_cast<CWorldDrop*>(m_generators[5])->loadScript(
            "Etc/WorldDropPCRoom.etc", "Etc/WorldDropWarAreaPCRoom.etc");
        while (line != 0)
        {
            throw line;
            break;
        }
        line = static_cast<CWorldDrop*>(m_generators[6])->loadScript(
            "Etc/WorldDropPCRoom2.etc", "Etc/WorldDropWarAreaPCRoom.etc");
        while (line != 0)
        {
            throw line;
            break;
        }
        line = m_common.loadScript("Etc/ItemDropInfo_Common.etc");  // 内嵌 @+0x3e0
        while (line != 0)
        {
            throw line;
            break;
        }

        for (int i = 0; i < 8; ++i)
            m_generators[i]->setCommonData(&m_common);

        memset(m_head, 0, 0x3c0);   // 头部数据区清零（生成器指针/公共数据区不受影响）

        TimerUpdateItemGenerateStatistics::registNextTimer(
            GlobalData::s_systemTime_.getCurSec());
        setDropRatio(1.0f);
        return true;
    }
    catch (int line)
    {
        // ORIG __PRETTY_FUNCTION__ 字符串 0x08c9a69c 实证为
        // "bool WongWork::CItemGeneratorMgr::initialize()"；trace 行号 0x19a、flag 5。
        cMyTrace(__PRETTY_FUNCTION__, 0x19a, 5)("script load fail. line(%d)", line);
        return false;
    }
}