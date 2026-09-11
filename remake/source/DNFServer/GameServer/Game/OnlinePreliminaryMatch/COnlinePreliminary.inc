COnlinePreliminary::COnlinePreliminary()
{
    if (pWaitingRoomList_ == 0)
        /* ORIG @0855ff42-0x66 value-init 有 6 条标量置零（4.1 对 new T() 先
         * 清零再调 C1@0855ff70）；4.4 视 user-provided ctor 类免置零直调，
         * probe(placement new+try) 反而引入 operator new(void*,ptr) call +
         * EH spill，保留直译，置零差记 N 类 */
        pWaitingRoomList_ = new CLeagueMatchRoomList();   // ORIG new(0x1c)+置零+C1
    nWaitingRoomListRefCount_ = nWaitingRoomListRefCount_ + 1;
}