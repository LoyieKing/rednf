// [b28d-11b] 修法：it++（P28 增量侧）158→138；残余 N82 帧/槽 @0x086d14d4
// [b28d-solo-GameWorld] _ZN9GameWorld20UpdateServerSnapShotEv verdict=DIFF-N 186/147
// N-DEP 封顶（等价替代）：ORIG 用 boost::posix_time::second_clock::local_time() +
// gregorian::to_iso_extended_string + posix_time::to_simple_string（39 条 boost 链）。
// b28d 实测：真 boost 形（仓内 Library3rd/Boost/Include 头 + 已挂 -I）可编译且本件
// 186=186 指令数精确对齐（NDIFF 76 全槽号差），但 ORIG 的 boost 是预编译库产物
// （0x086d4419/0x086d28b5/0x086d288c/0x080cde16/0x080ce4bc 为独立 T 符号），头内
// 实现展开致 TU +273 符号、missing 140→227（+87）且本仓无 libboost 定义 ⇒ 越界，
// 维持 localtime_r+strftime 等价替代（同值语义）。P225 推广拆嵌套 if 已复现两条件
// 的 setne 物化（150→147）。帧 0xac vs 0xcc 系 boost 对象槽组。
// [L11·round1 判词更新] ndiff 91→**79**（186/147）。本轮修法 = P221/P227：具名
// `cMyTrace trace(...)` 改全表达式临时链。残余不可造（附 ORIG 0x086d14d4 段实证）：
//  ① p167 MISSING 三件 boost（`second_clock::local_time` / `to_simple_string` /
//     `to_iso_extended_string`）+ EXTRA `CSystemTime::getCurSec` —— 维持 N-DEP
//     （真 boost 形实测 TU missing_in_game 135→222，+82 个 C++11 ABI 别名仅NEW ⇒ 纪律禁）；
//  ② ORIG 六个实参值先落 -0x80..-0x6c 槽（4.1 具名局部）再统一装载，我方内联无槽：
//     属版本装载策略差（P244/P0 反向），补具名亦无法补回 boost 的 string 链。
// [H8B 2026-09-11 落地更新 · 部分推翻 ② 的"无法补回 string 链"判定] ORIG 的 string
// 链可**非 boost** 复现：date/time 改 `std::string` 承载 + `.c_str()` 实参即补回
// `_ZNKSs5c_strEv`×2 与 EH cleanup `_Unwind_Resume`（ORIG 0x086d16c8/0x086d16d9 的
// D2 位于 EH 路径）；而 cMyTrace 按 ORIG 0x086d1661 `lea -0x38(%ebp)` + 0x086d16b7
// `operator()` 改回**具名对象**（L11 那轮的全表达式临时链是反方向）。当前 224 vs
// ORIG 186：p167 MISSING 9→7（仅剩 boost 5 项 + cMyTrace C1/C2 别名），EXTRA =
// strftime×2 / time / localtime_r / `_ZNSsC1EPKcRKSaIcE`×2（非 boost 实现必然，
// ORIG 的日期/时间文本在 boost 库内生成，本函数内不含该 39 条链）；TU 四数零变化。
// [H8B 2026-09-11 终态取舍（父泳道裁决①）] 保留本 224 形，两条路径对照：
//   · 147 形（char[16] + strftime 直传）：**正 gap**（我方少 39）⇒ 缺 ORIG 有而我方无
//     的段（`_ZNKSs5c_strEv`×2 与 EH `_Unwind_Resume`），语义完成度低；
//   · 224 形（本文）：**负 gap −38**，多出的全是"我方有 ORIG 无"段（`time`/`localtime_r`/
//     strftime×2 + `_ZNSsC1EPKcRKSaIcE`×2）——这是「不发明符号」红线下的必然代价
//     （ORIG 的日期/时间文本在 boost 库内生成，本仓无 boost 库可链，见 4167-4182 判词）。
// N-DEP 归因（不可消除的 5 项）：`second_clock::local_time` / `base_time::date` /
// `base_time::time_of_day` / `to_simple_string` / `to_iso_extended_string`（跨版本 boost
// ABI；真 boost 头形实测 TU missing 135→222 ⇒ 违禁发明符号）。**时间源已对齐**：
// ORIG `second_clock::local_time()` = 当次**系统本地时间** ⇒ 我方改 libc `time(0)` +
// `localtime_r`（原先用 `GlobalData::s_systemTime_.getCurSec()` 缓存秒，语义不齐）。
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
            // [H8B 2026-09-11 复核·终判] 语义等价确认（同值语义）：我方
            // `strftime "%Y-%m-%d"` ≡ ORIG `to_iso_extended_string(date)`（RFC3339
            // extended "YYYY-MM-DD"）、我方 `strftime "%H:%M:%S"` ≡ ORIG
            // `to_simple_string(time_duration)`（24h "HH:MM:SS"；ORIG 时间源为
            // second_clock::local_time() 秒精度 ⇒ 无毫秒小数段）；两串以 `"%s %s"`
            // 顺序（date 在前）写入快照，与 ORIG 调用链 0x086d1565(second_clock::
            // local_time) → 0x080cde16(date) → 0x080ce4bc(time_of_day) →
            // 0x086d28b5(to_simple_string) → 0x086d288c(to_iso_extended_string) 的
            // 结果槽 -0x80/-0x7c 装载序一致。真 boost 形实测 TU missing_in_game
            // +87（82 个 C++11 ABI 别名 C5/D5）⇒ 违反「仅NEW 不得上升」⇒ 维持 N-DEP。
            // asm 桥路线（extern 声明 5 个 ORIG W 符号）经父泳道 2026-09-11 明确否决
            // （§4.5 禁 `extern "C" asm` 桥 + 引用 C5/D5 有链接期未定义风险）。
            // [H8B 2026-09-11 父泳道裁决④落地 · 时间源轴] ORIG 0x086d1565 调
            // `boost::date_time::second_clock<ptime>::local_time()`（= **当次系统本地
            // 时间**，非缓存值）⇒ 我方原 `GlobalData::s_systemTime_.getCurSec()`（缓存秒）
            // 语义不齐，改用 libc `time(0)` + `localtime_r`（两符号 ORIG 全库存在，
            // 不发明新符号；ORIG p167 EXTRA 少 `CSystemTime::getCurSec` 一项）。
            // [H8B 2026-09-11 父泳道裁决④落地 · p167 缺口补齐] ORIG 的日期/时间串是
            // **std::string** 具名临时（-0x48 = to_simple_string(time_of_day())、
            // -0x3c = to_iso_extended_string(date())，见 0x086d15b2/0x086d15d5 的
            // `call c_str`）⇒ 我方原 `char[16]` 直传缺 `std::string::c_str()`×2 与
            // `std::string::~string()`×2 + EH cleanup（`_Unwind_Resume`）⇒ 现改为
            // std::string 承载 + `.c_str()` 实参（ORIG 参数槽序实证：0x28(%esp)=date、
            // 0x2c(%esp)=time ⇒ 对应 `%s %s` 的先后）。boost 三项
            // （local_time/to_iso_extended_string/to_simple_string）维持 N-DEP。
            // [H8B 2026-09-11] cMyTrace 改**具名对象**：ORIG 0x086d1661 `lea -0x38(%ebp)`
            // 后 `call cMyTrace::cMyTrace(char const*, int, int)` 再
            // `call cMyTrace::operator()(char const*, ...)`（0x086d16b7）⇒ 是栈上具名
            // trace 对象（非临时链）。
            time_t now = time(0);
            struct tm local;
            localtime_r(&now, &local);
            char dateBuf[16];
            char timeBuf[16];
            strftime(dateBuf, sizeof(dateBuf), "%Y-%m-%d", &local);
            strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", &local);
            std::string dateStr(dateBuf);
            std::string timeStr(timeBuf);
            cMyTrace trace(__PRETTY_FUNCTION__, 0x1b61, 8);
            trace("%u,%d,%u,%d,%d,%d,%d,%d,%s %s",
                  user->get_acc_id(),
                  G_CEnvironment()->get_server_group(),
                  user->getCurCharacNo(),
                  (int)user->getCurCharacVill(),
                  user->get_area(false),
                  dungIdx,
                  user->getCurCharacExp(),
                  user->getCurCharacMoney(),
                  dateStr.c_str(), timeStr.c_str());
        }
        }
    }
}