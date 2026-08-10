#ifndef USER_H_
#define USER_H_

namespace RelayServiceApp
{
// ---- UserState（独立空类，只有用户构造、平凡析构；ORIG 无 UserStateD1 符号）----
class UserState
{
public:
    UserState()
    {
    }
};


// ---- User：继承 UserState（EBO），m_acc_id@0 / m_user_id@4 ----
class User : public UserState
{
public:
    User();
    ~User();
    int m_acc_id;
    int m_user_id;
};


} // namespace RelayServiceApp

#endif // USER_H_
