	.sect	.text

	.define _getcrp
	.define _setcrp
	.define _getsrp
	.define _setsrpfd
	.define _gettc
	.define _settcfd
	.define _gettt0
	.define _gettt1
	.define _settt0fd
	.define _settt1fd

_getcrp:
	move.l	4(a7),a0	! arg
	! pmove.q crp,(a0)
	! 1111 000000 010 000 010 011 10 0000 0000
	.data4	0xf0104e00
	rts
_setcrp:
	move.l	4(a7),a0	! arg
	! pmove.q (a0),crp
	! 1111 000000 010 000 010 011 00 0000 0000
	.data4	0xf0104c00
	rts

_getsrp:
	move.l	4(a7),a0	! arg
	! pmove.q srp,(a0)
	! 1111 000000 010 000 010 010 10 0000 0000
	.data4	0xf0104a00
	rts
_setsrpfd:
	move.l	4(a7),a0	! arg
	! pmovefd.q (a0),srp
	! 1111 000000 010 000 010 010 01 0000 0000
	.data4	0xf0104900
	rts

_gettc:
	move.l	d0,-(a7)	! dummy
	! pmove.l tc,(a7)
        ! 1111 000000 010 111 010 000 10 0000 0000
        .data4	0xf0174200
	move.l	(a7)+,d0
	rts
_settcfd:
	! pmovefd.l 4(a7),tc
        ! 1111 000000 101 111 010 000 01 0000 0000
	.data4	0xf02f4100
	.data2	0x0004
	rts

_gettt0:
	move.l	d0,-(a7)	! dummy
	! pmove.l tt0,(a7)
	! 1111 000000 010 111 000 010 10 0000 0000
	.data4	0xf0170a00
	move.l	(a7)+,d0
	rts
_gettt1:
	move.l	d0,-(a7)	! dummy
	! pmove.l tt1,(a7)
	! 1111 000000 010 111 000 011 10 0000 0000
	.data4	0xf0170e00
	move.l	(a7)+,d0
	rts
_settt0fd:
	! pmovefd.l 4(a7),tt0
        ! 1111 000000 101 111 000 010 01 0000 0000
	.data4	0xf02f0900
	.data2	0x0004
	rts
_settt1fd:
	! pmovefd.l 4(a7),tt1
        ! 1111 000000 101 111 000 011 01 0000 0000
	.data4	0xf02f0d00
	.data2	0x0004
	rts

