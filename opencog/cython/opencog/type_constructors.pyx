
# This file reads files that are generated by the OPENCOG_ADD_ATOM_TYPES
# macro so they can be imported using:
#
# from type_constructors import *
#
# This imports all the python wrappers for atom creation.
#

from opencog.atomspace import AtomSpace, TruthValue, types

atomspace = None
def set_type_ctor_atomspace(new_atomspace):
    global atomspace
    atomspace = new_atomspace

include "opencog/atoms/proto/core_types.pyx"
