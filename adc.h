#pragma once

void adc_init(void);

/* Result is in mV. */
uint16_t measure_vspi(void);
uint16_t measure_vcc(void);
