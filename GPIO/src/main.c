#include <asf.h>

#define LED		IOPORT_CREATE_PIN(PORTD, 5)
#define BUTTON	IOPORT_CREATE_PIN(PORTE, 5)

int main (void)
{
	ioport_init();
	ioport_set_pin_dir(LED, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(BUTTON, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(BUTTON, IOPORT_MODE_PULLUP);
	
	while(1) {
		if(0 == ioport_get_pin_level(BUTTON)) {
			ioport_toggle_pin_level(LED);
			delay_ms(100);
		}
	}
}
