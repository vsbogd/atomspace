/*
 * Present.cc
 *
 * Copyright (C) 2017 Linas Vepstas
 *
 * Author: Linas Vepstas <linasvepstas@gmail.com>  January 2009, 2015, 2017
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the
 * exceptions at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public
 * License along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <opencog/util/mt19937ar.h>

#include "PresentLink.h"

using namespace opencog;

void PresentLink::init(void)
{
	// The UnorderedLink ctor will have already sorted the outgoing set
	// for us into some order.  To find duplicates, we merely need to
	// iterate over the outgoing set, comparing neighbors.

	Arity sz = _outgoing.size();
	if (0 == sz) return;

	HandleSeq uniq;
	uniq.push_back(_outgoing[0]);  // The first one is always good.

	// Look for an remove duplicates.
	Arity lst = 0;
	Arity nxt = 1;
	while (nxt < sz)
	{
		if (_outgoing[lst] != _outgoing[nxt])
		{
			uniq.push_back(_outgoing[nxt]);
			lst = nxt;
		}
		nxt++;
	}

	// swap into place; faster than copy.
	_outgoing.swap(uniq);
}

PresentLink::PresentLink(const HandleSeq& oset, Type t)
	: UnorderedLink(oset, t)
{
	if (not nameserver().isA(t, PRESENT_LINK))
	{
		const std::string& tname = nameserver().getTypeName(t);
		throw InvalidParamException(TRACE_INFO,
			"Expecting an PresentLink, got %s", tname.c_str());
	}

	init();
}

PresentLink::PresentLink(const Link& l)
	: UnorderedLink(l)
{
	// Type must be as expected
	Type tscope = l.get_type();
	if (not nameserver().isA(tscope, PRESENT_LINK))
	{
		const std::string& tname = nameserver().getTypeName(tscope);
		throw InvalidParamException(TRACE_INFO,
			"Expecting an PresentLink, got %s", tname.c_str());
	}

	// We have to call init here, because the input link l might
	// not have gone through a PresentLink constructor before.
	init();
}

// ---------------------------------------------------------------

DEFINE_LINK_FACTORY(PresentLink, PRESENT_LINK)

/* ===================== END OF FILE ===================== */
