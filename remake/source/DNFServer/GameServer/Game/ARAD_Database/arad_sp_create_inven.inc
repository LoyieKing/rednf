// ORIG 0x0818b2b7：inSert into inventory ……
//   m_giftFindOk(+0x533f)==0 → 无背包快照，仅插空库存（coin 由送币事件定）；
//   否则把 +0x3a9 起 0x3b55 字节 zip 压缩为 blob 连同原串一并入库。
// b28d-solo-ARAD_Database：ORIG 0x818b2b7 全头部声明（P212）——memset(zipBuf)/
//   zipLen/invenSrc 三条在 giftFindOk 判断之前；giftFindOk==0 分支的 insert 返回值
//   按 ORIG 不检查（P171 照抄不写）；主流程 insert 四可变实参右→左求值（coin 先入
//   %esi、两个 blob_to_str 结果 %ebx/%eax 寄存器直传，无具名局部槽）。
// b28d-solo-ARAD_Database verdict=DIFF(85/84)：P167 8/8 CLEAR；残余=memset 首参
//   edx 中转（待解条目，字面量/&arr[0]/sizeof 均试过无效）+ compress_zip 实参
//   装载序（同 create_equip）。语义闭合。
// [round5 复验] P68 形 `memset((char*)zipBuf, 0, sizeof(zipBuf))`（dest 先入 %eax）
//   实测 TU 四数零变化（63/130/0/6），dest 装载序未翻 ⇒ 维持 N 判词 | ORIG 0x0818b299
//   `lea -0x4162(%ebp),%eax; mov $0x4152,%edx; mov %edx,0x8(%esp); movl $0x0,0x4(%esp);
//   mov %eax,(%esp); call memset@plt`。
// [S5-2026-09-11 复核] 维持 N：ORIG 的 `mov $imm,%edx; mov %edx,0x8(%esp)` 常量经寄存器
//   中转是 4.1 风格（4.4 恒直出 `movl $imm,0x8(%esp)`），源码侧无旋钮（字面量/&arr[0]/
//   sizeof 三形已试）；gap +1 = ORIG 独有段，判 N 不硬啃。SQL 串逐字节命中 ORIG。
int CREATE_QUERY::arad_sp_create_inven(MySQL* db, SIG_CREATE_CHARAC* sig)
{
    // [符号名轴 2026-09-11 W13D 探针实测] 原 `static const unsigned int kInvenSrcLen =
    //   sourceInvenSize;` 会在本 .o 发射 ORIG 侧不存在的
    //   `_ZZN4ARAD8DATABASE12CREATE_QUERY20arad_sp_create_inven…E12kInvenSrcLen`（4B r）
    //   ⇒ 发明符号。改用命名空间常量 sourceInvenSize（ORIG 侧有同名
    //   `_ZN4ARAD8DATABASEL15sourceInvenSizeE`；编译期常量 ⇒ 仍出立即数）。
    //   overlay 实测：OUR-only 2→0、四数 63/130/0/6 零变化。
    char zipBuf[0x4152];
    memset(zipBuf, 0, sizeof(zipBuf));
    unsigned int zipLen = 0x4152;
    char* invenSrc = reinterpret_cast<char*>(sig) + 0x3a9;

    if (sig->m_giftFindOk != 0)
    {
        // ORIG 判定与 equip 相反：compress 返回非 0 即放弃（逐指令转写，勿"修正"）
        if (compress_zip(zipBuf, zipLen, invenSrc, sourceInvenSize))
        {
            return -1;
        }
        if (db->insert("inSert into inventory (charac_no, inventory,coin) "
                       "values(%u, '%s',%d)",
                       sig->m_characNo,
                       db->blob_to_str(0, zipBuf, static_cast<int>(zipLen)),
                       db->blob_to_str(0, invenSrc, sourceInvenSize),
                       WongWork::CEventMgr::GetCoinCharacCreate()) != 0)
        {
            return -1;
        }
    }
    else
    {
        db->insert("inSert into inventory  (charac_no, coin, money)  values(%u, %d) ",
                   sig->m_characNo);
        if (WongWork::CEventMgr::GetCoinCharacCreate() != 0)
        {
            return -1;
        }
    }
    return 0;
}