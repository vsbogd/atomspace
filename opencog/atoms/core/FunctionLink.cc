/*
 * opencog/atoms/core/FunctionLink.cc
 *
 * Copyright (C) 2015 Linas Vepstas
 * All Rights Reserved
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

#include <opencog/atoms/proto/atom_types.h>
#include <opencog/atoms/base/ClassServer.h>
#include "FunctionLink.h"

using namespace opencog;

void FunctionLink::check_type(Type t)
{
	if (not nameserver().isA(t, FUNCTION_LINK))
		throw InvalidParamException(TRACE_INFO, "Expecting a FunctionLink");
}

void FunctionLink::init(void)
{
	FreeLink::init();
}

FunctionLink::FunctionLink(const HandleSeq& oset, Type t)
    : FreeLink(oset, t)
{
	check_type(t);
	init();
}

FunctionLink::FunctionLink(const Link& l)
    : FreeLink(l)
{
	check_type(l.get_type());
	init();
}

ProtoAtomPtr FunctionLink::execute() const
{
	throw RuntimeException(TRACE_INFO, "Not executable: %s\n",
		nameserver().getTypeName(get_type()).c_str());
}

DEFINE_LINK_FACTORY(FunctionLink, FUNCTION_LINK);

/* ===================== END OF FILE ===================== */
