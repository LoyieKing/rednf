// ---------------------------------------------------------------------------
// Arad_GiftItem_Set::send_mail(CUser*, int, int, int, int)
// ORIG T _ZN17Arad_GiftItem_Set9send_mailEP5CUseriiii @0x081a329e sz0x177。
// 报告 docs/class_func_reports/Arad_GiftItem_Set/send_mail.md（汇编+Ghidra C）。
// find(第5,4,3参) → 未命中 logFormat+0 → AradMailInfo 组装：
//   +0x0c=第3参、+0x00=GetServerGroup(字节)、+0x1c=Data.m_str4.c_str、
//   +0x18=Data.m_str8.c_str、+0x14=Data.m_str8.length、
//   m_item{ +0x0c=Data.m_item, +0x04=1, +0x08=2, +0x0a=Inven_Item+0xb(endurance) }
// → find_item 失败 logFormat+0；否则 item.make_item(inven)（CItem vtable slot
//   +0x8）→ 读 endurance → ARAD::send_mail(info)。
// ---------------------------------------------------------------------------
// [b28d-solo-LinkClosureE] DIFF 95/94（p167 CLEAR 12/12）：唯一残差 = make_item 的
//   间接 call 目标寄存器（ORIG `mov (%eax),%edx … call *%edx` vs 4.4.7 恒 %eax）
//   ⇒ N7 封顶（P230：需 4.4.6 后端，路由由 Main 统一管）。真 bug 已修：
//   info.m_fieldC 实参 p3→p2（ORIG 读 0x10(%ebp)）、find_item/日志源
//   data->m_item → info.m_item.m_fieldC（ORIG 从栈槽重读）；m_item 字段写序
//   m_fieldC/m_field8/m_field4/m_fieldA/m_itemAttr 与 Inven_Item 声明位置照 ORIG。
// [D08 2026-09-10] 复核：残差恰 2 处、gap −1 —— ①0x081a33e5 间接 call 目标寄存器
//   ORIG %edx / 我方 %eax（N7，P230：4.4.6 后端可修，路由归 Main）；②0x081a3387
//   `mov -0x1c(%ebp),%eax; mov %eax,%ebx`（find_item 实参装载两段）vs 我方
//   `mov -0x1c(%ebp),%ebx`（4.4 直落 callee-saved）⇒ N18a。两处均封顶。
//   [D08 2026-09-10 补] C07 形（字段读改具名 int 局部）在本件**不适用**：写成
//   `int itemIndex = info.m_item.m_fieldC; find_item(itemIndex);` 后 4.4 折叠为
//   93 条（原 94，ORIG 95）⇒ gap −1→−2 反劣，已回滚。C07 的「升格保活」只在
//   ORIG 侧具名局部值为 4.4 未折叠的字段 load 时成立，本件字段在栈上且 4.4 直落
//   callee-saved ⇒ 不具名仍最优。
//   [Main 边界并入] 同因 A14 `CPrivateStore::BuyItem` 实测 +2 劣化 ⇒ C07 完整前提 =
//   字段读被升格 callee-saved **且该值在 call 之后仍被本函数复用**；本件该值仅作
//   find_item 的**被调方按值实参**（call 后不复用）⇒ 属禁套面。
// [N1D 2026-09-11 · n1_v 口径 + §6.5 串审计] 本波真修：**返回类型 int → bool**。
//   证据三合一：① ORIG `.rodata` `_ZZN17Arad_GiftItem_Set9send_mailEP5CUseriiiiE
//   19__PRETTY_FUNCTION__` @0x08bad6e0 size **0x3e** 实字节 =
//   `bool Arad_GiftItem_Set::send_mail(CUser*, int, int, int, int)`（我方原 0x3d
//   = `int ...`，N8 strall.py 审计报 O-only/N-only）；② ORIG 尾段 0x081a3407
//   `call ARAD::send_mail` 之后直落 `add/pop/ret`，**无 bool 截断**（`test/setne`
//   零条）⇒ 与 `ARAD::send_mail` 同型；③ 头文件声明侧（`CItemList.h:338` /
//   `Arad_ItemBonus.h:82`，父泳道批准）+ 本 .inc 首行 + N4 的 ARAD 定义侧同批落 bool。
//   落地后：我方 prett 符号 size **0x3e** 与 ORIG 逐字节一致；读数 95/95 gap 0
//   保持（4.4 未插入截断）⇒ 串差异消除、零 ABI 影响（返回类型不入 mangled）。
//   余 2 处 codegen 残差（本波复测仍存，判后端封顶）：
//   ① 0x081a3384 `mov -0x1c(%ebp),%eax; mov %eax,%ebx`（find_item 实参两段装载）
//      vs 我方 `mov -0x1c(%ebp),%ebx` 单段 ⇒ N18a（C07 具名形反劣已证，见上）；
//   ② 0x081a33e5 `mov (%eax),%edx … call *%edx` vs 我方 `call *%eax` ⇒ N7
//      （间接 call 目标寄存器，4.4.7 恒 %eax；4.4.6 后端可翻，路由归 Main）。
bool Arad_GiftItem_Set::send_mail(CUser* user, int p2, int p3, int p4, int giftIdx)
{
    Data* data = find(giftIdx, p4, p3);
    if (data == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_Script_Loader.cpp",
                              __PRETTY_FUNCTION__,
                              0x15d, "[ERROR] send mail - Data None ");
        return 0;
    }

    ARAD_DATAS::AradMailInfo info;
    info.m_fieldC = p2;
    info.m_isSystemMail = (unsigned char)user->GetServerGroup();
    info.m_name = const_cast<char*>(data->m_str4.c_str());
    info.m_content = const_cast<char*>(data->m_str8.c_str());
    info.m_field14 = (int)data->m_str8.length();

    info.m_item.m_fieldC = data->m_item;
    info.m_item.m_field8 = 2;
    info.m_item.m_field4 = 1;
    info.m_item.m_fieldA = 0;
    info.m_item.m_itemAttr = 0;

    Inven_Item inven;

    CItem* item = G_CDataManager()->find_item(info.m_item.m_fieldC);
    if (item == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_Script_Loader.cpp",
                              __PRETTY_FUNCTION__,
                              0x175, "[ARAD_RYOSIKA] Item Null. (index:%d)",
                              info.m_item.m_fieldC);
        return 0;
    }

    item->make_item(inven);
    info.m_item.m_fieldA = inven.m_endurance;
    return ARAD::send_mail(info);
}