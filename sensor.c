/** SENSOR SENDS DATA TO THE PC_CLIENT **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "rest.h"

#if defined (PLATFORM_HAS_LEDS)
#include "dev/leds.h"
#endif

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF(" %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x ", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF(" %02x:%02x:%02x:%02x:%02x:%02x ",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

char temp[100];

RESOURCE(printer, METHOD_GET, "printer");
void
printer_handler(REQUEST* request, RESPONSE* response)
{
	sprintf(temp, "First printing");
	rest_set_header_content_type(response, TEXT_PLAIN);
	rest_set_response_payload(response, (uint8_t*)temp, strlen(temp));
}

RESOURCE(discover, METHOD_GET, ".well-known/core");
void
discover_handler(REQUEST* request, RESPONSE* response)
{
  char temp[100];
  int index = 0;
  index += sprintf(temp + index, "%s,", "</helloworld>;n=\"HelloWorld\"");
#if defined (PLATFORM_HAS_LEDS)
  index += sprintf(temp + index, "%s,", "</led>;n=\"LedControl\"");
#endif

  rest_set_header_content_type(response, APPLICATION_LINK_FORMAT);
  rest_set_response_payload(response, (uint8_t*)temp, strlen(temp));
}

RESOURCE(value, METHOD_GET, "value");
void
value_handler(REQUEST* request, RESPONSE* response)
{
	printf("Entering in value_handler...\n");
	// sensor sends some stored values
	static int cont;
	int dim = 4;
	int buf[13];
	int data[dim];
	data[0] = 2;
	data[1] = 4;
	data[2] = 6;
	data[3] = 8;

	printf("Resource: value\n");
	while (cont < dim) {
		printf("Array values: %d\n", data[cont]);
		sprintf(buf, "Temp values: %d", data[cont]);
		cont++;
		rest_set_response_payload(response, (uint8_t*)buf, strlen(buf));
	}
	cont = 0;
}

PROCESS(coap_sensor, "Starting Sensor....");
AUTOSTART_PROCESSES(&coap_sensor);

PROCESS_THREAD(coap_sensor, ev, data)
{
	PROCESS_BEGIN();
	PRINTF("COAP Sensor\n");
	rest_init();

	rest_activate_resource(&resource_printer);
  	rest_activate_resource(&resource_discover);
  	rest_activate_resource(&resource_value);

  	PROCESS_END();
}



