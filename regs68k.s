	.sect	.text

	.define	_getsr
	.define _getcacr
	.define _getsfc
	.define _setsfc
	.define _getdfc
	.define _setdfc
	.define _getusp
	.define _setusp
	.define _readphy
	.define _writephy

_getsr:
	move.w	sr,d0
	rts
_getcacr:
	! movec	cacr,d0
        ! 0100 1110 0111 101 0 0 000 0000 0000 0010
	.data4	0x4e7a0002
	rts

_getsfc:
	movec	sfc,d0
	rts
_setsfc:
	movec	sfc,d0
	move.w	4(a7),d1
	movec	d1,sfc
	rts

_getdfc:
	movec	dfc,d0
	rts
_setdfc:
	movec	dfc,d0
	move.w	4(a7),d1
	movec	d1,dfc
	rts

_getusp:
	movec	usp,d0
	rts
_setusp:
	movec	usp,d0
	move.l	4(a7),d1
	movec	d1,usp
	rts

_readphy:
	move.l	4(a7),a0	! src @ PA(sfc=7)
	moves.l	(a0),d0
	!moves.l	([4,a7]),d0
	rts
_writephy:
	move.l	8(a7),d0	! src data
	move.l	4(a7),a0	! dst @ PA(dfc=7)
	moves.l	d0,(a0)
	!moves.l	d0,([4,a7])
	rts

