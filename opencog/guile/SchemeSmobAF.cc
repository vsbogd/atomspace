/*
 * opencog/guile/SchemeSmobAF.cc
 *
 * Scheme small objects (SMOBS) for AttentionBank.
 *
 * Copyright (C) 2014 Cosmo Harrigan
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

#include <cstddef>
#include <libguile.h>

#include <opencog/guile/SchemeSmob.h>
#include <opencog/attentionbank/AttentionBank.h>

using namespace opencog;

/**
 *   Return AttentionalFocus Size
 **/
SCM SchemeSmob::ss_af_size (void)
{
    AtomSpace* atomspace = ss_get_env_as("cog-af-size");
    return scm_from_int(attentionbank(atomspace).get_af_size());
}

/**
 * Set AttentionalFocus Size
 */
SCM SchemeSmob::ss_set_af_size (SCM ssize)
{
    AtomSpace* atomspace = ss_get_env_as("cog-set-af-size!");
    if (scm_is_false(scm_integer_p(ssize)))
        scm_wrong_type_arg_msg("cog-set-af-size", 1, ssize,
                "integer opencog AttentionalFocus size");

    int bdy = scm_to_int(ssize);
    attentionbank(atomspace).set_af_size(bdy);
    return scm_from_int(attentionbank(atomspace).get_af_size());
}

/**
 * Return the list of atoms in the AttentionalFocus
 */
SCM SchemeSmob::ss_af (void)
{
	AtomSpace* atomspace = ss_get_env_as("cog-af");
	HandleSeq attentionalFocus;
	attentionbank(atomspace).get_handle_set_in_attentional_focus(back_inserter(attentionalFocus));
	size_t isz = attentionalFocus.size();
	if (0 == isz) return SCM_EOL;

	SCM head = SCM_EOL;
	for (size_t i = 0; i < isz; i++) {
		Handle hi = attentionalFocus[i];
		SCM smob = handle_to_scm(hi);
		head = scm_cons(smob, head);
	}

	return head;
}

/**
 * Return the top N atoms from the AF based on their STI value.
 * Setting N to a value larger than the AF size will be equivalent
 * to calling ss_af or cog-af from scheme.
 */
SCM SchemeSmob::ss_af_topn (SCM n)
{
	AtomSpace* atomspace = ss_get_env_as("cog-af-topn");
	HandleSeq attentionalFocus;
	attentionbank(atomspace).get_handle_set_in_attentional_focus(back_inserter(attentionalFocus));
	size_t isz = attentionalFocus.size();
	if (0 == isz) return SCM_EOL;

	SCM head = SCM_EOL;
	int N = scm_to_int(n);
	if( N > isz)  N = isz;
	for (size_t i = isz - N; i < isz ; i++) {
		Handle hi = attentionalFocus[i];
		SCM smob = handle_to_scm(hi);
		head = scm_cons(smob, head);
	}

	return head;
}

/**
 *  Stimulate an atom with given stimulus amount.
 */
SCM SchemeSmob::ss_stimulate (SCM satom, SCM sstimulus)
{
	Handle h(scm_to_handle(satom));
	double stimulus = scm_to_double(sstimulus);
	AtomSpace* atomspace = ss_get_env_as("cog-stimulate");
	attentionbank(atomspace).stimulate(h, stimulus);
	return satom;
}
