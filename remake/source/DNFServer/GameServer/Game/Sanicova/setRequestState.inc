void CPad::setRequestState(ENUM_REQUEST_STATE state)      // ORIG W 0x0822efcc
{
    // ORIG 0822efcc: this-先 store（mov 0x8,%eax; mov 0xc,%edx; 写回）。
    // N76 同型化翻案（NL7 定律）：m_requestState40 int→ENUM_REQUEST_STATE
    // （CUser.h Main 代落；st10 探针实证同型 store 出 this-先形），落 identical。
    m_requestState40 = state;
