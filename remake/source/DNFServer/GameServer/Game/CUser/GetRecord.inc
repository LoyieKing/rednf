// ORIG 0x08677dd8 _ZNK5CUser9GetRecordEP12SIG_LOAD_ETC
// 遍历 getBattleRecordRefR() 的 map<int,GameResultType>，每元素 0x50 字节序列化到
// sig+0x3c8+idx*0x50（最多 100 个），末尾写 count 到 sig+0x3c4。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08677dd8 | _ZNK5CUser9GetRecordEP12SIG_LOAD_ETC
void CUser::GetRecord(SIG_LOAD_ETC* sig) const
{
    // 2026-09-10（G3CUser-B 复验）：槽集 ORIG/我方 各 4 槽、零平移零多余
    //   （110:110 insn）⇒ 残余纯属跨版本 codegen（含 rep movsl vs memcpy lowering），判 N。
    // b28d-solo-CUser p167：ORIG 0x08677dd8 调 getBattleRecordRefR 3 次
    //   （begin/end/size，无 rec 缓存；我方缓存仅 1 次 ⇒ MISSING ×2），且序列化用
    //   结构整体赋值（rep movsl，报告实证）而非 memcpy call（我方 EXTRA memcpy ×1）
    //   ⇒ 去缓存 + `sig->m_records[index] = it->second;`（P83/P183）。
    // b28d-lane5（probe_slots f1/f2）：ORIG end() **只调一次**（0x08677ecb 循环尾无 getter/end
    //   构造）= 具名缓存；4.4 先声明更浅 ⇒ it(-0x20) 先声明、itEnd(-0x24) 后声明与 ORIG
    //   0x08677dec/0x08677e09 槽址一致。index 裸声明 + for-init 赋值 ⇒ `movl $0,-0x1c` 落到
    //   begin/end 之后（ORIG 0x08677e1b）；getBattleRecordRefR 仍调 3 次（begin/end/size）。
    int index;
    std::map<int, GameResultType>::const_iterator it =
        getBattleRecordRefR()->begin();
    std::map<int, GameResultType>::const_iterator itEnd =
        getBattleRecordRefR()->end();
    for (index = 0; it != itEnd && index <= 99; ++it, ++index)
    {
        sig->m_records[index] = it->second;
    }
    sig->m_recordCount = (int)getBattleRecordRefR()->size();
}