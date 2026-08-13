#ifndef NSL_ENCYPTTOOLS_H_
#define NSL_ENCYPTTOOLS_H_

namespace nsl {

class EncyptTool;

class EncyptTools
{
public:
    EncyptTools();
    EncyptTool* getEncyptTool();

    EncyptTool* encyptTool_;
};

} // namespace nsl

#endif // NSL_ENCYPTTOOLS_H_
