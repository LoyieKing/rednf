// ORIG D1 0x84922d6 W：仅对 +0x1fd20 SIG_LOAD_ETC 调用一次析构；其余七个
//   子对象/POD 区无用户析构 ⇒ 编译器析构序一致。
CACHE_CHARACTER_TYPE::~CACHE_CHARACTER_TYPE()
{
}