#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "program.h"


CLASS_PTR(Context)
class Context {
public:
    static ContextUPtr Create();
    void Render();

private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;
};


#endif