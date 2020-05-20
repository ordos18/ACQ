#include <asf.h>

#define UART_TXPIN		IOPORT_CREATE_PIN(PORTE, 3)
#define MY_ADC			ADCA
#define MY_ADC_CH		ADC_CH0
#define MY_ADC_CH_RES	CH0RES
#define DMA_CHANNEL		0
#define SAMPLES_NR		5000

uint8_t Samples[SAMPLES_NR];

static void dma_init(void)
{
	struct dma_channel_config dmach_conf;
	memset(&dmach_conf, 0, sizeof(dmach_conf));
	
	dma_channel_set_burst_length(&dmach_conf, DMA_CH_BURSTLEN_1BYTE_gc);
	dma_channel_set_transfer_count(&dmach_conf, SAMPLES_NR);
	
	dma_channel_set_src_reload_mode(&dmach_conf, DMA_CH_SRCRELOAD_BURST_gc);
	dma_channel_set_dest_reload_mode(&dmach_conf, DMA_CH_DESTRELOAD_TRANSACTION_gc);
	
	dma_channel_set_src_dir_mode(&dmach_conf, DMA_CH_SRCDIR_INC_gc);
	dma_channel_set_dest_dir_mode(&dmach_conf, DMA_CH_DESTDIR_INC_gc);
	//adc_get_result(MY_ADC, MY_ADC_CH)
	//
	dma_channel_set_source_address(&dmach_conf, (uint16_t)(uintptr_t)&MY_ADC.MY_ADC_CH_RES);
	dma_channel_set_destination_address(&dmach_conf, (uint16_t)(uintptr_t)Samples);
	
	dma_channel_set_trigger_source(&dmach_conf, DMA_CH_TRIGSRC_ADCA_CH0_gc);
	dma_channel_set_single_shot(&dmach_conf);
	
	dma_enable();
	
	dma_channel_write_config(DMA_CHANNEL, &dmach_conf);
	dma_channel_enable(DMA_CHANNEL);
}

static void adc_init(void)
{
	struct adc_config adc_conf;
	struct adc_channel_config adcch_conf;
	
	adc_read_configuration(&MY_ADC, &adc_conf);
	adcch_read_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);
	
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_8, ADC_REF_AREFA);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_FREERUN_SWEEP, 1, 0);
	adc_set_clock_rate(&adc_conf, 1000000UL);
	adc_write_configuration(&MY_ADC, &adc_conf);
	
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN1, ADCCH_NEG_NONE, 1);
	adcch_write_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);
}

int main(void){
	
	// CLK
	sysclk_init();
	
	// UART
	ioport_set_pin_dir(UART_TXPIN, IOPORT_DIR_OUTPUT);
	static usart_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = 921600,
		.charlength = USART_CHSIZE_8BIT_gc,
		.paritytype = USART_PMODE_DISABLED_gc,
		.stopbits = false
	};
	usart_serial_init(&USARTE0, &USART_SERIAL_OPTIONS);
	
	// ADC
	adc_init();
	adc_enable(&MY_ADC);
	
	// DMA
	dma_init();
	
	//adc_start_conversion(&ADCA, ADC_CH0);
	
	// Main loop
	while(1) {
		uint8_t c;
		
		usart_serial_getchar(&USARTE0,&c);
		
		dma_channel_enable(DMA_CHANNEL);
		while (dma_channel_is_busy(DMA_CHANNEL));
		
		usart_serial_write_packet(&USARTE0,Samples, SAMPLES_NR);
	}
}
