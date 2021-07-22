label 0x0000:
	mvi C, 0x00
	lxi H, 0x0070
	mov A, m
	inx H
	add m
	jnc _skip
	inr C

_skip:
	inx H
	mov m, A
	inx H
	mov m, C
	hlt

data 0x0070:
	FE FF