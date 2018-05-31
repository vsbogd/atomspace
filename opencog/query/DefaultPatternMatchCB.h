/*
 * DefaultPatternMatchCB.h
 *
 * Copyright (C) 2009, 2014, 2015 Linas Vepstas <linasvepstas@gmail.com>
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
 *
 * Created by Linas Vepstas January 2009
 */

#ifndef _OPENCOG_DEFAULT_PATTERN_MATCH_H
#define _OPENCOG_DEFAULT_PATTERN_MATCH_H

#include <opencog/util/ThreadLocal.h>
#include <opencog/atoms/proto/types.h>
#include <opencog/atoms/core/Quotation.h>
#include <opencog/atoms/execution/Instantiator.h>
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/query/PatternMatchCallback.h>
#include <opencog/query/PatternMatchEngine.h>

namespace opencog {

/**
 * Callback mixin class, used to provide a default node and link
 * matching behaviour. This class is a pure virtual class, since
 * it does not implement either the `initiate_search()` method,
 * nor the `solution()` method.
 *
 * It provides is node and link matching, assuming the canonical
 * meaning of VariableNodes and QuoteLinks. It also implements
 * crisp-logic handling of AndLink, OrLink, NotLink when these
 * are combined with AbsentLink, EqualLink, GreaterThanLink, and
 * other clear-box evaluatable link types.
 *
 * It handles AbsentLink using standard intuitionist logic,
 * and provides tracking of an _optionals_present flag, so that
 * conversion to explicit classical logic can be performed at the
 * conclusion of the search.  The default implicator performs
 * conversion; see the notes there for details.
 */
class DefaultPatternMatchCB : public virtual PatternMatchCallback
{
	public:
		DefaultPatternMatchCB(AtomSpace*);
		virtual void set_pattern(const Variables&, const Pattern&);

		virtual bool node_match(const Handle&, const Handle&);
		virtual bool variable_match(const Handle&, const Handle&);
		virtual bool scope_match(const Handle&, const Handle&);

		virtual bool link_match(const PatternTermPtr&, const Handle&);
		virtual bool post_link_match(const Handle&, const Handle&);
		virtual void post_link_mismatch(const Handle&, const Handle&);

		virtual bool clause_match(const Handle&, const Handle&,
		                          const HandleMap&);
		/**
		 * Typically called for AbsentLink
		 */
		virtual bool optional_clause_match(const Handle& pattrn,
		                                   const Handle& grnd,
		                                   const HandleMap&);

		virtual IncomingSet get_incoming_set(const Handle&);

		/**
		 * Called when a virtual link is encountered. Returns false
		 * to reject the match.
		 */
		virtual bool evaluate_sentence(const Handle& pat, const HandleMap& gnds)
		{ return eval_sentence(pat, gnds); }

		virtual const TypeSet& get_connectives(void)
		{
			return _connectives;
		}

		bool optionals_present(void) { return _optionals_present; }
	protected:

		NameServer& _nameserver;

		const Variables* _vars = NULL;
		const HandleSet* _dynamic = NULL;
		bool _have_evaluatables = false;
		const HandleSet* _globs = NULL;

		bool _have_variables;
		Handle _pattern_body;

		bool is_self_ground(const Handle&, const Handle&,
		                    const HandleMap&, const HandleSet&,
		                    Quotation quotation=Quotation()) const;

        /**
         * Variables that should be ignored, because they are bound
         * (scoped) in the current context (i.e. appear in a ScopeLink
         * that is being matched.)
         */
        class ScopedVariables
        {
            const Variables* pattern_vars = nullptr;
            const Variables* grounding_vars = nullptr;

        public:

            bool is_set() const noexcept
            {
                return pattern_vars != nullptr;
            }

            void set(const Variables* pattern_vars, const Variables* grounding_vars)
                    noexcept
            {
                this->pattern_vars = pattern_vars;
                this->grounding_vars = grounding_vars;
            }

            void clear() noexcept
            {
                pattern_vars = nullptr;
                grounding_vars = nullptr;
            }

            bool is_alpha_convertible(const Handle& npat_h,
                    const Handle& nsoln_h) const
            {
                return pattern_vars->is_alpha_convertible(npat_h, nsoln_h,
                        *grounding_vars);
            }

            bool is_in_varset(const Handle& npat_h) const
            {
                return pattern_vars->is_in_varset(npat_h);
            }
        };
        /**
         * Thread local values to execute scope_match(), link_match(),
         * post_link_match(), post_link_mismatch() methods independently.
         */
        ThreadLocal<ScopedVariables> scoped_vars;

        /**
         *  Temp atomspace used for test-groundings of virtual links.
         */
        class TempAtomSpace
        {
            AtomSpace* _temp_aspace;
            Instantiator* _instor;

        public:

            TempAtomSpace(AtomSpace* parent_atomspace) :
                    _temp_aspace(grab_transient_atomspace(parent_atomspace)), _instor(
                            new Instantiator(_temp_aspace))
            {
            }

            ~TempAtomSpace()
            {
                clear();
            }

            TempAtomSpace(const TempAtomSpace&) = delete;
            TempAtomSpace(TempAtomSpace&&) = delete;
            TempAtomSpace& operator=(const TempAtomSpace&) = delete;
            TempAtomSpace& operator=(TempAtomSpace&&) = delete;

            AtomSpace* get_clean_atomspace()
            {
                _temp_aspace->clear();
                return _temp_aspace;
            }

            Instantiator* get_instantiator()
            {
                return _instor;
            }

            void clear()
            {
                // If we have a transient atomspace, release it.
                if (_temp_aspace) {
                    release_transient_atomspace(_temp_aspace);
                    _temp_aspace = nullptr;
                }
                // Delete the instantiator.
                if (_instor) {
                    delete _instor;
                    _instor = nullptr;
                }
            }

#ifdef CACHED_IMPLICATOR
            void set_parent_atomspace(AtomSpace* parent_atomspace)
            {
                _temp_aspace = grab_transient_atomspace(parent_atomspace);
                _instor->ready(_temp_aspace);
            }
#endif
        };
        /**
         * Thread local temporary atomspace to evaluate sentences from
         * different threads independently
         */
        ThreadLocal<TempAtomSpace> temp_atomspace;

		// The transient atomspace cache. The goal here is to
		// avoid the overhead of constantly creating/deleting
		// the temp atomspaces above. So instead, just keep a
		// cache of empty ones, ready to go.
		static std::mutex s_transient_cache_mutex;
		static std::vector<AtomSpace*> s_transient_cache;
		static AtomSpace* grab_transient_atomspace(AtomSpace* parent);
		static void release_transient_atomspace(AtomSpace* atomspace);

#ifdef CACHED_IMPLICATOR
		virtual void ready(AtomSpace*);
		virtual void clear();
#endif
		// Crisp-logic evaluation of evaluatable terms
		TypeSet _connectives;
		bool eval_term(const Handle& pat, const HandleMap& gnds);
		bool eval_sentence(const Handle& pat, const HandleMap& gnds);

		std::atomic<bool> _optionals_present;
		AtomSpace* _as;
};

} // namespace opencog

#endif // _OPENCOG_DEFAULT_PATTERN_MATCH_H
