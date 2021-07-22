label 0x0000:
	mvi C, 0x00
	lxi H, 0x0070
	mov A, m
	inx H
	add m
	jnc _func
	jnc _func2
	inr C

_func:
	inx H
	mov m, A
	inx H
	mov m, C
	jnc _func

_func2:
	inx H
	mov m, A
	inx H
	mov m, C	

data 0x0070:
	05 08