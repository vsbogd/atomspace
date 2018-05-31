
from opencog.atomspace import AtomSpace, types
from opencog.utilities import initialize_opencog, finalize_opencog
from opencog.bindlink import execute_atom
import opencog.scheme_wrapper as scheme
from opencog.scheme_wrapper import load_scm, scheme_eval
from opencog.type_constructors import *

atomspace = AtomSpace()

# Initialize Scheme
scheme_preload = [
                    "opencog/atoms/proto/core_types.scm",
                    "opencog/scm/utilities.scm"
                 ]

for scheme_file in scheme_preload:
    load_scm(atomspace, scheme_file)

initialize_opencog(atomspace)

executed = False

def add_link(atom1, atom2):
    global executed
    link = ListLink(atom1, atom2)
    executed = True
    return link

# Fix up the module for cog-execute. Most Python users won't be using cog-execute
# since there is now a Cython call execute_atom which does the same thing, so we
# don't need to add the library path below under normal Python initialization.
scheme_eval(atomspace, '(add-to-load-path "/usr/local/share/opencog/scm")')
scheme_eval(atomspace, '(use-modules (opencog exec))')

execute_code = \
    '''
    (cog-execute!
        (ExecutionOutputLink
            (GroundedSchemaNode \"py: add_link\")
            (ListLink
                (ConceptNode \"one\")
                (ConceptNode \"two\")
            )
        )
    )
    '''
scheme_eval(atomspace, execute_code)

print ("execute: cog-execute")
if (executed):
    print ("add_link - executed successfully")
else:
    print ("add_link - did NOT execute")

executed = False
execute_atom( atomspace,
    ExecutionOutputLink(
        GroundedSchemaNode("py: add_link"),
        ListLink(
            ConceptNode("one"),
            ConceptNode("two")
        )
    )
)
print ("execute: execute_atom")
if (executed):
    print ("add_link - executed successfully")
else:
    print ("add_link - did NOT execute")


finalize_opencog()
del atomspace
