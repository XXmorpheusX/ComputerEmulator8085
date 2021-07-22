label 0x0000:
	mvi C, 0x00
	lxi H, 0x0070
	mov A, m
	inx H
	add m
	inx H
	mov m, A
	hlt

data 0x0070:
	05 08