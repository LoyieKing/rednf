// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_singleton.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_SINGLETON_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_SINGLETON_H_H_

// sizeof = 1
struct Singleton<tss_sdk_impl::TssSdkCommSvc> { // line 10
    // sizeof = 1
    struct ObjectCreator {
    public:
    ObjectCreator(); // line 23
    void do_nothing() const; // line 28
    };
private:
static Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator object_creator;
public:
static /*anon struct*/ int * instance(); // line 13
};
// sizeof = 1
struct Singleton<tss_sdk_impl::TssSdkPluginMgr> { // line 10
    // sizeof = 1
    struct ObjectCreator {
    public:
    ObjectCreator(); // line 23
    void do_nothing() const; // line 28
    };
private:
static Singleton<tss_sdk_impl::TssSdkCommSvc>::ObjectCreator object_creator;
public:
static TssSdkPluginMgr * instance(); // line 13
};
namespace Singleton<tss_sdk_impl::TssSdkCommSvc> {
    // sizeof = 1
    struct ObjectCreator { // line 22
    public:
    ObjectCreator(); // line 23
    void do_nothing() const; // line 28
    };
} // namespace Singleton<tss_sdk_impl::TssSdkCommSvc>
namespace Singleton<tss_sdk_impl::TssSdkPluginMgr> {
    // sizeof = 1
    struct ObjectCreator { // line 22
    public:
    ObjectCreator(); // line 23
    void do_nothing() const; // line 28
    };
} // namespace Singleton<tss_sdk_impl::TssSdkPluginMgr>

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_SINGLETON_H_H_
