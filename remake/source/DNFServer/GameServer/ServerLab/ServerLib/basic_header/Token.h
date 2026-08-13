#ifndef NSL_TOKEN_H_
#define NSL_TOKEN_H_

namespace nsl {

// ORIG 的 assert 消息为 "idx0>=0 && idx0<MAX_TOKEN"（未展开）：ORIG 用的
// glibc assert.h 经 __STRING 嵌套宏字符串化，宏实参会被展开，只有非常量宏
// 的标识符（enum 常量）才能在消息里保留原名；enum 常量也不会像 namespace 级
// const int 那样在被 odr-use 时生成 `_ZN3nslL9MAX_TOKENE` 数据符号。
enum { MAX_TOKEN = 256 };

class Token
{
public:
    Token();
    ~Token();
    bool get_token(char* str, int size);
    char* get_context(int idx0);
    void free_token(int idx0);
    int get_n_token();

private:
    int n_token;
    char seps[52];
    char* m_token[MAX_TOKEN];
};

} // namespace nsl

#endif // NSL_TOKEN_H_
