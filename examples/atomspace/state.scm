;
; state.scm - Setting state in the atomspace.
;
; This demonstrates using the StateLink to maintain unique state in the
; atomspace. A Statelink is a kind of link of which only one can ever
; exist in the atomspace, for a given "state" atom.  Whenever a new
; StateLink is added to the atomspace, the old one is automatically
; removed.
;
; StateLinks can be used to associate properties with atoms; see the
; property.scm example.

; The current state of "fruit" is "apple".
(StateLink (AnchorNode "fruit") (ConceptNode "apple"))

; Lets make sure of that:
(cog-incoming-set (AnchorNode "fruit"))

; Change the state to bananna:
(StateLink (AnchorNode "fruit") (ConceptNode "bananna"))

; Lets make sure the state changed:
(cog-incoming-set (AnchorNode "fruit"))

; Change it back to apple:
(StateLink (AnchorNode "fruit") (ConceptNode "apple"))

; Lets make sure the state changed:
(cog-incoming-set (AnchorNode "fruit"))

; Query the current state: the cog-execute! function always returns an
; atom; in this case, it should be the current state.  Note that GetLink
; always returns a SetLink; so that the current state is wrapped in a
; SetLink.
(cog-execute! (GetLink
	(StateLink (AnchorNode "fruit") (VariableNode "$x"))))

; cog-evaluate! is used to evaluate expressions that return truth
; values. So, evaluate, to see if the current state is "apple": this
; should return a truth value corresponding to "true"
(cog-evaluate! (EqualLink (SetLink (ConceptNode "apple"))
	(GetLink (StateLink (AnchorNode "fruit") (VariableNode "$x")))))

; Same as above, but should be false:
(cog-evaluate! (EqualLink (SetLink (ConceptNode "bananna"))
	(GetLink (StateLink (AnchorNode "fruit") (VariableNode "$x")))))

; Change the state, using PutLink:
(cog-execute! (PutLink
	(StateLink (AnchorNode "fruit") (VariableNode "$x"))
	(ConceptNode "strawberry")))

; And, again verify that the stae has changed, as expected:
(cog-execute! (GetLink
	(StateLink (AnchorNode "fruit") (VariableNode "$x"))))
