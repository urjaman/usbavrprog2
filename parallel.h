void parallel_init(void);
void parallel_safe(void);
uint8_t parallel_read(uint32_t addr);
void parallel_readn(uint32_t addr, uint32_t len);
void parallel_write(uint32_t addr, uint8_t data);
