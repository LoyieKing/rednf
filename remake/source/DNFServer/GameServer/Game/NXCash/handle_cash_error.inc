// ORIG 0x081ad1e0：switch(err-5) 跳转表分支（默认 16）。
int NCashImpl::handle_cash_error(int err)
{
    int result = 0x10;
    // ORIG 跳转表（0x8bb42d4，objdump -s 实证）逐项：
    //   err-5=0→0x81ad225 r3 "[%s] NO REMIN CASH"
    //   err-5=1→0x81ad205 r4 "[%s] DB error occur"
    //   err-5=2→0x81ad299 r0x10 "[%s] Undefined cash error: %u"
    //   err-5=3→0x81ad242 r5 "[%s] E_GIFT_LIMIT_OVER Remain cash"
    //   err-5=4→0x81ad25f r6 "[%s] E_GIFT_NOT_READY error"
    //   err-5=5→0x81ad27c r7 "[%s] E_GIFT_UNDER_AGE error"
    // [§6.5 串盲区·运行期真 bug] 旧形 5 个串按块序整体**错位一格**（ndiff 把
    // 立即数归一成 <A> ⇒ verdict 仍 AE，串内容差异不可见；strall.py 串多重集
    // 才暴露）。错位后果：err=6 打 "NO REMIN CASH"（应 DB error occur）、
    // err=5 打 "E_GIFT_LIMIT_OVER"（应 NO REMIN）、err=8 打 GIFT_NOT_READY、
    // err=9 打 GIFT_UNDER_AGE、err=10 打 "Undefined cash error"（应 UNDER_AGE）。
    // 串地址：0x8bb422b / 0x8bb4240 / 0x8bb4254 / 0x8bb4278 / 0x8bb4295 / 0x8bb42b4。
    switch (err - 5)
    {
    case 1:      // 置于 case0 前，使块1=r4（DB error occur）、块2=r3（NO REMIN）匹配 ORIG 块序
        result = 4;
        printf("[%s] DB error occur\n", __FUNCTION__);
        break;
    case 0:
        result = 3;
        printf("[%s] NO REMIN CASH\n", __FUNCTION__);
        break;
    case 3:
        result = 5;
        printf("[%s] E_GIFT_LIMIT_OVER Remain cash\n", __FUNCTION__);
        break;
    case 4:
        result = 6;
        printf("[%s] E_GIFT_NOT_READY error\n", __FUNCTION__);
        break;
    case 5:
        result = 7;
        printf("[%s] E_GIFT_UNDER_AGE error\n", __FUNCTION__);
        break;
    case 2:      // 与 default 同流（跳转表同项）
    default:
        result = 0x10;
        printf("[%s] Undefined cash error: %u\n", __FUNCTION__,
               static_cast<unsigned>(err));
        break;
    }
    return result;
}