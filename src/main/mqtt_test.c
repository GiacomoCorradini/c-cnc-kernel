//   __  __  ___ _____ _____
//  |  \/  |/ _ \_   _|_   _|
//  | |\/| | | | || |   | |
//  | |  | | |_| || |   | |
//  |_|  |_|\__\_\|_|   |_|
// Simple MQTT client example
#include "../inic.h"
// needed fot mqtt
#include <mosquitto.h>
#include <mqtt_protocol.h>
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

//   ____            _                 _   _                 
//  |  _ \  ___  ___| | __ _ _ __ __ _| |_(_) ___  _ __  ___ 
//  | | | |/ _ \/ __| |/ _` | '__/ _` | __| |/ _ \| '_ \/ __|
//  | |_| |  __/ (__| | (_| | | | (_| | |_| | (_) | | | \__ \
//  |____/ \___|\___|_|\__,_|_|  \__,_|\__|_|\___/|_| |_|___/
// 
// Preprocessor macros and constants
#define eprintf(...) fprintf(stderr, __VA_ARGS__)
#define BUFLEN 1024
#define INI_FILE "settings.ini"
#define INI_SECTION "MQTT"

// Custom types
typedef struct {
  char broker_addr[BUFLEN];
  int broker_port;
  char topic[BUFLEN];
  struct mosquitto_message *msg;
  int running;
  int ready_to_exit;
} userdata_t;

// Functions
void on_connect(struct mosquitto *mqt, void *ud, int rc);
void on_disconnect(struct mosquitto *mqt, void *ud, int rc);
void on_subscribe(struct mosquitto *mqt, void *ud, int mid, int qos_len, const int *qos);
void on_unsubscribe(struct mosquitto *mqt, void *ud, int rc);
void on_message(struct mosquitto *mqt, void *ud, const struct mosquitto_message *msg);

// Global variable for controlling the main loop
static int _running = 1;
static void sig_handler(int signal) {
  switch (signal)
  {
  case SIGINT: // signal send each time that ctrl + c are pressed
    _running = 0;
    break;
  default:
    break;
  }
}

//                   _
//   _ __ ___   __ _(_)_ __
//  | '_ ` _ \ / _` | | '_ \
//  | | | | | | (_| | | | | |
//  |_| |_| |_|\__,_|_|_| |_|
//
int main(int argc, char const *argv[]) {
  void *ini = NULL;
  struct mosquitto *mqt;
  int delay = 0;
  userdata_t ud = {
    .broker_addr = "localhost", 
    .broker_port = 1883,
    .topic = "ccnc/test",
    .running = 1,
    .ready_to_exit = 0
    };


    signal(SIGINT, sig_handler);

    //INI file
    ini = ini_init(INI_FILE);
    if(!ini) {
      eprintf("Error opening INI file\n");
      return 1;
    }

    ini_get_char(ini, INI_SECTION, "broker_addr", ud.broker_addr, BUFLEN);
    ini_get_int(ini, INI_SECTION, "broker_port", &ud.broker_port);
    ini_get_char(ini, INI_SECTION, "topic", ud.topic, BUFLEN);
    ini_get_int(ini, INI_SECTION, "delay", &delay);

    // MQTT
    // Initialize library
    if(mosquitto_lib_init() != MOSQ_ERR_SUCCESS) {
      perror("Could not initialize MQTT library");
      return 2;
    }

    // library version
    {
      int major, minor, revision;
      mosquitto_lib_version(&major, &minor, &revision);
      eprintf("Library version: %d.%d.%d\n", major, minor, revision);
    }

    // creating MQTT object
    mqt = mosquitto_new(NULL, 1, &ud);
    if(!mqt) {
      perror("Could not create MQTT object");
      return 3;
    }

    // CallBacks
    mosquitto_connect_callback_set(mqt, on_connect);
    mosquitto_disconnect_callback_set(mqt, on_disconnect);
    mosquitto_subscribe_callback_set(mqt, on_subscribe);
    mosquitto_unsubscribe_callback_set(mqt, on_unsubscribe);
    mosquitto_message_callback_set(mqt, on_message);

    // Connections to the broker
    if(mosquitto_connect(mqt, ud.broker_addr, ud.broker_port, 60) != MOSQ_ERR_SUCCESS) {
      perror("Erroc connecting to the broker");
      return 4;
    }

    // main loop
    while (ud.running && _running) {
      if(mosquitto_loop(mqt, -1, 1) != MOSQ_ERR_SUCCESS) {
        perror("mosquitto_loop error");
        break;
      }
      usleep(delay * 1000); // good idea to avoid to use 100% of the CPU
    }
    
    // unsubscribe
    mosquitto_unsubscribe(mqt, NULL, ud.topic);
    // wait for unsubsciption to happen
    while (ud.ready_to_exit == 0) {
      mosquitto_loop(mqt, -1, 1);
      usleep(delay * 1000);
    }

    mosquitto_disconnect(mqt);
    mosquitto_destroy(mqt);
    mosquitto_lib_cleanup();

  return 0;
}

//   ____        __ _       _ _   _                 
//  |  _ \  ___ / _(_)_ __ (_) |_(_) ___  _ __  ___ 
//  | | | |/ _ \ |_| | '_ \| | __| |/ _ \| '_ \/ __|
//  | |_| |  __/  _| | | | | | |_| | (_) | | | \__ \
//  |____/ \___|_| |_|_| |_|_|\__|_|\___/|_| |_|___/
// 

void on_connect(struct mosquitto *mqt, void *obj, int rc) {
  userdata_t *ud = (userdata_t *)obj;
  if (rc == CONNACK_ACCEPTED) {
    eprintf("-> Connected to %s:%d\n", ud->broker_addr, ud->broker_port);
    // subscribe
    if(mosquitto_subscribe(mqt, NULL, ud->topic, 0) != MOSQ_ERR_SUCCESS) {
      perror("Could not subscribe");
      exit(EXIT_FAILURE);
    }
  } else {
    eprintf("-X Connection error: %s\n", mosquitto_connack_string(rc));
    exit(EXIT_FAILURE);

  }
}
void on_disconnect(struct mosquitto *mqt, void *ud, int rc) {
  eprintf("<- Disconnected\n");
}
void on_subscribe(struct mosquitto *mqt, void *ud, int mid, int qos_len, const int *qos) {
  eprintf("-> Subscribed to the topic %s\n", ((userdata_t *)ud)->topic);
}
void on_unsubscribe(struct mosquitto *mqt, void *ud, int rc) {
  eprintf("<- Unsubscribe from %s\n", ((userdata_t *)ud)->topic);
  ((userdata_t *)ud)->ready_to_exit = 1;
}
void on_message(struct mosquitto *mqt, void *ud, const struct mosquitto_message *msg) {
  printf("<- message: %s\n", (char *)msg->payload);
  if(strcmp((char *)msg->payload, "stop") == 0) {
    ((userdata_t *)ud)->running = 0;
  }
  mosquitto_message_copy(((userdata_t *)ud)->msg, msg);
}