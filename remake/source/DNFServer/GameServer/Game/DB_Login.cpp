// ============================================================================
// [b28d-38 判词] 本 TU 27 件最终状态（compare_tu_game_orig 口径，ORIG 地址见报告）：
//  CheckLoginIP 0x08415472 AE（SGB->SGBC 宏形 + std::strrchr _ZSt7strrchrPci）
//  CreateAccount 0x08412eb8 NEAR（N18a/P75：hangame_flag 实参 movzbl 目标寄存器/装载序，7 形探针）
//  GetUserFatigueCount 0x084155xx DIFF（N7 间接 call 目标 4.4.7 恒 %eax + 装载序；p167 CLEAR）
//  GetUserInGameEventHistory 0x08415xxx DIFF（P30/N4 add-vs-lea 常量折叠；p167 CLEAR）
//  _isPunishedHackUser 0x0842xxxx DIFF（N82 槽/帧；p167 CLEAR 10/10）
//  RemovePreminuInfo 0x084128c4 DIFF（const_iterator 双端转换已修，消混合 operator!=；残差槽/物化）
//  UpdateLoginStat 0x0840xxxx DIFF（N88 惰性求值位置+槽；p167 CLEAR 13/13）
//  GetGameDBInfo 0x08414772 DIFF（setcc 物化/N88；p167 CLEAR 5/5）
//  GetLoginCommonData 0x0841090c DIFF（SGB->SGBC 已修；余 setcc 物化 P197④）
//  getCharacLinkSystemData 0x0840xxxx DIFF（块序/N93；p167 CLEAR 16/16）
//  _LoadEventItem 0x08411d7e DIFF（p167 缺迭代器 begin/end/++/->，我方为索引形——待改）
//  GetOntimeUserRecvIdx 0x0841xxxx DIFF（N7+装载序；p167 CLEAR 10/10）
//  IsThisIPLoginPermissionGranted 0x0844b982 NEAR（SGBC/SGFC 宏形已修 116=116；N82 槽）
//  _LoadDNFEventEntry 0x08411bda DIFF（EXTRA tagDNFEventEntry::~ x2+_Unwind_Resume：EH 尾段 N91/N104）
//  _DecryptUserInfo 0x0841300e DIFF（p167 MISSING strlen x2 语义缺口）
//  GetAccountCharacterLevelUpOnceGift 0x0840xxxx DIFF（N7/setcc；p167 CLEAR 11/11）
//  GetBadgeCount 0x0840xxxx DIFF（N82 槽/帧；p167 CLEAR 16/16）
//  GetPunishUserInfo 0x0840xxxx DIFF（setcc/N82；p167 CLEAR 14/14）
//  _getGameDBCharacView 0x084149ce DIFF（SGBC 宏形已修；N82 槽；p167 CLEAR 13/13）
//  GetGrowthCreatureEvent 0x0840xxxx DIFF（N7/setcc；p167 CLEAR 13/13）
//  GetBingoEvent 0x08448bee DIFF（N82 槽；p167 CLEAR 22/22）
//  _getRewardInfo 0x0840xxxx DIFF（N82/P183；p167 CLEAR 18/18）
//  GetMercenarySystemInfo 0x08415618 DIFF（N82；p167 CLEAR 27/27）
//  getBreakAwayMemberData 0x0840xxxx DIFF（N82；p167 CLEAR 44/44）
//  GetPremiumInfo 0x0840xxxx DIFF（p167 MISSING operator-> x21：需去 (*it). 缓存改 it-> 形）
//  dispatch 0x08425374 DIFF（p167 MISSING G_GameWorld() x1 语义缺口）
//  GetAccountInfo 0x0842xxxx DIFF（p167 MISSING NumberToString x2 + EXTRA memset x7 语义缺口）
// ----------------------------------------------------------------------------
// [b28d-solo-DB_Login 最终判词] 单文件泳道收口（check_tu_routed.sh + tu_diff gap 口径）。
// TU 计数：identical=48 ae=129 near=1 diff=19（含非清单件 CCompressedBlob::resize）；
// 本清单 27 件 = AE/IDENTICAL 8 + NEAR 1 + N 封顶 18 + 未解决 0。TU COMPILE OK 0 错误。
//  p167 差集全部 CLEAR（27/27；CheckLoginIP 的 strrchr 符号差除外，见下）。
//  AE/IDENTICAL（8）：
//   CheckLoginIP 0x08415472 AE（SGBC 宏形 + std::strrchr；p167 `_ZSt7strrchrPci` vs
//       `strrchr` = 工具链头 `__CORRECT_ISO_CPP_STRING_H_PROTO` 版本差 N，语义等价）
//   _LoadEventItem 0x08411d7e AE（P167 迭代器化；真语义 bug：Arrange 第二实参
//       ORIG 为 serverId 0x10(%ebp)，旧码误传 &entries）
//   RemovePreminuInfo 0x084128c4 AE（P245 去 static 整帧回正 + P232 dead int 槽 +
//       begin→end 调序 + P225 continue 形复现 exec 后 dead 读 sig+0x2938）
//   _LoadDNFEventEntry 0x08411bda AE（P12/N84 删 tagDNFEventEntry 用户空析构消 2×D1
//       +_Unwind_Resume；for(i<get_n_rows()&&fetch()) 拆 body；P223 五独立 nop 岛；
//       P209 unsigned i 复现 seta）
//   IsThisIPLoginPermissionGranted 0x0844b982 AE（三兄弟块 trace 全表达式临时链 P221/P227）
//   GetLoginCommonData 0x0841090c AE（rows 具名槽 + col++ 递增索引 P191/P0）
//   GetBadgeCount 0x08414512 AE（get_short 直传 sig->m_dailyBadge[i]；P227 链式 trace）
//   _isPunishedHackUser 0x08412ac8 AE（expireSec int 化 P217；if(nowFlag==0)return 0 极性）
//  NEAR/N 封顶（19）：
//   CreateAccount 0x08412eb8 NEAR gap=2（N18a：hangame_flag 实参 4.4 恒 movzbl→%eax
//       中转；P267 双 cast / P268 mask / 字段 uchar 三档试尽不翻）
//   GetUserFatigueCount 0x084150e8 DIFF gap=5（N7 间接 call 目标 4.4.7 恒 %eax + 装载序）
//   GetUserInGameEventHistory 0x084152b0 DIFF gap=9（N1：ORIG `add $0x3fa0; add sig;
//       add $0xc` 三段被 4.4 折 $0x3fac+lea；十形探针既定）
//   _DecryptUserInfo 0x0841300e DIFF gap=6（N7/P75 实参装载序；p167 CLEAR）
//   UpdateLoginStat 0x08414d50 DIFF gap=4（N18a：三条 store 的 RHS-first 中转；
//       P259 消 stream 局部 + P82 异型视图已修，余中转封顶）
//   GetGameDBInfo 0x08414772 DIFF gap=13（N18b 内存直比 + P30/N4 数组寻址序；
//       P0 去 world 局部/P222 正极性包裹/P209 unsigned viewRet/P28 声明序已修）
//   getCharacLinkSystemData 0x0840ffca DIFF gap=1（P223/N：尾 `jmp exit` 岛不可造；
//       P168a 极性翻转 + P227 链式 trace 已修，26→1）
//   GetOntimeUserRecvIdx 0x08414f5c DIFF gap=6（N18a：除法中间值落 -0xc 槽；
//       P48/P0 去 mid 缓存已修，32→6）
//   GetAccountCharacterLevelUpOnceGift 0x0844834e DIFF gap=27（N82 槽/帧；P0/P140 去
//       evt 局部与判空、P158 bool 槽、P222 正极性包裹已修，56→27）
//   GetPunishUserInfo 0x08410cf6 AE（本波修：+0x3a50 双视角 union，见函数头）
//   GetGrowthCreatureEvent 0x08448102 DIFF gap=35（N82 槽/帧；P0/P140/P158/P222 已修，66→35）
//   _getGameDBCharacView 0x084149ce DIFF gap=3（N4/P30 数组寻址 add-vs-lea；
//       P253 三缓冲 `={0}` rep stos 清零 + P87/P227 链式 trace 已修，32→3）
//   GetBingoEvent 0x08448bee DIFF gap=4（N7/N4 尾残差；P0/P140/P198/P227 已修 86→4；
//       真语义 bug：ORIG 成功路径落 08448eac `mov $0` ⇒ return 0）
//   _getRewardInfo 0x084115f8 DIFF gap=177（N82 槽/帧 + N78 memset 展开；p167 CLEAR 18/18）
//   GetMercenarySystemInfo 0x08415618 DIFF gap=169（N82 槽/帧 + P30/N4；p167 CLEAR 27/27）
//   getBreakAwayMemberData 0x084101ae DIFF gap=105（N78 对齐 memset 展开 + N82 帧；
//       p167 CLEAR 44/44）
//   GetPremiumInfo 0x08411ee8 DIFF gap=256（N82 槽/帧；P157 去 eventId 缓存后
//       p167 CLEAR 84/84）
//   dispatch 0x08425374 DIFF gap=374（N82 帧 0x40↔0x90；p167 CLEAR 56/56）
//   GetAccountInfo 0x0841325e DIFF gap=455（N78 memset 展开 + N80 varargs + N82 帧；
//       p167 CLEAR 90/90：补 get(7)/get(9) trace 的 NumberToString 语义缺口 x2 +
//       8 处 memset 改 `={0}` 消 EXTRA memset x7）
// 真语义 bug 修复（3）：_LoadEventItem Arrange 第二实参、GetBingoEvent 成功返回值、
//   GetAccountInfo 两处 trace 的 NumberToString 实参。
// .h 改动（2，均已 R2 广播）：DB_Login.h 三函数去 static（P245）；tagDNFEventEntry
//   删用户空析构（P12/N84）。SigTypes_D.h 的 m_loginType uchar 试验已回退（无收益）。
// ----------------------------------------------------------------------------
// [b28d-solo-DB_Login 本轮续修] 20 件清单复核 + 新范式（P307–P320/N115–N124）回扫：
// TU 计数 identical=48 ae=130 near=2 diff=17（含非清单件 CCompressedBlob::resize）；
// 本清单 20 件 = AE 2（CheckLoginIP/UpdateLoginStat）+ NEAR 2（CreateAccount/
// _getGameDBCharacView）+ N 封顶 16 + 未解决 0。TU COMPILE OK 0 错误。
// 本轮改法（6 件，零 .h 改动、零 ABI/mangled 影响）：
//  UpdateLoginStat DIFF(4)→AE：P211 档③ PktView 字段同型化（m_accId→unsigned int、
//      m_gcNo→int）⇒ RHS 直装 %edx 无 eax 中转（探针 probe_store f2/f5）。
//  _getGameDBCharacView DIFF(5)→NEAR(2)：P311 具名 Slot 数组视图（CharacDataView，
//      文件头匿名 namespace）复现 `imul $0x94; lea; add $0xe0` 三步；余 2 行 = i/sig
//      装载序（N82附/P112）。
//  GetBingoEvent DIFF(5)→DIFF(2)：P0 resetBase 声明 unsigned 修实参装载时刻（ORIG
//      NumberToString 后重载 -0x18）；余 N107 lea-vs-add。
//  GetGameDBInfo DIFF(20)→DIFF(17)：P311 槽视图修循环数组寻址；余 N-P209L。
//  GetAccountCharacterLevelUpOnceGift DIFF(32)→DIFF(15)：P0/P183 具名 int* 局部
//      （ORIG -0xc 槽两次回读）替 jobType/level 双局部 + 两次写回。
//  GetGrowthCreatureEvent DIFF(37)→DIFF(17)：P0/P183 具名 char* 行指针（ORIG -0x10）
//      + P232 死 int 局部（ORIG -0xc），四个 getter 直写字段。
// 新负例/判据（已随各函数头判词登记）：
//  N-P209L：具名局部与常量 `==` 比较 4.4 恒折 `cmpl $imm,mem`；P310 的 `<` setcc 形
//      不适用（unsigned/int/long+显式提升+L 后缀四形实测）。
//  N-P133 回退：拆独立守卫（exec/get_n_rows）实测 244→254 反劣，保持合并短路。
//  N107 再证：cast 取址恒 add-vs-lea；同函数无 cast 字段（+0x4138）则 lea 可复现。
//  N1 分配律：(i+0x866)*8+8 被 4.4 折 (i+0x867)*8（probe_gift f5）。
//  N121/P223 边界：尾 `test; jmp exit` + nop 岛，probe_tail fA–fF 六形全败。
// 待 Main 裁决：GetUserFatigueCount 的 N7（4.4.7 间接 call 目标恒 %eax）属后端敏感
//  族——若本 TU 路由到 4.4.6 可试翻，但影响整 TU 其余 177 件，不自行改路由表。
// 探针工件：/tmp/b28d_solo/{probe_cast2,probe_pun,probe_arr,probe_gift,probe_tail,
//   probe_jmp2,probe_store}.cpp（4.4.7 -m32 -O0 实测）。
// 无子子代理（R4 合规）。
// ----------------------------------------------------------------------------
// [S216_DB_Login2 单文件泳道收口] 19 件清单（/tmp/b28d_tu/DB_Login.tsv）最终状态
// （check_tu_routed.sh .o 口径 + show_func_diff ndiff + p167 差集）：
// TU 计数 identical=48 ae=133 near=2 diff=14（含非清单件 CCompressedBlob::resize）；
// 本清单 19 件 = AE/IDENTICAL 4 + NEAR 2 + N 封顶 13 + 未解决 0。COMPILE OK 0 错误。
// p167 调用点差集 19/19 CLEAR（CheckLoginIP 的 `_ZSt7strrchrPci` vs `strrchr` 头版本差除外）。
// 本轮新收 AE（3）：
//   _getGameDBCharacView NEAR(2)→AE：P235 数组视角（SigTypes_D.h 匿名 union
//       m_characView）⇒ 复现 `mov i;mov sig;imul $0x94;lea;add $0xe0` 装载序。
//   GetOntimeUserRecvIdx DIFF(8)→AE：`int mod = (int)(sig->m_accId % 10u);`
//       （P217/P310 类型旋钮）⇒ 4.4 除法 magic 中间值不再 spill 到 -0xc。
//   GetUserInGameEventHistory DIFF(18)→AE：P235/P311 历史表视角
//       （SigTypes_D.h m_ingameEventHistoryView）⇒ 复现 `add $0x3fa0;add sig;add $0xc` 三步链。
// 本轮新收 NEAR（1）：
//   GetUserFatigueCount DIFF(10)→NEAR(8)：NumberToString 内联进 set_query 实参 ⇒
//       occDay `movzbl -0x11(%ebp),%esi` 装载时刻回到 ORIG 位（余 N7 间接 call 目标
//       4.4.7 恒 %eax，4.4.6 后端可翻，属 Main 路由裁决）。
// 本轮实质改善（2）：
//   GetGameDBInfo DIFF(22)→DIFF(6)：P235 视角修 memmove/比较地址链 + int==unsigned
//       异型比较修两段装载；余 N-P209L（`==` 常量比较恒折 cmpl mem；switch 双 case
//       形 gap 0 但 ndiff 8、显式 (unsigned) 提升形均不翻，已实测）。
//   _getRewardInfo DIFF(420)→DIFF(382)：P10/P248 档② flags/latest char→int（`orb`→`orl`）、
//       拆独立守卫 + 空 then 正向形修 execOk 槽物化、`time(&now)` 传址形修 time 调用形态；
//       余 N82 帧 0x10 + N-P133 第二守卫 setne/je 极性 + N78 段。
// N 封顶复核（13 件，逐件范式试尽后维持）：
//   CreateAccount NEAR(4) N18a/P75：P267 双 cast 序 / P268 mask / P226 语句表达式三形
//       本轮再测均不翻（movzbl %al,%eax vs ORIG %edx）。
//   getCharacLinkSystemData DIFF(1) N121：P285 正/负条件空 then+else return 两形实测
//       （正形反劣 gap2，负形与原形逐条同）⇒ epilogue-threading 尾 jmp+nop 岛封顶。
//   _DecryptUserInfo DIFF(12) N7/P75：Hex2Binary/Decrypt 第三实参 cast 互换四形不翻。
//   GetBingoEvent DIFF(3) N107：union{unsigned;int} 同址 int 视图实测仍产 add+mov
//       （探针 s216_probe2 fA/fB）⇒ 成员常量取址 lea-vs-add 封顶。
//   GetPunishUserInfo DIFF(5) N18a/P211：RHS `(unsigned int)get_n_rows()` cast 形实测
//       仍 LHS-first（4.4 折叠同宽 cast）⇒ 字段侧 unsigned 化牵动三泳道，维持封顶。
//   GetAccountCharacterLevelUpOnceGift DIFF(30) N1/N115：P205 显式拆形
//       `(i+0x866)*8+8` 实测仍折 `(i+0x867)*8`。
//   GetGrowthCreatureEvent DIFF(34) N1/N115：同上（0x4150+4 折 0x4154）。
//   GetMercenarySystemInfo DIFF(447) N82/N4：帧 0x60↔0x70、槽 -0x14↔-0x1c、
//       数组 add-vs-imul/lea；setl↔setle 为极性镜像（P196 折痕，非真反）。
//   getBreakAwayMemberData DIFF(272) N78/N82：ORIG `push esi;push ebx;add $N,%esp`
//       大帧 vs 4.4 `sub $0x98`；N78 对齐 memset 展开。
//   GetPremiumInfo DIFF(711) N82：p167 84/84 CLEAR（旧判词 operator-> MISSING 已消）。
//   dispatch DIFF(1014) N82：794 insn 超大口径（§6.2 三件套），p167 56/56 CLEAR，
//       帧 0x40↔0x90、ORIG shl/lea/add 展开族 vs 4.4 setne/sar。
//   GetAccountInfo DIFF(1190) N78/N80/N82：1045 insn 超大口径，p167 90/90 CLEAR。
// P0：本文件 0 处 `register ... asm("reg")` 绑定（清绑历史见 GetGrowthCreatureEvent 头注释）。
// .h 改动（2 处，均已 R2 广播且消费方复测零回归）：SigTypes_D.h
//   ①SIG_LOGIN_DATA::m_characData → 匿名 union + m_characView 数组视角；
//   ②+0x3fa0 区 8 成员 → 匿名 union + m_ingameEventHistoryView 历史表视角。
//   两处 sizeof 0x4358 / 全部偏移 / mangled 零变化；消费 TU = DB_Login.cpp + InterDispatcher.cpp。
// 无子子代理（R4 合规）。
// ----------------------------------------------------------------------------
// [b28d-PoolDBLogin 池批·本轮] ≤30 小件 6 件复核（check_tu_routed.sh 现编现测）。
// TU 三数 基线 identical=47 ae=133 near=2 diff=12 → 现在 identical=47 ae=134
// near=2 diff=12（+1 ae = 新增库包装 `_ZSt7strrchrPci` 自身 10/10 AE）。
//  修成（1）：CheckLoginIP ndiff 2→0（P306 族：extern "C" + `__asm__("_ZSt7strrchrPci")`
//      + `weak` 的同体转发包装，体逐条 = ORIG 0x08265244）。直写 `namespace std
//      { inline char* strrchr(char*, int) }`（先例 TU 形）在本 TU **编译失败**：
//      DB_Login.h/GlobalData.h 链必然引 <cstring> ⇒ `using ::strrchr;` 已注入 std
//      （`char* strrchr(char*, int)`）与同签名定义冲突；删本 TU 的 <cstring> 亦不解决
//      （链上来）。两条失败形均已回退。
//  维持封 N（5，逐件按 P321 T1–T5/S1 复核，残差均落已登记 N 族，无一新形）：
//   CreateAccount NEAR(4)：N18a/P75 —— `movzbl 0xa0(%eax)` 后 ORIG 值留 %edx
//     （`movzbl %al,%edx` + 延后 store），4.4 恒 %eax 中转即存；T1 直证字段 uchar，
//     残差纯寄存器角色 ⇒ T5 禁改类型。S1 旋钮（P267 双 cast / P268 mask / P226
//     语句表达式 / 字段 uchar 三档）前轮已试尽。
//   GetUserFatigueCount NEAR(8)：N7 —— 间接 call 目标 ORIG 落 %edx、4.4.7 恒 %eax
//     （`mov (%eax),%eax; call *%eax`），属后端版本族；本 TU 不在 446 路由表 ⇒
//     维持「待 Main 路由裁决」，泳道不改路由。
//   GetGrowthCreatureEvent DIFF(10)：N1 —— ORIG `mov i; imul $0x2c; add $0x4150;
//     add 0xc(%ebp); add $0x4`（6 条）vs 4.4 `mov sig; mov i; imul; add $0x4154;
//     add`；本轮逐条复核**语义等价**（0x4150+0x4 = OFF_GROWTH_CREATURE 0x4154，
//     四 getter 目标 sig+0x4154/0x4158/0x415c/0x4160 与 ORIG 0x08448251..0x084482db
//     完全一致）⇒ 非真语义 bug，仅常量拆分/装载序；P311 数组视图前轮实测 7>6 劣化
//     回退 ⇒ S1 已耗尽。
//   GetAccountCharacterLevelUpOnceGift DIFF(10)：N1 —— 同上（`add $0x866; shl $3;
//     add sig; add $0x8` = (i+0x867)*8，与 OFF_ONCE_GIFT 0x4338 等价）；P205 显式
//     拆形前轮实测仍折 ⇒ 封顶。
//   getCharacLinkSystemData DIFF(1/gap1)：N121 —— 唯一残差 = ORIG 尾 `test %al,%al;
//     jmp exit` + 0x084101a7 nop 岛（4.1 为空 else 发射的中继），4.4 恒 DCE；
//     probe2/probe3 十四形 + P285 两形试尽 ⇒ 封顶。**旧判词 N93 已失效**（本件无
//     任何迭代器，与 Main P326 无关）。
//  Main P326（N93 翻案）在本文件的适用性：0 件。全文件仅 `_LoadEventItem` /
//  `RemovePreminuInfo` 用双具名迭代器，两件现状均 AE（p167 CLEAR，无槽互换残差）；
//  `GetPremiumInfo` 的 for(begin/end) 件 ndiff 639 > 30，不属本批范围。
//  P0：本轮新增 0 处 `register ... asm("reg")` 绑定；无 .h 改动（R2 无申报项）。
// 探针/证据：/tmp/p_bridge.o（asm 标签 weak 形实测 W + 8 条体 = ORIG）；
//  nm /tmp/tu_DB_Login.o | grep strrchr ⇒ `W _ZSt7strrchrPci`。
// ----------------------------------------------------------------------------
// [S414_DB_Login 单文件泳道收口] 16 件清单（/tmp/b28d_tu/DB_Login.tsv）最终状态
// （check_tu_routed.sh .o 口径 + /tmp/solo_diff.py ndiff + p167 差集）：
// TU 计数 identical=48 ae=134 near=2 diff=13（基线 ae=133/diff=14，+1 AE）。
// 本清单 16 件 = AE 2（CheckLoginIP/GetGameDBInfo）+ NEAR 2（CreateAccount/
// GetUserFatigueCount）+ N 封顶 12 + 未解决 0。COMPILE OK 0 错误。
// p167 16/16 CLEAR（CheckLoginIP 的 `_ZSt7strrchrPci` vs `strrchr` = 4.4.7 头无
//   std::strrchr C++ 重载，语义等价，已知豁免）。
// 本轮新收 AE（1）：
//   GetGameDBInfo 0x08414772 DIFF(6)→AE：P286 单 case switch——两处 `== 0xffffffff`
//       /`== 1` 由 `switch (viewRet) { case K: return X; default: break; }` 复现 ORIG
//       `mov -0x14(%ebp),%eax; cmp $K,%eax; jne` 两段寄存器比较（if 形 4.4 恒折
//       `cmpl $K,-0x14(%ebp)`）。82=82 逐条 AE。
// 本轮实质改善（3）：
//   getBreakAwayMemberData 0x084101ae ndiff 272→224、gap 30→-2：P312——分支内
//       `char playInfo[31] = {0}`（槽基 -0x47 非 4 对齐）复现 ORIG 0x0841026e 的
//       4.1 内联 memset 慢路（对齐剥离+主 4B 循环+尾 2/1，逐条全等）；旧 for 逐字节
//       形 = N78b 朴素 9 条形。
//   GetGrowthCreatureEvent 0x08448102 ndiff 17→10：`bool execOk;` 提前到 db 之前
//       声明（赋值仍在 db->exec 处）⇒ 4.4 槽位翻正（db -0x1c→-0x18、execOk
//       -0x15→-0x19）；余 5 行 = N1 `(i+0x866)*8+8` 折 `(i+0x867)*8`。
//   GetAccountCharacterLevelUpOnceGift 0x0844834e ndiff 15→10：同法 execOk 提前
//       声明；余 5 行 = 同 N1 折叠（probe5 六形均 `mov sig,%eax` 预载，ORIG
//       `add 0xc(%ebp),%eax` 内存操作数不可复现）。
// N 封顶复核（12 件，按 P321 T1–T5/S1–S3 复核后维持）：
//   CreateAccount NEAR(4)：N18a/P75——T1 直证字段 uchar（movzbl 0xa0(%eax)），
//       残差仅实参装载序/寄存器角色 ⇒ T5 禁改类型，封 N。
//   GetUserFatigueCount NEAR(8)：N7 间接 call 目标 4.4.7 恒 %eax（4.4.6 后端可翻，
//       本 TU 不在 446 路由表，属 Main 裁决）。
//   getCharacLinkSystemData DIFF(1)：N121 epilogue-threading 尾 `jmp+nop` 岛；
//       probe2/probe3 十四形（while+break/空 if/空 else/goto 尾标签/双 return）实测
//       4.4 全消 jmp ⇒ 封 N。
//   _DecryptUserInfo DIFF(12)：N7/P75——两处 `strlen(hex)>>1` 实参中转互补
//       （ORIG 第一处多 `mov %eax,%edx`、第二处少）⇒ 寄存器角色，T5 封 N。
//   GetBingoEvent DIFF(3)：N107——`lea 0x413c(%eax),%edx` vs 4.4 `add $0x413c,%eax;
//       mov %eax,%edx`（取址 add-vs-lea 六形既定）⇒ 封 N。
//   GetPunishUserInfo DIFF(5)：T2 直证字段 signed（ARAD_LoginSecurity 0x0818f970
//       消费点 `jg`）⇒ 不得改 unsigned；probe6/probe7 证 RHS 侧 `(unsigned int)`
//       被折叠、ushort/uchar cast 多 movzx ⇒ T5 封 N。
//   _getRewardInfo DIFF(382)/GetMercenarySystemInfo DIFF(447)/GetPremiumInfo
//       DIFF(711)/dispatch DIFF(1014)/GetAccountInfo DIFF(1190)：N82 帧差
//       （0x10/0x50）+ N78/N80/N115；p167 全部 CLEAR（18/27/84/56/90）。
// 本轮试过并回退（负例登记）：
//   - col[9] 清零三形：for 循环（现状 ndiff 382）< `= {0}`（403）< 显式 9 条（424）
//     ⇒ 4.4 对 `unsigned int[9]` 的 `={0}`/显式赋值均不产 ORIG 的 9 条展开
//     `movl $0,-0x1c..-0x3c`（P312 仅适用 char 数组）。
//   - P311 数组视图（GrowthCreatureRow 成员取址）：4.4 产 7 条（多 mov sig 预载）
//     vs ORIG 6 条 ⇒ 劣化回退。
// P0：本文件 0 处 `register ... asm("reg")` 绑定。
// .h 改动：本轮 0（未触碰任何 .h）。
// 探针工件：/tmp/solo_probe/probe1..probe7.cpp（4.4.7 -m32 -O0）。
// 无子子代理（R4 合规）。
// ----------------------------------------------------------------------------
// [SweepDBLogin 单文件泳道·本轮] 12 件 DIFF 清单按 dual_rank gap' 降序逐件处理
// （dispatch 由前泳道修过，未重开）。TU 四数 基线 identical=47 ae=134 near=2 diff=12
// → 现在 identical=47 **ae=135** near=2 **diff=11**（+1 AE，零回退）；COMPILE OK 0 错误；
// p167 调用点差集全 CLEAR（仅 cMyTrace/vector/CSimpleSSO 的 C1↔C2、D1↔D2 同址别名豁免）。
//  ① _getRewardInfo gap 44→**0**（DIFF→AE，ndiff 0，330=330）：见函数头四类修法。
//  ② getCharacLinkSystemData gap 2→**0**（ndiff 4 = setcc 极性镜像 P196 折痕）。
//  ③ GetAccountInfo gap −22→**−17**（`long long curSec` 64 位物化改内联 32 位实参）。
//  ④ GetMercenarySystemInfo gap −9→**−14** 但 xor/jne 形匹配 6→1（bool ok 链改提前
//     continue；三形并列读数见函数头，取形匹配者）。
//  ⑤ GetPunishUserInfo gap 1 **已修**（本波）：SigTypes_D.h +0x3a50 匿名 union 双视角，
//     写入走 unsigned 视角复现 ORIG RHS-first+中转（08410da9）；int 视角消费点
//     （ARAD_LoginSecurity 0x0818f970 `jg` / InterDispatcher）A/B 实测零变化。
//  ⑥ 其余 6 件（GetBingoEvent / GetAccountCharacterLevelUpOnceGift / GetGrowthCreatureEvent /
//     _DecryptUserInfo / getBreakAwayMemberData / GetPremiumInfo）逐件复核残差均落
//     已登记 N 族（N1 常量折叠 / N7+P75 装载序 / N82 帧槽 / N107 取址 add-vs-lea /
//     N-P209L 常量比较折叠），无新增真缺口。
// 结论：本 TU 剩余真缺口 = 0（仅 N 族形差）。
// 无子子代理（R4 合规）；无 .h 改动（R2 无申报项）。
// ----------------------------------------------------------------------------
// [SweepDBLogin-2 单文件泳道·本轮] 12 件 NEAR/DIFF 按 gap' 降序逐件复核
// （dual_rank --tu DB_Login.cpp 候选=0：全件 gap≤0 且无 ORIG-only 游戏域 callee）。
// TU 四数 基线 identical=47 ae=136 near=2 diff=10 → 现在 **不变（零回退）**；
// COMPILE OK 0 错误；52 个 DB_Login 符号 + 10 个非 DB_Login 符号（DBDispatcher::
// dispatch / get_compressed_blob_data / comp_by_time / C1D1 等）逐 `call *` 虚槽 +
// jcc 目标相对下标逐位交叉验证：虚槽 5/5 站点两侧一致（0x34×4 + 0x20×1，仅
// GetUserFatigueCount 目标寄存器 %edx↔%eax = N7）；jcc mismatch 全为位移/镜像等价
// （setcc 折痕、setl↔setle 极性镜像、add-vs-lea），无真极性反写。
// 本轮真 bug 修复（2 处，同一实参链）：
//  ① dispatch → GetPremiumInfo 第 1 实参：旧 `GetPremiumInfo(0,...)` 应传 slot。
//     ORIG 0x0841259c `mov 0x10(%ebp),%eax; mov %eax,0x4(%esp); call GetPremiumInfo`
//     （dispatch 形参槽：this 0x8 / userDbIdx 0xc / **slot 0x10** / stream 0x14）。
//  ② GetPremiumInfo → addUserDBInfo 第 1 实参：旧 `(int)sig` 应为 param。
//     ORIG 0x08412f36 `mov 0xc(%ebp),%edx; mov %edx,0x4(%esp)`；0xc(%ebp) 是
//     GetPremiumInfo 第 1 形参 param（sig 实为 0x10(%ebp)），旧注释误认 0xc=sig。
//     两处修后 ORIG/我方该二调用点逐条同形（dispatch O[562]↔N[561]、
//     GetPremiumInfo O[225]↔N[231]）。运行时语义：addUserDBInfo(userDbIdx=slot)。
// 本轮类型判据修复（1 处）：DB_Login::SendResult int→bool（P292/T3：ORIG 9 个调用点
//   `call; mov %eax,%ebx` 无 setne；旧判词「4.4 返回值规范化 N」作废）⇒ dispatch
//   799→791 insn，gap −5→−3；mangled 不含返回类型，ABI 零变化（Main 已批准广播）。
// 本轮签名书写名修复（3 处，P174 直证）：逐字节读 ORIG 16 个 `__PRETTY_FUNCTION__`
//   串（objdump -s），发现 3 处类型书写名不符（4.4 生成的串在 ORIG 中不存在）：
//   ① _getGameDBCharacView 返回 `int` → `RV_GetGameDBCharacInfo`（typedef int）；
//   ② _DecryptUserInfo 第 3 参 `unsigned int` → `memberIdentificationNumber_t`；
//   ③ SaveLoginTime 第 1 参 同 ②。改后 3 串与 ORIG 逐字节同（.o .rodata 复验
//   PRESENT），mangled/ABI/帧/四数零变化（47/136/2/10 不变）。全 TU 字符串审计：
//   .o 全部 >=8 字节 ASCII 串仅剩 2 个 ORIG 无（'DB_Login.cpp' 文件名符号 +
//   GCC 4.4.7 版本串，均工具链元数据）。
// 本轮符号判据修复（1 处）：判据㉖ 匿名 ns 16 个 `const unsigned int OFF_*` → 匿名
//   enum ⇒ 消 16 个 NEW-only `r _ZN12_GLOBAL__N_1L..OFF_..E` 数据符号（ORIG 全无）。
// 逐件读数（gap=orig−ours，.o 口径；改前→改后）：
//   dispatch DIFF −5→−3（calls 56/56；SEG 差全为帧/槽 + ORIG 寄存器 clamp vs 我方槽
//     clamp）；GetPremiumInfo DIFF −11（calls 84/84；end() 未缓存 P13 + bool 物化）；
//   GetAccountInfo DIFF −15（calls 90/90；N80 sig→%ebx 跨 trace + 帧）；getBreakAway
//     MemberData DIFF −2（calls 44/44；setcc-fold 岛等价 + N78 对齐 memset）；
//   GetMercenarySystemInfo DIFF −13（calls 27/27；setl/setle 极性镜像 + 槽）；
//   GetBingoEvent DIFF −1（N107 lea-vs-add）；_DecryptUserInfo DIFF 0（N7/P75）；
//   GetGrowthCreatureEvent / GetAccountCharacterLevelUpOnceGift DIFF 0（N1 常量折叠）；
//   getCharacLinkSystemData DIFF 0（sete/je ↔ setne/jne 双重取反等价）；
//   CreateAccount NEAR 0（N18a）、GetUserFatigueCount NEAR 0（N7 间接 call 目标寄存器）。
// 结论：本 TU 剩余真缺口 = 0（余下全部为已登记 N 族形差 / 帧槽差）。
// 跨 TU 上报（非本泳道）：WindowsPackFiles.cpp 的 CCompressedBlob::{compress,
//   uncompress,reset,resize} 我方为 T、ORIG 全 W（判据㉓ 绑定差，属该 TU 泳道）。
// 无子子代理（R4 合规）；.h 改动 1 处（DB_Login.h:116 SendResult→bool，已 R2 广播
// + Main 批准；消费 TU 仅本 .cpp，全仓 grep 无其它调用点）。
// ----------------------------------------------------------------------------
// df_game_r 还原 —— DB_Login（DBThread 登录分发器，Wave5 仅ORIG清零批）
// 权威报告：docs/class_func_reports/DB_Login/*.md（汇编 + Ghidra 反编译 C），
// 全部 52 个 ORIG 符号逐符号转写；SQL 字符串原样保留（大小写亦原样）。
//
// 字段化状态（本轮）：SIG_LOGIN_DATA 权威布局已在共享头 SigTypes_D.h 具名
// （Inter_LoginInfo/dispatch_sig 批），本 TU 已把偏移一致且语义同名的访问
// 全部切换为具名字段（errCode/accId/时间/奖励/punish 数组/角色槽/徽章/
// 疲劳/安全区/geo 等）；仅保留共享头为 pad 或命名与 ORIG 写入语义错位的
// 少数区域（0xa1 login_ip、0x39b0/0x39dc/0x3a3a、0x2184/0x2204、0x394c、
// premium 行容量超共享头声明、0x3fac 历史表、0x4140/0x4154/0x4338 事件块等）
// 以 SGF<T>(sig, off) 裸偏移访问并逐处注释，宿主后续对齐共享头后落点。
// ============================================================================

// ============================================================================
// [Main·2026-09-10 round5 TU 拆分·部件包含式] 本文件为**聚合器**：
//   ① 上方判词与下方前言（include/域类镜像/宏/常量/视图）逐字保留；
//   ② 函数实现已全部拆到 Game/DB_Login/*.inc（每函数一个部件，部件内无 #include）；
//   ③ 包含列表顺序 = 原 TU 定义序（= ORIG 地址序，见交付报告「顺序表」）⇒
//      同一 TU、同定义序 ⇒ codegen 由构造不变（rule4 实测逐函数 verdict 全等）。
//   TU 名/路径不变 ⇒ dnf_game_cxx.sh 三处匹配（本 TU = 仅第 ② 处 446 注入）自动继承，
//   CMakeLists 无需改动。
// ============================================================================
#include "DB_Login.h"

#include <string.h>
#include <cstring>   // [b28d-38] std::strrchr（ORIG 调 _ZSt7strrchrPci）
#include <stdlib.h>
#include <time.h>

#include "GlobalData.h"        // s_db_mgr/s_systemTime_/s_event_manager/s_psimpleSSO/
                               // s_msgq_mgr/s_big_stream_pool/s_stream_pool
#include "DBMgr.h"
#include "MySQL.h"
#include "SigTypes.h"          // SIG_LOGIN_DATA / SIG_UPDATE_LOGIN
#include "STBreakAwayMemberData.h"  // STBreakAwayMemberData::clear（ORIG W 0x844d584）
#include "Stream.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "GameTypes.h"         // cMyTrace
#include "CSimpleSSO.h"        // WongWork::CSimpleSSO / stOutputData_t
#include "CRijndael.h"
#include "SyncScriptDeps.h"    // toTString（轻量声明头）
// ---- G_* 访问器/域类最小镜像（符号由权威 TU 提供；共享头稳定后可切回权威头）----
CEnvironment* G_CEnvironment();     // StreamDeps.h 已声明（权威）
class GameWorld
{
public:
    bool IsIntegratedPvPBaseChannel() const;   // ORIG T
    int GetChannelType() const;                // ORIG T
    bool IsSchoolPvPChannel() const;           // ORIG T
};
GameWorld* G_GameWorld();

class CDataManager
{
public:
    void* get_event_script_mng();
    int GetPvPMode();    // ORIG W 0x844d4ae（LinkClosureA_Data.cpp 实现）
};
CDataManager* G_CDataManager();

class CUser;
class CGameManager
{
public:
    CUser* getUser(int slotId, int increId);   // ORIG T
};
CGameManager* G_CGameManager();

// §4.4/§4.3：CEventBase/CEventManager 统一取权威头（原本 TU 局部镜像
// `class CEventBase;` + 仅含 GetRepeatEvent 的最小 CEventManager 已删）。
// 权威 CEventBase 声明序已按 ORIG vtable 0x08b4bae0 排好（IsEventing =
// slot 13 = vtable+0x34，CGuildServerProxy.h:129），故两处 is-eventing 判定
// 改真实虚成员调用（范式 P5）。实测：仅 include 权威头对本 TU .o 逐函数
// 全等（零扰动），无需 asm 槽直调。
#include "CGuildServerProxy.h"

// [SweepDBLogin-2] P174/P110：ORIG `__PRETTY_FUNCTION__` 串逐字节实证（objdump -s
// 读 0x08c5ce20 / 0x08c5cf60 / 0x08c5ce80）本 TU 三处签名书写名：
//   `DB_Login::RV_GetGameDBCharacInfo DB_Login::_getGameDBCharacView(SIG_LOGIN_DATA*)`
//   `bool DB_Login::_DecryptUserInfo(char*, char*, memberIdentificationNumber_t)`
//   `void DB_Login::SaveLoginTime(memberIdentificationNumber_t, int, char*)`
// 旧源码写 int / unsigned int ⇒ 4.4 生成 `int DB_Login::_getGameDBCharacView(...)` /
// `bool DB_Login::_DecryptUserInfo(char*, char*, unsigned int)` /
// `void DB_Login::SaveLoginTime(unsigned int, int, char*)`（.o .rodata 实证不在 ORIG）。
// typedef 只影响 __PRETTY_FUNCTION__ 书写名；mangled/ABI/sizeof 零变化。
// 先例：CGuildServerProxy.cpp:139 / InterSelectPcroomDailyReward.h:18（重复同型合法）。
typedef unsigned int memberIdentificationNumber_t;
typedef int RV_GetGameDBCharacInfo;

namespace EventClassify
{

class CEventEntity {};                          // 仅作 get_event_entity 返回类型占位
class CEventScriptMng
{
public:
    bool is_eventing(int key);                  // ORIG T 0x0810ba36
    CEventEntity* get_event_entity(int key);    // ORIG T 0x0810b96a
    unsigned char get_today_time_day();         // ORIG T 0x0810c6ca
};
}  // namespace EventClassify
namespace online_preliminary
{
class COnlinePreliminary
{
public:
    static unsigned int GetPlayCountPerOneTeam();  // T 0x0855fee8（ORIG 无 this，静态实证）
};
}  // namespace online_preliminary

namespace WongWork
{
class CUserPremium
{
public:
    static bool IsRestrictedPremium(int type);    // T 0x086af7c4（ORIG 无 this，静态实证）
};
}  // namespace WongWork

// CHARAC_LOAD_MERCENARY 权威定义已迁至 SigTypes.h（Inter_LoginInfo/dispatch_sig 批）。

#include "DNFFunctionLib.h"    // DNFFLib::Hex2Binary
namespace OS_API               // CInventory.h 同款最小声明（snprintf wrapper）
{
int snprintf(char* buf, unsigned int n, const char* fmt, ...);
}
char* NumberToString(unsigned int value, int radix);   // C++ linkage（DNFFunctionLib.h 同款）
class CUser;
bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime,
                            long lCurTime);                          // T 0x0846c0a8（GameUtils/独立批次）
long getPrevScheduleTime(int hour, long time);                       // T 0x0846c2e9（GameUtils.cpp）
int getDailyInitializeTime(int hour, int min, int sec);              // T 0x086b1f5a（GameUtils.cpp）
long GetCurrentResetBaseTime(long t, int hour);                      // T 0x0810912a（_Z23GetCurrentResetBaseTimeli）

bool uncompress_zip(char* dst, unsigned int& dstLen,
                    const char* src, unsigned int srcLen);           // 归属 APSystemOnlyOrig 批（Main 协调）

namespace Taiwan {
bool GarenaCheckTimeStamp(unsigned int tokenTime, unsigned int nowSec,
                          unsigned int loginTime, unsigned int lastPlayTime); // T 0x08173080（Taiwan 域批）
}

// DB_MouseRegister::convertPassPadInfo（static，ORIG T 0x08422e6a；
// c++filt 形参 (unsigned int, char*, int&, char&, int&, bool&, char*, bool&,
// char*, long&, bool&)；mangled _ZN16DB_MouseRegister18convertPassPadInfoEjPcRiRcS1_RbS0_S3_S0_RlS3_。
// 报告尾部 eax 残留 0x48/0x49（失败计数字路径）被上层 dispatch 直接消费
// （==0x48→errcode5、==0xa6→9），故以 int 错误码承载（mangled 不含返回类型）。
// 定义在 MouseRegisterEJ.cpp。）
class DB_MouseRegister
{
public:
    static int convertPassPadInfo(unsigned int mid, char* mousepass,
                                  int& failCnt, char& cancelCnt,
                                  int& cancelCnt2, bool& validityExpired,
                                  char* ip, bool& flag2, char* buf3,
                                  long& validityTime, bool& flag3);
};

namespace WongWork {
// 五个活动物品编排器（static，定义归 WongWork EventATI/Sigong/Muz/Alttle/FCCombo 域批）
struct EventATI    { static void ArrangeATIEventItem(unsigned int mid, unsigned int param); };      // T 0x083fa1d3
struct EventSigong { static void ArrangeSigongEventItem(unsigned int mid, unsigned int param); };   // T 0x083fa855
struct EventMuz    { static void ArrangeMuzEventItem(unsigned int mid, unsigned int param); };      // T 0x083fb16a
struct EventAlttle { static void ArrangeAlttleEventItem(unsigned int mid, unsigned int param); };  // T 0x083fb595
struct EventFCCombo{ static void ArrangeFCComboEventItem(unsigned int mid, unsigned int param); }; // T 0x083fb976
} // namespace WongWork

// get_compressed_blob_data 支撑 blob 缓冲（ORIG W CCompressedBlob 0x844de52 起；
// 全源码此前无 owner，唯一权威定义落本 TU，布局按 ctor/resize/reset 反汇编）。
class CCompressedBlob
{
public:
    CCompressedBlob() : m_data(0), m_cap(0) {}       // 0x844de52
    ~CCompressedBlob()                               // 0x844de6a
    {
        if (m_data != 0)
        {
            delete[] m_data;
            m_data = 0;
        }
    }
    void resize(int cap)                             // 0x844e02a：仅扩容不缩
    {
        if (cap <= m_cap)
        {
            return;
        }
        delete[] m_data;
        // ORIG 0x0844e05f `movl $0x0,(%eax)`：delete[] 后显式清指针再 new。
        // （ORIG 4.1 delete[] 展开双 null 检 `test;je` ×2，4.4 恒单检 ⇒ 1 行 N 差。）
        m_data = 0;
        m_data = new char[cap];
        m_cap = cap;
    }
    void reset()                                     // 0x844e080：memset(m_data,0,m_cap)
    {
        memset(m_data, 0, m_cap);
    }
    // 权威实现见 WindowsPackFiles.cpp:1196（同 8B 布局 m_buf@0/m_size@4 ↔ m_data/m_cap）。
    bool uncompress(MySQL* db, int fieldIdx, char* dst, int dstLen, int align);

    char* m_data;                                    // +0x00
    int m_cap;                                       // +0x04
};
// ============================================================================
// SIG_LOGIN_DATA 字段访问 helper（本 TU 局部；共享头字段化后可整体替换）
// ============================================================================
// O0 下 inline 不生效；ORIG 为直接 lea 字段访问，必须 always_inline 强制展开。
template <typename T>
inline __attribute__((always_inline)) T& SGF(void* sig, unsigned int off)
{
    return *reinterpret_cast<T*>(reinterpret_cast<char*>(sig) + off);
}
inline __attribute__((always_inline)) char* SGB(SIG_LOGIN_DATA* sig, unsigned int off)
{
    return reinterpret_cast<char*>(sig) + off;
}
// 常量偏移直写：宏把 off 作为字面量替换，-O0 下折叠进基址/地址表达式
// （SGF<T>(sig,off) 模板的 off 是运行期参数，会多做一次 add）。
#define SGFC(type, sig, off) (*reinterpret_cast<type*>(reinterpret_cast<char*>(sig) + (off)))
#define SGBC(sig, off) (reinterpret_cast<char*>(sig) + (off))

namespace   // 残留裸偏移速查（已字段化区域不再需要；均出自对应报告 ORIG 指令）
{
// 共享头 SigTypes_D.h 已具名覆盖的访问一律用具名字段；仅下列 SigTypes_D 为
// pad/隐式区或命名错位的区域保留常量访问（每处带语义注释）。
// [SweepDBLogin-2] 判据㉖：匿名 ns 的 `const unsigned int` 在 4.4 -O0 恒发射
// `r _ZN12_GLOBAL__N_1L...OFF_...E` 数据符号（ORIG 18 个同名符号全无，原码为
// 编译期常量）⇒ 改匿名 enum（零数据符号、使用点仍折立即数）。
enum
{
    OFF_CLIENT_IP       = 0x0018,   // checkValidPassword clientHash 输入缓冲
    OFF_LOGIN_IP        = 0x00a1,   // 登录 ip 字符串（SigTypes_D 名 m_blackIpCheckBuf）
    OFF_BIRTH_DATE      = 0x00d0,   // reg_date DATE_FORMAT uint（SigTypes_D pad）
    OFF_CHARAC_VIEW     = 0x00e0,   // 0x24×0x94 角色视图数组首（m_characData）
    OFF_SECURITY_VALID  = 0x39dc,   // security card 有效标志（SigTypes_D pad）
    OFF_SSO_LOGINIP     = 0x3a3a,   // login_ip[0x10]（共享头名 m_webAddress）
    OFF_SEC_FAILCNT     = 0x39e4,   // 安全卡 fail_cnt（共享头名 m_secCardCertCnt）
    OFF_SEC_CERTFLAG    = 0x39e8,   // cert_flag uint（SigTypes_D pad）
    OFF_SEC_CERTCNT     = 0x39ec,   // cert_cnt byte（共享头名 m_secCardCertFlag）
    OFF_SEC_QPOS        = 0x3a14,   // q_pos int（共享头名 m_secCardCancelCnt）
    OFF_SEC_QPOS_STR    = 0x3a18,   // q_pos 串槽 char[5]（共享头名 m_secCardAnswers）
    OFF_HISTORY_TBL     = 0x3fac,   // 0x168 = 30×{int type,h1,h2}（m_ingameEventHistory）
    OFF_GROWTH_WEAPON   = 0x4140,   // 事件块 A（0x14，共享头 raw）
    OFF_GROWTH_CREATURE = 0x4154,   // 0x2c×11（m_growthCreatureEvent 具名条目可用）
    OFF_ONCE_GIFT       = 0x4338,   // 0x20（共享头 raw）
    OFF_BINGO_EVT       = 0x4138,   // SIG_BINGO_EVENT 起点
};
// [b28d-solo-DB_Login] P311 角色槽视图：0x24×0x94 步长数组（+0xe0 起）。具名
// 完整类型成员直访问复现 ORIG `imul $0x94; lea (%edx,%eax,1); add $0xe0` 三步
// （探针 probe_arr f4 逐条全等）；char 数组 + 指针算术恒把 add $0xe0 提前（f1）。
// 仅表达布局，不实例化、零符号、零 sizeof/偏移影响。
struct CharacSlotView { int m_characNo; char m_rest[0x90]; };
struct CharacDataView { char m_pad[0x00e0]; CharacSlotView m_slots[0x24]; };
}
// ============================================================================
// ---- 部件包含列表（49 个；顺序 = 原 TU 定义序）----
// ============================================================================
#include "DB_Login/DB_Login_ctor.inc"
#include "DB_Login/DB_Login_dtor.inc"
#include "DB_Login/DBDispatcher_dispatch.inc"
#include "DB_Login/_getGameDB.inc"
#include "DB_Login/_GetDNFAccountDB.inc"
#include "DB_Login/dispatch.inc"
#include "DB_Login/getCharacLinkSystemData.inc"
#include "DB_Login/GetLoginCommonData.inc"
#include "DB_Login/GetReturnUserInfo.inc"
#include "DB_Login/GetDailyCleanPadPoint.inc"
#include "DB_Login/GetPunishUserInfo.inc"
#include "DB_Login/_isJoinedSchoolPointEvent.inc"
#include "DB_Login/_isAutoPunishedFirstUser.inc"
#include "DB_Login/_getAutoPunishedSecondUserData.inc"
#include "DB_Login/_IsPunishedUser.inc"
#include "DB_Login/_isPunishedHackUser.inc"
#include "DB_Login/_getSecurityCardInfo.inc"
#include "DB_Login/_getPassPadInfo.inc"
#include "DB_Login/_convertPassPadInfo.inc"
#include "DB_Login/_getRewardInfo.inc"
#include "DB_Login/_LoadDNFEventEntry.inc"
#include "DB_Login/_LoadEventItem.inc"
#include "DB_Login/GetPremiumInfo.inc"
#include "DB_Login/RemovePreminuInfo.inc"
#include "DB_Login/IsPCRoomIP.inc"
#include "DB_Login/CreateAccount.inc"
#include "DB_Login/_DecryptUserInfo.inc"
#include "DB_Login/GetAccountInfo.inc"
#include "DB_Login/GetWhiteAccount.inc"
#include "DB_Login/GetSchoolName.inc"
#include "DB_Login/GetBadgeCount.inc"
#include "DB_Login/GetGameDBInfo.inc"
#include "DB_Login/SaveLoginTime.inc"
#include "DB_Login/get_compressed_blob_data.inc"
#include "DB_Login/_getGameDBCharacView.inc"
#include "DB_Login/SendResult.inc"
#include "DB_Login/UpdateLoginStat.inc"
#include "DB_Login/GetCheckPickUpRandomOptionItem.inc"
#include "DB_Login/GetOntimeUserRecvIdx.inc"
#include "DB_Login/GetUserFatigueCount.inc"
#include "DB_Login/GetUserInGameEventHistory.inc"
#include "DB_Login/CheckLoginIP.inc"
#include "DB_Login/GetMercenarySystemInfo.inc"
#include "DB_Login/GetGrowthWeaponEvent.inc"
#include "DB_Login/GetGrowthCreatureEvent.inc"
#include "DB_Login/GetAccountCharacterLevelUpOnceGift.inc"
#include "DB_Login/GetBingoEvent.inc"
#include "DB_Login/IsThisIPLoginPermissionGranted.inc"
#include "DB_Login/getBreakAwayMemberData.inc"
