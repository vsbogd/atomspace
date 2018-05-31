/*
 * DefaultImplicator.h
 *
 * Copyright (C) 2009, 2014 Linas Vepstas
 *
 * Author: Linas Vepstas <linasvepstas@gmail.com>  January 2009
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

#ifndef _OPENCOG_DEFAULT_IMPLICATOR_H
#define _OPENCOG_DEFAULT_IMPLICATOR_H

#include "DefaultPatternMatchCB.h"
#include "Implicator.h"
#include "InitiateSearchCB.h"
#include "PatternMatchCallback.h"


namespace opencog {

class DefaultImplicator:
	public virtual Implicator,
	public virtual InitiateSearchCB,
	public virtual DefaultPatternMatchCB
{
	public:
		DefaultImplicator(AtomSpace* asp) :
			Implicator(asp),
			InitiateSearchCB(asp),
			DefaultPatternMatchCB(asp) {}

#ifdef CACHED_IMPLICATOR
	virtual void ready(AtomSpace* asp)
	{
		Implicator::ready(asp);
		InitiateSearchCB::ready(asp);
		DefaultPatternMatchCB::ready(asp);
	}

	virtual void clear()
	{
		Implicator::clear();
		InitiateSearchCB::clear();
		DefaultPatternMatchCB::clear();
	}
#endif

	virtual void set_pattern(const Variables& vars,
	                         const Pattern& pat)
	{
		InitiateSearchCB::set_pattern(vars, pat);
		DefaultPatternMatchCB::set_pattern(vars, pat);
	}
};

#ifdef CACHED_IMPLICATOR
class CachedDefaultImplicator {
	static DefaultImplicator* _cached_implicator;
	public:
		CachedDefaultImplicator(AtomSpace*asp);
		~CachedDefaultImplicator();
		operator Implicator&() { return *_cached_implicator; }
};
#endif

Handle do_imply(AtomSpace*, const Handle&, Implicator&, bool);

}; // namespace opencog

#endif // _OPENCOG_DEFAULT_IMPLICATOR_H
