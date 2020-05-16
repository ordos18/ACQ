#include <asf.h>

#define UART_TXPIN	IOPORT_CREATE_PIN(PORTE, 3)
#define MY_ADC		ADCA
#define MY_ADC_CH	ADC_CH0

#define GAIN					64
#define SAMPLES_PER_MEASUREMENT	2048
#define REF_VOLTAGE_mV			1000
#define TOP_VALUE				2048
#define OFFSET_mg				0.418
#define SCALE_mg				(5.346-OFFSET_mg)

static void adc_init(void)
{
	struct adc_config adc_conf;
	struct adc_channel_config adcch_conf;
	
	adc_read_configuration(&MY_ADC, &adc_conf);
	adcch_read_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);
	
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12, ADC_REF_BANDGAP);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_set_clock_rate(&adc_conf, 500000UL);
	
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
		float adc_mV, adc_mg, adc_LSB, adc = 0;
		uint16_t iter;
		char out_str[30] = {0};
		
		scanf("%c",&ch);
		for(iter = 0; iter < SAMPLES_PER_MEASUREMENT; iter++) {
			adc_start_conversion(&MY_ADC, MY_ADC_CH);
			adc_wait_for_interrupt_flag(&MY_ADC, MY_ADC_CH);
			adc = adc + adc_get_signed_result(&MY_ADC, MY_ADC_CH);
		}
		adc_LSB = adc/SAMPLES_PER_MEASUREMENT;
		adc_mV = adc_LSB * REF_VOLTAGE_mV / GAIN / TOP_VALUE;
		adc_mg = 1000*(adc_mV-OFFSET_mg)/SCALE_mg;
		sprintf(out_str, "%f\r", adc_mg);
		printf("%s", out_str);
	}
}
