	.sect	.text

	.extern	_Gestalt
_Gestalt:
	link	a6,#0
	movem.l	d2/a5,-(sp)
	tst.l	savedsp
	beq	LL0
	move.l	a7,a0
	move.l	savedsp,a7
	move.l	a0,-(sp)
LL0:
	move.l	0x8(a6),d0
	move.l	saveda5,a5
	.data2	0xa1ad
	move.l	0xc(a6),a1
	move.l	a0,(a1)
	tst.l	savedsp
	beq	LL1
	move.l	(sp)+,a7
LL1:
	movem.l	(sp)+,d2/a5
	unlk	a6
	rts

