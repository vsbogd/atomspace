from cpython cimport PyLong_FromLongLong
from cpython.object cimport Py_LT, Py_EQ, Py_GT, Py_LE, Py_NE, Py_GE
from libcpp.set cimport set as cpp_set

# Atom wrapper object
cdef class Atom(Value):

    @staticmethod
    cdef Atom createAtom(const cHandle& handle):
        return create_python_value_from_c_value(<const cValuePtr&>handle)

    def __init__(self, ptr_holder):
        super(Atom, self).__init__(ptr_holder)
        self.handle = <cHandle*>&((<PtrHolder>ptr_holder).shared_ptr)
        # cache the results after first retrieval of
        # immutable properties
        self._atom_type = None
        self._name = None
        self._outgoing = None

    cdef cHandle get_c_handle(Atom self):
        """Return C++ shared_ptr from PtrHolder instance"""
        return <cHandle&>(self.ptr_holder.shared_ptr)

    property atomspace:
        def __get__(self):
            cdef cAtomSpace* a = self.get_c_handle().get().getAtomSpace()
            return AtomSpace_factory(a)

    property name:
        def __get__(self):
            cdef cAtom* atom_ptr
            if self._name is None:
                atom_ptr = self.handle.atom_ptr()
                if atom_ptr == NULL:   # avoid null-pointer deref
                    return None
                if atom_ptr.is_node():
                    self._name = atom_ptr.get_name().decode('UTF-8')
                else:
                    self._name = ""
            return self._name

    property tv:
        def __get__(self):
            cdef cAtom* atom_ptr = self.handle.atom_ptr()
            cdef tv_ptr tvp
            if atom_ptr == NULL:   # avoid null-pointer deref
                return None
            tvp = atom_ptr.getTruthValue()
            if (not tvp.get()):
                raise AttributeError('cAtom returned NULL TruthValue pointer')
            return create_python_value_from_c_value(<shared_ptr[cValue]&>tvp)

        def __set__(self, truth_value):
            try:
                assert isinstance(truth_value, TruthValue)
            except AssertionError:
                raise TypeError("atom.tv property needs a TruthValue object")
            cdef cAtom* atom_ptr = self.handle.atom_ptr()
            if atom_ptr == NULL:   # avoid null-pointer deref
                return
            atom_ptr.setTruthValue(deref((<TruthValue>truth_value)._tvptr()))

    def set_value(self, key, value):
        self.get_c_handle().get().setValue(deref((<Atom>key).handle),
                                (<Value>value).get_c_value_ptr())

    def get_value(self, key):
        cdef cValuePtr value = self.get_c_handle().get().getValue(
            deref((<Atom>key).handle))
        if value.get() == NULL:
            return None
        return create_python_value_from_c_value(value)

    def get_keys(self):
        """
        Returns the keys of values associated with this atom.

        :returns: A list of atoms.
        """
        cdef cpp_set[cHandle] keys = self.get_c_handle().get().getKeys()
        return convert_handle_set_to_python_list(keys)

    def get_out(self):
        cdef cAtom* atom_ptr = self.handle.atom_ptr()
        if atom_ptr == NULL:   # avoid null-pointer deref
            return None
        cdef vector[cHandle] handle_vector = atom_ptr.getOutgoingSet()
        return convert_handle_seq_to_python_list(handle_vector)

    property out:
        def __get__(self):
            if self._outgoing is None:
                atom_ptr = self.handle.atom_ptr()
                if atom_ptr == NULL:   # avoid null-pointer deref
                    return None
                if atom_ptr.is_link():
                    self._outgoing = self.get_out()
                else:
                    self._outgoing = []
            return self._outgoing

    property arity:
        def __get__(self):
            return len(self.out)

    property incoming:
        def __get__(self):
            cdef vector[cHandle] handle_vector
            cdef cAtom* atom_ptr = self.handle.atom_ptr()
            if atom_ptr == NULL:   # avoid null-pointer deref
                return None
            atom_ptr.getIncomingSet(back_inserter(handle_vector))
            return convert_handle_seq_to_python_list(handle_vector)

    def incoming_by_type(self, Type type):
        cdef vector[cHandle] handle_vector
        cdef cAtom* atom_ptr = self.handle.atom_ptr()
        if atom_ptr == NULL:   # avoid null-pointer deref
            return None
        atom_ptr.getIncomingSetByType(back_inserter(handle_vector), type)
        return convert_handle_seq_to_python_list(handle_vector)

    def truth_value(self, mean, count):
        self.tv = createTruthValue(mean, count)
        return self

    def handle_ptr(self):
        return PyLong_FromVoidPtr(self.handle)

    def __richcmp__(self, other, int op):
        assert isinstance(other, Atom), "Only Atom instances are comparable with atoms"
        if op == Py_LT:
            return self.__lt(other)
        if op == Py_EQ:
            return self.__eq(other)
        if op == Py_GT:
            return other.__lt(self)
        if op == Py_LE:
            return not other.__lt(self)
        if op == Py_NE:
            return not self.__eq(other)
        if op == Py_GE:
            return not self.__lt(other)
        raise RuntimeError("unexpected comparison kind: {0}".format(op))

    def __lt(self, other):
        assert isinstance(other, Atom), "Only Atom instances are comparable with atoms"
        cdef cAtom* p = self.get_c_handle().get()
        cdef cAtom* o = ((<Atom>other).get_c_handle()).get()
        return deref(p) < deref(o)

    def __eq(self, other):
        if not isinstance(other, Atom):
            return False
        cdef cAtom* p = self.get_c_handle().get()
        cdef cAtom* o = (<Atom>other).get_c_handle().get()
        return deref(p) == deref(o)

    def __hash__(self):
        return PyLong_FromLongLong(self.get_c_handle().get().get_hash())
