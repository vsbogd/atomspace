/*
 * opencog/atoms/proto/RandomStream.h
 *
 * Copyright (C) 2015, 2018 Linas Vepstas
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

#ifndef _OPENCOG_RANDOM_STREAM_H
#define _OPENCOG_RANDOM_STREAM_H

#include <vector>
#include <opencog/atoms/proto/StreamValue.h>
#include <opencog/atoms/proto/atom_types.h>

namespace opencog
{

/** \addtogroup grp_atomspace
 *  @{
 */

/**
 * RandomStreams provide an example of streaming data.
 */
class RandomStream
	: public StreamValue
{
protected:
	RandomStream(Type t) : StreamValue(t) {}
	int _len;

	virtual void update() const;

public:
	// int is the desired size of the vector.
	RandomStream(int=1);
	virtual ~RandomStream() {}

	/** Returns a string representation of the value.  */
	virtual std::string to_string(const std::string& indent = "") const;
};

typedef std::shared_ptr<RandomStream> RandomStreamPtr;
static inline RandomStreamPtr RandomStreamCast(ProtoAtomPtr& a)
	{ return std::dynamic_pointer_cast<RandomStream>(a); }

#define createRandomStream std::make_shared<RandomStream>


/** @}*/
} // namespace opencog

#endif // _OPENCOG_RANDOM_STREAM_H
