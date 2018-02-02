#include "contiki.h"
#include "lib/random.h"
#include "net/rime/rime.h"
#include "net/rime/collect.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "dev/light-sensor.h"
#include "net/netstack.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static struct collect_conn tc;
char value[3];
/*---------------------------------------------------------------------------*/
PROCESS(example_collect_process, "Test collect process");
AUTOSTART_PROCESSES(&example_collect_process);
/*---------------------------------------------------------------------------*/
static void
recv(const linkaddr_t *originator, uint8_t seqno, uint8_t hops)
{
  printf("Sink got message from %d.%d, seqno %d, hops %d: len %d '%s'\n",
         originator->u8[0], originator->u8[1],
         seqno, hops,
         packetbuf_datalen(),
         (char *)packetbuf_dataptr());
         
         strcpy(value, (char *)packetbuf_dataptr());
         int value_int = atoi(value);
         if(value_int > 50 && value_int <= 150){
            printf("ACTION2: Sending less water to mote %d area\n",  originator->u8[0]);
         }
		 if(value_int > 150 && value_int < 200){
            printf("ACTION3: Sending more water to mote %d area\n",  originator->u8[0]);
         }
		 if(value_int >= 200){
            printf("ACTION4: WARNING - very high temperature in mote %d area\n",  originator->u8[0]);
         }
         
}
/*---------------------------------------------------------------------------*/
static const struct collect_callbacks callbacks = { recv };
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_collect_process, ev, data)
{
  
  PROCESS_BEGIN();
  
  collect_open(&tc, 130, COLLECT_ROUTER, &callbacks);
  if(linkaddr_node_addr.u8[0] == 1 &&
     linkaddr_node_addr.u8[1] == 0) {
        printf("I am sink\n");
        collect_set_sink(&tc, 1);
        PROCESS_WAIT_UNTIL(1);
  }
  PROCESS_END();
}
