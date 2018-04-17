/*
 * PatternMatchCallbackThreadSafe.h
 *
 * Author: Vitaly Bogdanov
 *
 * Copyright (C) 2018 Vitaly Bogdanov <vsbogd@gmail.com>
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
#ifndef OPENCOG_QUERY_PATTERNMATCHCALLBACKTHREADSAFE_H_
#define OPENCOG_QUERY_PATTERNMATCHCALLBACKTHREADSAFE_H_

#include <mutex>

#include "PatternMatchCallback.h"

namespace opencog {

class PatternMatchCallbackThreadSafe: public PatternMatchCallback {

	PatternMatchCallback& delegate;

	std::mutex evaluate_sentence_mutex;
	std::mutex grounding_mutex;

public:

	PatternMatchCallbackThreadSafe(PatternMatchCallback& delegate) :
			delegate(delegate) {
	}

	virtual ~PatternMatchCallbackThreadSafe() {
	}

	virtual bool node_match(const Handle& patt_node, const Handle& grnd_node) {
		return delegate.node_match(patt_node, grnd_node);
	}

	virtual bool variable_match(const Handle& patt_node,
			const Handle& grnd_node) {
		return delegate.variable_match(patt_node, grnd_node);
	}

	virtual bool scope_match(const Handle& patt_node, const Handle& grnd_node) {
		return delegate.scope_match(patt_node, grnd_node);
	}

	virtual bool link_match(const PatternTermPtr& patt_link,
			const Handle& grnd_link) {
		return delegate.link_match(patt_link, grnd_link);
	}

	virtual bool post_link_match(const Handle& patt_link,
			const Handle& grnd_link) {
		return delegate.post_link_match(patt_link, grnd_link);
	}

	virtual void post_link_mismatch(const Handle& patt_link,
			const Handle& grnd_link) {
		delegate.post_link_match(patt_link, grnd_link);
	}

	virtual bool fuzzy_match(const Handle& ph, const Handle& gh) {
		return delegate.fuzzy_match(ph, gh);
	}

	virtual bool evaluate_sentence(const Handle& eval, const HandleMap& gnds) {
	    std::lock_guard<std::mutex> lock(evaluate_sentence_mutex);
		return delegate.evaluate_sentence(eval, gnds);
	}

	virtual bool clause_match(const Handle& pattrn_link_h,
			const Handle& grnd_link_h, const HandleMap& term_gnds) {
		return delegate.clause_match(pattrn_link_h, grnd_link_h, term_gnds);
	}

	virtual bool optional_clause_match(const Handle& pattrn, const Handle& grnd,
			const HandleMap& term_gnds) {
		return delegate.optional_clause_match(pattrn, grnd, term_gnds);
	}

	virtual bool grounding(const HandleMap &var_soln,
			const HandleMap &term_soln) {
		std::lock_guard<std::mutex> lock(grounding_mutex);
		return delegate.grounding(var_soln, term_soln);
	}

	virtual IncomingSet get_incoming_set(const Handle& h) {
		return delegate.get_incoming_set(h);
	}

	virtual void push(void) {
		delegate.push();
	}

	virtual void pop(void) {
		delegate.pop();
	}

	virtual const TypeSet& get_connectives(void) {
		return delegate.get_connectives();
	}

	virtual bool initiate_search(PatternMatchEngine *pme) {
		return delegate.initiate_search(pme);
	}

	virtual bool search_finished(bool done) {
		return delegate.search_finished(done);
	}

	virtual void set_pattern(const Variables& vars, const Pattern& pat) {
		return delegate.set_pattern(vars, pat);
	}

};

} /* namespace opencog */

#endif /* OPENCOG_QUERY_PATTERNMATCHCALLBACKTHREADSAFE_H_ */
