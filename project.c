#include "contiki.h"
#include "lib/random.h"
#include "net/rime/rime.h"
#include "net/rime/collect.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "dev/light-sensor.h"
#include "net/netstack.h"
#include <stdio.h>

static struct collect_conn tc;
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
}
/*---------------------------------------------------------------------------*/
static const struct collect_callbacks callbacks = { recv };
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_collect_process, ev, data)
{
  static struct etimer periodic;
  static struct etimer et;
  
  PROCESS_BEGIN();
  
  //activate light sensor
  SENSORS_ACTIVATE(light_sensor);
  
  collect_open(&tc, 130, COLLECT_ROUTER, &callbacks);
  if(linkaddr_node_addr.u8[0] == 1 &&
     linkaddr_node_addr.u8[1] == 0) {
        printf("I am sink\n");
        collect_set_sink(&tc, 1);
  }
  /* Allow some time for the network to settle. */
  etimer_set(&et, 120 * CLOCK_SECOND);
  PROCESS_WAIT_UNTIL(etimer_expired(&et));
  while(1) {
    /* Send a packet every 60 seconds. */
    if(etimer_expired(&periodic)) {
      etimer_set(&periodic, CLOCK_SECOND * 60);
      etimer_set(&et, random_rand() % (CLOCK_SECOND * 60));
    }
    PROCESS_WAIT_EVENT();
    int value = light_sensor.value(0);
    if(value >= 10){ //SLEEP MODE
          
    if(etimer_expired(&et)) {
      static linkaddr_t oldparent;
      const linkaddr_t *parent;
      printf("Sending\n");
      packetbuf_clear();
      
        char sensor_value[3];
        sprintf(sensor_value, "%d", value);
        printf("Sensor value: %s\n", sensor_value);
        packetbuf_set_datalen(sprintf(packetbuf_dataptr(),
                                  "%s", sensor_value) + 1);
        
        
      collect_send(&tc, 15);
      parent = collect_parent(&tc);
      if(!linkaddr_cmp(parent, &oldparent)) {
        if(!linkaddr_cmp(&oldparent, &linkaddr_null)) {
          printf("#L %d 0\n", oldparent.u8[0]);
        }
        if(!linkaddr_cmp(parent, &linkaddr_null)) {
          printf("#L %d 1\n", parent->u8[0]);
        }
        linkaddr_copy(&oldparent, parent);
      }
    }
    }
  }
  PROCESS_END();
}

