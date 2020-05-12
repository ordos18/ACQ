#include <asf.h>

#define UART_TXPIN IOPORT_CREATE_PIN(PORTE, 3)

int main (void) {
	
	unsigned int i = 0;
	
	static usart_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = 9600,
		.charlength = USART_CHSIZE_8BIT_gc,
		.paritytype = USART_PMODE_DISABLED_gc,
		.stopbits = false
	};
	
	stdio_serial_init(&USARTE0, &USART_SERIAL_OPTIONS);
	ioport_set_pin_dir(UART_TXPIN, IOPORT_DIR_OUTPUT);
	
	while(1) {
		/*
		char ch;
		scanf("%c",&ch);
		switch(ch) {
			case 'b':
				printf("binary\n\r",ch);
				break;
			case 'v':
				printf("voltage\n\r",ch);
				break;
			case 't':
				printf("temperature\n\r",ch);
				break;
			default:
				printf("???\n\r",ch);
				break;
		}
		*/
		printf("%d\n\r",i);
		i++;
	}
}
