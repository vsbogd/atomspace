/*
 * opencog/atomspace/BackingStore.h
 *
 * Implements an interface class for storage providers.
 *
 * Copyright (C) 2009, 2013 Linas Vepstas <linasvepstas@gmail.com>
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

#ifndef _OPENCOG_BACKING_STORE_H
#define _OPENCOG_BACKING_STORE_H

#include <set>

#include <opencog/atoms/base/Atom.h>

namespace opencog
{
/** \addtogroup grp_atomspace
 *  @{
 */

/**
 * This class provides a simple, generic interface for dynamically
 * storing/retreiving atoms from disk or other remote location or
 * process. This class focuses on "on-demand" atom retreival,
 * rather than on bulk-save/restore (although perhaps that should
 * be provided as well.)
 */
class BackingStore
{
	public:
		virtual ~BackingStore() {}

		/**
		 * Return a Link with the indicated type and outset,
		 * if it exists; else return nullptr. The returned atom
		 * will have all values attached to it, that the backing
		 * store knows about.
		 */
		virtual Handle getLink(Type, const HandleSeq&) = 0;

		/**
		 * Return a Node with the indicated type and name, if it
		 * exists; else return nullptr. The returned atom will have
		 * all values attached to it, that the backing store knows
		 * about.
		 */
		virtual Handle getNode(Type, const char *) = 0;

		/**
		 * Put the entire incoming set of the indicated handle into
		 * the atom table.
		 */
		virtual void getIncomingSet(AtomTable&, const Handle&) = 0;

		/**
		 * Put all atoms of the given type in the incoming set of the
		 * indicated handle into the atom table.
		 */
		virtual void getIncomingByType(AtomTable&, const Handle&, Type) = 0;

		/**
		 * Recursively store the atom and anything in it's outgoing set.
		 * If the atom is already in storage, this will update it's
		 * truth value, etc. If the `synchronous` flag is set, this
		 * method will not return until the atom has actually been stored.
		 * (Not all backends will respect this flag.)
		 */
		virtual void storeAtom(const Handle&, bool synchronous = false) = 0;

		/**
		 * Remove the indicated atom from the backing store.
		 * If the recursive flag is set, then incoming set of the atom
		 * will also be removed.  If the recursive flag is not set, and
		 * the atom has a non-empty incoming set, then the atom will not
		 * be reomved.
		 */
		virtual void removeAtom(const Handle&, bool recursive) = 0;

		/**
		 * Load *all* atoms of the given type, but only if they are not
		 * already in the AtomTable.  (This avoids truth value merges
		 * between truth values stored in the backend, and truth values
		 * in the atomspace.)
		 */
		virtual void loadType(AtomTable&, Type) = 0;

		/**
		 * Load *all* atoms.
		 */
		virtual void loadAtomSpace(AtomTable&) = 0;

		/**
		 * Store *all* atoms.
		 */
		virtual void storeAtomSpace(const AtomTable&) = 0;

		/**
		 * Read-write synchronization barrier.
		 * All writes will be completed before this routine returns.
		 * This allows the backend to implement asynchronous writes,
		 * while still providing some control to those who need it.
		 * (Mostly the unit tests, at this time.)
		 */
		virtual void barrier() = 0;

		/**
		 * Register this backing store with the atomspace.
		 */
		void registerWith(AtomSpace*);

		/**
		 * Unregister this backing store with the atomspace.
		 */
		void unregisterWith(AtomSpace*);
};

/** @}*/
} //namespace opencog

#endif // _OPENCOG_BACKING_STORE_H
