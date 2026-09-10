int NCashImpl::handle_cash_error(int err)
{
    int result = 0x10;
    // ORIG 跳转表（0x8bb42d4）逐项：case0=err5→r3"E_GIFT_LIMIT_OVER Remain cash"、
    // case1=err6→r4"NO REMIN CASH"、case2=err7→default(r0x10)、case3=err8→r5"E_GIFT_NOT_READY
    // error"、case4=err9→r6"E_GIFT_UNDER_AGE error"、case5=err10→r7"Undefined cash error"。
    switch (err - 5)
    {
    case 1:      // 置于 case0 前，使块1=r4（NO REMIN）、块2=r3（GIFT_LIMIT）匹配 ORIG 块序
        result = 4;
        printf("[%s] NO REMIN CASH\n", __FUNCTION__);
        break;
    case 0:
        result = 3;
        printf("[%s] E_GIFT_LIMIT_OVER Remain cash\n", __FUNCTION__);
        break;
    case 3:
        result = 5;
        printf("[%s] E_GIFT_NOT_READY error\n", __FUNCTION__);
        break;
    case 4:
        result = 6;
        printf("[%s] E_GIFT_UNDER_AGE error\n", __FUNCTION__);
        break;
    case 5:
        result = 7;
        printf("[%s] Undefined cash error\n", __FUNCTION__);
        break;
    case 2:      // 与 default 同流（跳转表同项）
    default:
        result = 0x10;
        printf("[%s] Undefined cash error: %u\n", __FUNCTION__,
               static_cast<unsigned>(err));
        break;
    }
    return result;
