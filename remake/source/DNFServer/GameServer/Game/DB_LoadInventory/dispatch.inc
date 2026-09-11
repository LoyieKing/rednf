// ============================================================================
// DB_LoadInventory::dispatch @0x0840a66e
// 基类 dispatch 通过 → GetOutBuffer<SIG_LOAD_INVENTORY> → GetUserInven/
// GetUserAvatar/creature 槽 reset/GetUserCreature/_getCharacInvenExpand/
// GetItemLock/_GetEquipMaxItemLevel，任一步失败 throw int（1/2/3/4/6/7），
// catch 后置 sig->m_result=1 继续 SendResult。错误行号 0x2364..0x23a4。
// b28d-solo-DB_LoadInventory：P227 全表达式临时链（6 处 trace 的 ebx 保活）
// + 尾 SendResult 具名 bool 早返已归位；残差 = 4.4 EH 帧/槽布局（sub 0x94 vs
// 0x84、sig -0x1c vs -0x14、catch 段 __cxa_get_exception_ptr + _Unwind_Resume）
// ⇒ N104/跨版本 EH 封顶；p167 差集 MISSING=0（语义完整）。
// ============================================================================
bool DB_LoadInventory::dispatch(int userIdx, int slot, Stream* stream)
{
    // P212/P309：全头部声明 + 声明序按 ORIG 槽深序复现 4.1「先声明深」标量区。
    // ORIG 槽图：sent(-0x15,bool) / sig(-0x14) / err(-0x10) / i(-0xc) ⇒ sent 声
    // 明最先（最深），故 sent 亦提至函数头（P212「全头部声明」含 bool 汇聚变量）。
    bool sent;
    SIG_LOAD_INVENTORY* sig;
    int err;
    int i;
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
    {
        return false;
    }

    // Stream::GetOutBuffer<SIG_LOAD_INVENTORY>(param_3)（模板实例本 TU 隐式
    // 产出，ORIG 对应 W 实例符号）。
    sig = stream->GetOutBuffer<SIG_LOAD_INVENTORY>();
    if (sig == 0)
    {
        return false;
    }

    try
    {
        if (!GetUserInven(sig))
        {
            // P227 全表达式临时链：实参在 ctor 前求值并入 %ebx 跨 ctor 保活
            // （ORIG `mov 0x4(%eax),%ebx` + `mov %ebx,0x8(%esp)`）。
            cMyTrace(__PRETTY_FUNCTION__, 0x2364, 5)
                ("DB_LoadInventory::dispatch, GetUserInven ERROR charac_no=%u",
                 sig->m_characNo);
            throw 1;
        }
        if (!GetUserAvatar(sig))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2369, 5)
                ("DB_LoadInventory::dispatch, GetUserAvatar ERROR charac_no=%u",
                 sig->m_characNo);
            throw 2;
        }
        // 140 个 creature 槽 + 1 个尾槽 reset（Inven_Item::reset W 0x080cb7d8）。
        for (i = 0; i < 0x8c; ++i)
        {
            sig->m_invenType3[i].reset();
        }
        sig->m_invenType3[0xee].reset();   // 尾槽 0x12787 = 0xeed1+0xee×0x3d

        if (sig->m_creatureCount != 0)
        {
            if (!GetUserCreature(sig))
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x2379, 5)
                    ("DB_LoadInventory::dispatch, GetUserCreature ERROR"
                     " charac_no=%u", sig->m_characNo);
                throw 3;
            }
        }
        // [D04 P225] ORIG 0x0840a8c7/0x0840a963：`cmpl $0x0,err槽; setne %al;
        // test %al,%al; je <继续>`（4.1 对 int 形参/局部槽比零的 bool 物化）——
        // 4.4 对 `if (err != 0)` 恒折 `je` 直跳（少 2 条）。P225 复现形 =
        // `while (err != 0) { body; }`（body 以 throw 结束 ⇒ 无回边/无 break jmp）。
        err = 0;   // P198① 预清：ORIG 0x0840a8ab `movl $0x0,-0x10(%ebp)`（调用前）
        err = _getCharacInvenExpand(sig);
        while (err != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2381, 5)
                ("DB_LoadInventory::dispatch, _getCharacInvenExpand ERROR"
                 " charac_no=%u, line(%d)", sig->m_characNo, err);
            throw 4;
            break;
        }
        err = GetItemLock(sig->m_characNo,
                          &sig->m_locks[0]);
        while (err != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2395, 5)
                ("DB_LoadInventory::dispatch, GetItemLock ERROR charac_no=%u,"
                 " line(%d)", sig->m_characNo, err);
            throw 6;
            break;
        }
        if (!_GetEquipMaxItemLevel(sig))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x23a4, 5)
                ("DB_LoadInventory::dispatch, _GetEquipMaxItemLevel ERROR"
                 " charac_no=%u", sig->m_characNo);
            throw 7;
        }
    }
    // ORIG landing pad @0x0840aa6a 直接 `call __cxa_begin_catch`（无
    // `cmp $0x1,%edx` 类型过滤、无 `_Unwind_Resume`、无异常值读取）
    // ⇒ 原码是 catch(...)；写 catch(int) 会多出类型过滤 + unwind 续传路径。
    catch (...)
    {
        sig->m_result = 1;      // ORIG catch handler @0x84aa6a：+0x4030 置 1
    }
    // ORIG 尾形：SendResult 结果落具名 bool 槽 -0x15 → `xor $1; test; je` 分支，
    // 成功 `mov $1,%eax`、失败 `mov $0,%eax`（直接 return 调用被 4.4 tail 化）。
    sent = SendResult(userIdx, slot, sig);
    if (!sent)
    {
        return false;
    }
    return true;
}