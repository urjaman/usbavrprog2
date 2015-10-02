/* Define what our hardware is. */
#include "spihw_avrspi.h"


#define SPI_PORT	PORTB
#define SCK		PORTB1
#define MISO		PORTB3
#define MOSI 		PORTB2
#define SS		PORTB0
#define DDR_SPI		DDRB

#if 1
/* Open collector CS like in the shield. */
#define spi_select() do { DDR_SPI |=_BV(4); } while(0)
#define spi_deselect() do { DDR_SPI &= ~_BV(4); _delay_us(1); } while(0);
#else
/* Push-pull CS (normal-ish). */
#define spi_select() do { SPI_PORT &= ~(1<<SS); } while(0)
#define spi_deselect() do { SPI_PORT |= (1<<SS); } while(0)
#endif
