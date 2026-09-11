// ORIG 0x08431d8c sz 0x141（_ZN16DB_LoadDimension11makeRequestEij）：
// Acquire("DBThread.cpp",0x7916) → CStreamGuard(stream,true)
//   → <<0x14e(int) → <<param_1(int) → sig=GetInBuffer<SIG_LOAD_DIMENSION>()
//   → sig+0x00(uint)=param_2；i=0..5：sig+0x04+i(byte)=(char)
//     G_CDataManager()->get_dimensionInout(i)；sig+0x0c(byte)=(char)
//     G_CDataManager()->get_limit_inout_count(2) → put(DB_Q(2))。
void DB_LoadDimension::makeRequest(int userIdx, unsigned int param2)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7916), true);
    guard.operator*()->operator<<((int)0x14e);
    guard.operator*()->operator<<((int)userIdx);
    SIG_LOAD_DIMENSION* sig = guard->GetInBuffer<SIG_LOAD_DIMENSION>();
    // ORIG 0x08431e74(+0xe8) `movsbl %al,%eax; mov %eax,-0x14(%ebp);
    // mov -0x14(%ebp),%eax; mov %eax,%edx; mov -0x18(%ebp),%eax;
    // mov %dl,0xc(%eax)` ⇒ limit 值为跨循环边界的具名 int 槽（-0x14，
    // 声明位与 sig(-0x18)/guard(-0x20) 槽序一致，技巧21），store 走
    // 槽回读→%edx 隐式窄化（无显式 cast，避免折叠）。
    int limitVal;
    sig->m_field00 = param2;
    for (int i = 0; i <= 5; ++i)
    {
        // ORIG 0x08431d8c 体：`movsbl %al,%eax; mov %eax,-0x14(%ebp);
        // mov -0x14(%ebp),%eax; mov %eax,%edx; mov %dl,...` —— v int 槽
        // 物化（char 返回→int 扩展存槽，(char) 截断隐式）。4.4 显式
        // (char)v 会把槽折叠，⇒ 去掉显式 cast 走隐式窄化。
        // 残留（blocked N18a）：ORIG v 有 int 槽（`movsbl %al,%eax; mov
        // %eax,-0x14; mov -0x14,%eax; mov %eax,%edx` 四步），4.4 对单使用
        // v 恒 copy-forward 折槽；volatile 探针反产特殊远槽（-0x1c）全帧
        // 更乱，已回退。
        int v = G_CDataManager()->get_dimensionInout(i);
        sig->m_dimensionInout[i] = v;
    }
    limitVal = G_CDataManager()->get_limit_inout_count((ENUM_BLOOD_TYPE)2);
    sig->m_limitInoutCount = limitVal;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}