/*

	16 bit expander ref: PCA9555PW

	doc : https://www.ti.com/lit/ds/symlink/pca9555.pdf


	the adress:
	(doc page 19 / table 8-2)

		the addr format is the following: 

		bit 7:4 (fixed)			[ 0  1  0  0 ]
		bit 3:1 (programmable)	[ A2 A1 A0 ]
		bit 0   (Read/Write)	[ R/W ]

		real layout
		0  1  0  0  A2 A1 A0 R/W

	



*/