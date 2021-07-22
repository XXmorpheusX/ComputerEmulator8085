label 0x0000:
	mvi C, 0x00
	lda 0x0070
	mov B, A
	lda 0x0071
	add B
	jnc _skip
	inr C

_skip:
	sta 0x0072
	mov A, C
	sta 0x0073
	hlt

data 0x0070:
	FF FF