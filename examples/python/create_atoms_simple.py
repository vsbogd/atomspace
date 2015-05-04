#! /usr/bin/env python
#
# create_atoms_simple.py
#
"""
A nicer way of creating atoms in the AtomSpace.
"""

from opencog.atomspace import AtomSpace, TruthValue, Atom
from opencog.atomspace import types as t
from opencog.utilities import initialize_opencog, finalize_opencog
from opencog.type_constructors import *


a = AtomSpace()
set_atomspace(a)

# Create a truth value asserting true and confident.
TV = TruthValue(1,1)

# Add three nodes
# A = a.add_node(t.ConceptNode, 'Apple', TV)
A = ConceptNode("Apple", TV)

B = a.add_node(t.ConceptNode, 'Berry', TruthValue(0.5,1))
C = a.add_node(t.ConceptNode, 'Comestible', TV)

# Add three inhertance links, asserting that apples are berries
# and that betrries are edible.
AB = a.add_link(t.InheritanceLink, [A, B], TV)
BC = a.add_link(t.InheritanceLink, [B, C], TV)
AC = a.add_link(t.InheritanceLink, [A, C])


print "the atomsapce contains:\n", a.get_atoms_by_type(t.Atom)
