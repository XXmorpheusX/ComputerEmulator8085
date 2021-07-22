label 0x0000:
	lxi B, 0x0048
	lxi D, 0x0068
	mvi L, 0x0A

_back:
	ldax B
	stax D
	inx B
	inx D
	dcr L
	jnz _back
	hlt

data 0x0048:
	01 02 03 04 05 06 07 08 09 0A