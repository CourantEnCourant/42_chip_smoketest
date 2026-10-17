#include "main.h"

#include "pin.h"

int main() {
	set_bit(LED_DDR, D1);
	set_bit(LED_PORT, D1);
	while (1);
}
