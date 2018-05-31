#include <opencog/atoms/base/Handle.h>
#include <opencog/atoms/base/Atom.h>
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/atoms/execution/ExecutionOutputLink.h>
#include <opencog/atoms/core/FunctionLink.h>

#include "BindlinkStub.h"

using namespace opencog;

Handle opencog::stub_bindlink(AtomSpace* atomspace, Handle handle)
{
    return handle;
}

Handle opencog::do_execute(AtomSpace* atomspace, Handle handle)
{
    // XXX FIXME: the FunctionLink::do_execute should, in theory,
    // be able to handle the ExecutionOutputLink. However, currently,
    // it cannot, because putting this code there leads to a circular
    // shared library dependency (PythonEval depends on
    // ExecutionOutputLink depends on PythonEval, ad infinitum) and
    // so cmake refused to compile.  It would be great if this circular
    // dependency of python on itself was somehow broken, but, given
    // what ExecutionOutputLink does, its hard to see how to fix this.
    //
    if (EXECUTION_OUTPUT_LINK == handle->get_type()) {
        LinkPtr lp(LinkCast(handle));
        ExecutionOutputLinkPtr eolp(ExecutionOutputLinkCast(lp));
        if (NULL == eolp)
            eolp = createExecutionOutputLink(*lp);
        return eolp->execute(atomspace);
    }

    FunctionLinkPtr flp(FunctionLinkCast(handle));
    if (flp) return HandleCast(flp->execute());
    return Handle();
}
