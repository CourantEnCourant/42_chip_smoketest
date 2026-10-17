/*
	DEF :

		pins :
			MISO	: Master In Slave Out
			MOSI	: MASTER Out Slave In
			SCK		: SPI Clock
			SS		: Slave Select

		as a Master you can control all the line exept MISO
		as a Slave you can control only MISO 
		

	init example page 172

			replace with correct pin value:

				DDR_SPI = DDRB

				DD_SS	= PB2
				DD_MOSI = PB3
				DD_MISO = PB4
				DD_SCK	= PB5

	// MASTER

	void SPI_MasterInit(void)
	{
		// Set MOSI and SCK output, all others input
		DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK);
		// Enable SPI, Master, set clock rate fck/16
		SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	}
	void SPI_MasterTransmit(char cData)
	{
		// Start transmission
		SPDR = cData;
		// Wait for transmission complete
		while(!(SPSR & (1<<SPIF)))
			;
	}

	// SLAVE

	void SPI_SlaveInit(void)
	{
		// Set MISO output, all others input
		DDR_SPI = (1<<DD_MISO);
		// Enable SPI
		SPCR = (1<<SPE);
	}

	char SPI_SlaveReceive(void)
	{
		// Wait for reception complete
		while(!(SPSR & (1<<SPIF)))
			;
		// Return Data Register
		return SPDR;
	}
*/