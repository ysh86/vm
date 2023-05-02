	.sect	.text

! ACK assembler a.out
!
! head
! ushort magic = 0x0201
! ushort stamp = 0 (version)
! ushort flags
! ushort nsect = 1
! ushort nrelo = 0
! ushort nname
! long   nemit = code size
! long   nchar
! ---------------------------
! 20 [bytes]
!
! sect
! long base
! long size
! long foff
! long flen
! long lign
! ---------------------------
! 20 [bytes]
.space	0x400-40

	.define _start
_start:
	lea	global,a0
	move.l	(sp)+,d0	! ushm base
	move.l	d0,(a0)+
	move.l	(sp)+,d0	! kshm base
	move.l	d0,(a0)+
	move.l	(sp)+,d0	! shm size
	move.l	d0,(a0)
	lea	ushm_msg,a0	! message on ushm
	move.l	ushm_base,(a0)
	lea	kshm_msg,a0	! message on kshm
	move.l	kshm_base,(a0)
	jsr	_umain
	jsr	_exit
loop:	bra	loop

! ---------------------------
! main
! ---------------------------
_umain:
	move.w	#252,d0		! status
	rts

! ---------------------------
! syscall
!
! src/lib/mac/crt/sndrec.s
! SEND    = 1
! RECEIVE = 2
! BOTH    = 3
!
! include/lib.h
! #define MM     0
! #define FS     1
!
! include/minix/callnr.h
! #define EXIT   1
! #define READ   3
! #define WRITE  4
! #define OPEN   5
! #define CLOSE  6
!
! src/lib/other/message.c
! message _M = {0};
!
! ---------------------------
_exit:
	move.l	ushm_msg,a0
	move.w	#0,m_source(a0)	! dummy
	move.w	#1,m_type(a0)	! EXIT
	move.w	d0,m1i1(a0)	! d0:status
	move.w	#0,m1i2(a0)	! dummy
	move.w	#0,m1i3(a0)	! dummy
	move.l	#0,m1p1(a0)	! dummy
	move.l	#0,m1p2(a0)	! dummy
	move.l	#0,m1p3(a0)	! dummy
	move.w	#3,d0		! sendrec
	move.w	#0,d1		! to MM
	move.l	kshm_msg,a0	! message on kshm
	trap	#0
	rts

global:
ushm_base:	.data4	0
kshm_base:	.data4	0
shm_size:	.data4	0

! data on shm
ushm_msg:	.data4	0
kshm_msg:	.data4	0

! include/minix/type.h
! typedef struct {...} message;
!
! offsets
m_source	= 0
m_type		= 2
! message1
m1i1		= 4
m1i2		= 6
m1i3		= 8
m1p1		= 10
m1p2		= 14
m1p3		= 18
