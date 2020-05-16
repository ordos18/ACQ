#include <asf.h>

#define UART_TXPIN	IOPORT_CREATE_PIN(PORTE, 3)
#define MY_ADC		ADCA
#define MY_ADC_CH	ADC_CH0

#define GAIN					4
#define SAMPLES_PER_MEASUREMENT	16
#define REF_VOLTAGE_mV			1000
#define TOP_VALUE				2048

static void adc_init(void)
{
	struct adc_config adc_conf;
	struct adc_channel_config adcch_conf;
	
	adc_read_configuration(&MY_ADC, &adc_conf);
	adcch_read_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);
	
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12, ADC_REF_BANDGAP);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_set_clock_rate(&adc_conf, 200000UL);
	
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN1, ADCCH_NEG_PIN5, GAIN);
	adc_write_configuration(&MY_ADC, &adc_conf);
	adcch_write_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);
}

int main (void) {
	
	
	static usart_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = 9600,
		.charlength = USART_CHSIZE_8BIT_gc,
		.paritytype = USART_PMODE_DISABLED_gc,
		.stopbits = false
	};
	
	sysclk_init();
	adc_init();
	adc_enable(&MY_ADC);
	
	stdio_serial_init(&USARTE0, &USART_SERIAL_OPTIONS);
	ioport_set_pin_dir(UART_TXPIN, IOPORT_DIR_OUTPUT);
	
	while(1) {
		char ch;
		float adc_mV, adc_LSB, adc = 0;
		uint8_t iter;
		char out_str[30] = {0};
		
		scanf("%c",&ch);
		for(iter = 0; iter < SAMPLES_PER_MEASUREMENT; iter++) {
			adc_start_conversion(&MY_ADC, MY_ADC_CH);
			adc_wait_for_interrupt_flag(&MY_ADC, MY_ADC_CH);
			adc = adc + adc_get_signed_result(&MY_ADC, MY_ADC_CH);
		}
		adc_LSB = adc/SAMPLES_PER_MEASUREMENT;
		adc_mV = adc_LSB * REF_VOLTAGE_mV / GAIN / TOP_VALUE;
		sprintf(out_str, "%.3f\r", adc_mV);
		printf("%s", out_str);
	}
}
