/* Define what our hardware is. */


#define SPI_PORT	PORTD
#define SCK		PORTD5
#define MISO		PORTD2
#define MOSI 		PORTD3
#define SS		PORTD4
#define SPI_EN		PORTD0
#define SPI_XBUF	PORTD1
#define SPI_WP		PORTD6
#define SPI_HOLD	PORTD7

#define DDR_SPI		DDRD

/* Push-pull CS. */
#define spi_select() do { SPI_PORT &= ~(1<<SS); } while(0)
#define spi_deselect() do { SPI_PORT |= (1<<SS); } while(0)

void spi_init();

void spi_enable();
void spi_disable();
