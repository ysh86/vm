	.sect	.text

	.define	_k2u
_k2u:
	move.l	(sp)+,a0	! discard return addr
	rte			! goto user land

