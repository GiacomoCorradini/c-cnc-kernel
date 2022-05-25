//   __  __            _     _
//  |  \/  | __ _  ___| |__ (_)_ __   ___
//  | |\/| |/ _` |/ __| '_ \| | '_ \ / _ \
//  | |  | | (_| | (__| | | | | | | |  __/
//  |_|  |_|\__,_|\___|_| |_|_|_| |_|\___|

#include "machine.h"
#include "inic.h"
#include <unistd.h>

//   ____            _                _   _
//  |  _ \  ___  ___| | __ _ _ __ ___| |_(_) ___  _ __
//  | | | |/ _ \/ __| |/ _` | '__/ _ \ __| |/ _ \| '_ \ 
//  | |_| |  __/ (__| | (_| | | |  __/ |_| | (_) | | | |
//  |____/ \___|\___|_|\__,_|_|  \___|\__|_|\___/|_| |_|

#define BUFLEN 1024

typedef struct machine {
  data_t A, tq, max_error, error; // tq = sampling time
  point_t *zero, *offset, *setpoint, *position;
  char broker_addres[BUFLEN];
  int broker_port;
  char pub_topic[BUFLEN];
  char sub_topic[BUFLEN];
  char pub_buffer[BUFLEN];
  struct mosquitto *mqt;
  struct mosquitto_message *msg;
  int connecting;
} machine_t;

static void on_connect(struct mosquitto *mqt, void *obj, int rc);
static void on_message(struct mosquitto *mqt, void *ud, const struct mosquitto_message *msg);

//   _____                 _   _
//  |  ___|   _ _ __   ___| |_(_) ___  _ __  ___
//  | |_ | | | | '_ \ / __| __| |/ _ \| '_ \/ __|
//  |  _|| |_| | | | | (__| |_| | (_) | | | \__ \
//  |_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|___/

// LIFECYCLE ======================================================

// Create a new instance reading data from an INI file
// If the INI file is not given (NULL), provide sensible default values
machine_t *machine_new(const char *ini_path) {
  machine_t *m = (machine_t *)calloc(1, sizeof(machine_t));
  if (!m) {
    perror("Error creating machine object");
    exit(EXIT_FAILURE);
  }
  if (ini_path) { // load value from INI file
    void *ini = ini_init(ini_path);
    data_t x, y, z;
    int rc = 0;

    if (!ini) {
      fprintf(stderr, "Could not open the ini file %s\n", ini_path);
      return NULL;
    }
    rc += ini_get_double(ini, "C-CNC", "A", &m->A);
    rc += ini_get_double(ini, "C-CNC", "max_error", &m->max_error);
    rc += ini_get_double(ini, "C-CNC", "tq", &m->tq);
    rc += ini_get_double(ini, "C-CNC", "origin_x", &x);
    rc += ini_get_double(ini, "C-CNC", "origin_y", &y);
    rc += ini_get_double(ini, "C-CNC", "origin_z", &z);
    m->zero = point_new();
    point_set_xyz(m->zero, x, y, z);
    rc += ini_get_double(ini, "C-CNC", "offset_x", &x);
    rc += ini_get_double(ini, "C-CNC", "offset_y", &y);
    rc += ini_get_double(ini, "C-CNC", "offset_z", &z);
    m->offset = point_new();
    point_set_xyz(m->offset, x, y, z);

    rc += ini_get_char(ini, "MQTT", "broker_addr", m->broker_addres, BUFLEN);
    rc += ini_get_int(ini, "MQTT", "broker_port", &m->broker_port);
    rc += ini_get_char(ini, "MQTT", "pub_topic", m->pub_topic, BUFLEN);
    rc += ini_get_char(ini, "MQTT", "sub_topic", m->sub_topic, BUFLEN); 


    ini_free(ini);
    if (rc > 0) {
      fprintf(stderr, "Missing /wrong %d config parameters\n", rc);
      return NULL;
    }

  } else { // provide some default value
    m->A = 125;
    m->max_error = 0.005;
    m->tq = 0.005;
    m->zero = point_new();
    point_set_xyz(m->zero, 0, 0, 0);
    m->offset = point_new();
    point_set_xyz(m->offset, 0, 0, 0);
  }
  m->setpoint = point_new();
  point_modal(m->zero, m->setpoint);
  m->position = point_new();
  m->error = m->max_error;
  m->mqt = NULL;
  if(mosquitto_lib_init() != MOSQ_ERR_SUCCESS) {
    perror("Could not initialize Mosquitto library");
    exit(EXIT_FAILURE);
  }
  m->connecting = 1;
  return m;
}

void machine_free(machine_t *m) {
  assert(m);
  point_free(m->zero);
  point_free(m->offset);
  point_free(m->setpoint);
  if(m->mqt) {
    mosquitto_destroy(m->mqt);
  }
  mosquitto_lib_cleanup();
  free(m);
  m = NULL;
}

// MQTT COMMUNICATIONS ============================================

// return value is 0 on succes
int machine_connect(machine_t *m, machine_on_message callback) {
  assert(m);
  m->mqt = mosquitto_new(NULL, 1, m);
  if(!m->mqt) {
    perror("Could not create MQTT");
    return 1;
  }
  mosquitto_connect_callback_set(m->mqt, on_connect);
  mosquitto_message_callback_set(m->mqt, callback ? callback : on_message);
  if(mosquitto_connect(m->mqt, m->broker_addres, m->broker_port, 60) != MOSQ_ERR_SUCCESS) {
    perror("Could not connect to broker");
    return 2;
  }

  while (m->connecting) {
    mosquitto_loop(m->mqt, -1, 1);
  }
  return 0;
}

int machine_sync(machine_t *m) {
  assert(m);
  if(mosquitto_loop(m->mqt, 0, 1) != MOSQ_ERR_SUCCESS) {
    perror("Mosquitto_loop error");
    return 1;
  }
  // fill up pub_buffer with current set point
  snprintf(m->pub_buffer, BUFLEN, "%f,%f,%f", point_x(m->setpoint), point_y(m->setpoint), point_z(m->setpoint));
  // send buffer over MQTTQ
  mosquitto_publish(m->mqt, NULL, m->pub_topic, strlen(m->pub_buffer), m->pub_buffer, 0, 0);
  return 0;
}

int machine_listen_start(machine_t *m) {
  if(mosquitto_subscribe(m->mqt, NULL, m->sub_topic, 0) != MOSQ_ERR_SUCCESS) {
    perror("Could not subscribe");
    return 1;
  }
  eprintf("Subscribe to topic %s\n", m->sub_topic);
  return 0;
}

int machine_listen_stop(machine_t *m) {
  if(mosquitto_unsubscribe(m->mqt, NULL, m->sub_topic) != MOSQ_ERR_SUCCESS) {
    perror("Could not unsubscribe");
    return 1;
  }
  eprintf("Unsubscribe to topic %s\n", m->sub_topic);
  return 0;
}

void machine_listen_update(machine_t *m) {
  if(mosquitto_loop(m->mqt, 0, 1) != MOSQ_ERR_SUCCESS) {
    perror("Mosquitto_loop error");
  }
}

void machine_disconnect(machine_t *m) {
  if (m->mqt) {
    while (mosquitto_want_write(m->mqt)) {
      mosquitto_loop(m->mqt, 0, 1);
      usleep(10000);
    }
    mosquitto_disconnect(m->mqt);
  }
}

// ACCESSORS ======================================================

// Metaprogramming

#define machine_getter(typ, par) \
typ machine_##par(const machine_t *m) { assert(m); return m->par; }

machine_getter(data_t, A);
machine_getter(data_t, tq);
machine_getter(data_t, max_error);
machine_getter(data_t, error);
machine_getter(point_t *, zero);
machine_getter(point_t *, offset);
machine_getter(point_t *, setpoint);
machine_getter(point_t *, position);


/*
data_t machine_A(machine_t *m) {
  assert(m);
  return m->A;
}

data_t machine_tq(machine_t *m) {
  assert(m);
  return m->tq;
}

point_t *machine_zero(machine_t *m) {
  assert(m);
  return m->zero;
}

point_t *machine_offset(machine_t *m) {
  assert(m);
  return m->offset;
}

data_t machine_error(machine_t *m) {
  assert(m);
  return m->error;
}
*/

static void on_connect(struct mosquitto *mqt, void *obj, int rc) {
  machine_t *m = (machine_t *)obj;
  if (rc == CONNACK_ACCEPTED) {
    eprintf("-> Connected to %s:%d\n", m->broker_addres, m->broker_port);
    // subscribe
    if(mosquitto_subscribe(mqt, NULL, m->sub_topic, 0) != MOSQ_ERR_SUCCESS) {
      perror("Could not subscribe");
      exit(EXIT_FAILURE);
    }
  } else { // failed to connect
    eprintf("-X Connection error: %s\n", mosquitto_connack_string(rc));
    exit(EXIT_FAILURE);
  }
  m->connecting = 0;
}

static void on_message(struct mosquitto *mqt, void *ud, const struct mosquitto_message *msg) {
  machine_t *m = (machine_t *)ud;
  // subtopic
  char *subtopic = strrchr(msg->topic, '/') + 1;
  
  printf("<- message: %s\n", (char *)msg->payload);
  mosquitto_message_copy(m->msg, msg);
  if(strcmp(subtopic, "error") == 0) {
    m->error = atof(msg->payload);
  } else if (strcmp(subtopic, "position") == 0) {
  // we have to parse a string like "123.4,100.0,-98" into three coordinate values x, y and z
  char *nxt = msg->payload;
  point_set_x(m->position, strtod(nxt, &nxt));
  point_set_y(m->position, strtod(nxt + 1, &nxt));
  point_set_z(m->position, strtod(nxt + 1, &nxt));
  } else {
    eprintf("Got unexpected message on %s\n", msg->topic);
  }
}