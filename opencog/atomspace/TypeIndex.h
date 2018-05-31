/*
 * opencog/atomspace/TypeIndex.h
 *
 * Copyright (C) 2008 Linas Vepstas <linasvepstas@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _OPENCOG_TYPEINDEX_H
#define _OPENCOG_TYPEINDEX_H

#include <set>
#include <vector>

#include <opencog/atoms/base/Atom.h>
#include <opencog/atoms/base/Handle.h>
#include <opencog/atoms/proto/types.h>
#include <opencog/atomspace/FixedIntegerIndex.h>

namespace opencog
{
/** \addtogroup grp_atomspace
 *  @{
 */

/**
 * Implements an integer index as an RB-tree (C++ set) That is, given
 * an atom Type, this returns all of the Handles for that Type.
 *
 * The primary interface for this is an iterator, and that is because
 * the index will typically contain millions of atoms, and this is far
 * too much to try to return in some temporary array.  Iterating is much
 * faster.
 *
 * @todo The iterator is NOT thread-safe against the insertion or
 * removal of atoms!  Either inserting or removing an atom will cause
 * the iterator references to be freed, leading to mystery crashes!
 */
class TypeIndex : public FixedIntegerIndex
{
	private:
		size_t num_types;
	public:
		TypeIndex(void);
		void resize(void);
		void insertAtom(Atom* a)
		{
			insert(a->get_type(), a);
		}
		void removeAtom(Atom* a)
		{
			remove(a->get_type(), a);
		}

		class iterator
			: public HandleIterator
		{
			friend class TypeIndex;
			public:
				iterator(Type, bool);
				iterator& operator++();
				iterator& operator++(int);
				iterator& operator=(iterator);
				bool operator==(iterator);
				bool operator!=(iterator);
				Handle operator*(void);
			private:
				Type type;
				bool subclass;
				std::vector<AtomSet>::const_iterator s;
				std::vector<AtomSet>::const_iterator send;
				Type currtype;
				AtomSet::const_iterator se;
		};

		iterator begin(Type, bool) const;
		iterator end(void) const;
};

/** @}*/
} //namespace opencog

#endif // _OPENCOG_TYPEINDEX_H
