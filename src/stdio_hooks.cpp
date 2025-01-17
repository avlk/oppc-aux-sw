#include <Arduino.h>
#include <stdio.h>

static int arduinostream_get(void *cookie, char *buffer, int size);
static int arduinostream_put(void *cookie, const char *buffer, int size);


void stdio_setup(void) {
    FILE *serial_monitor;

    serial_monitor = funopen(NULL, arduinostream_get, arduinostream_put, NULL, NULL);

    setlinebuf(serial_monitor);

    stdin = serial_monitor;
    stdout = serial_monitor;
}

static int arduinostream_get(void *cookie, char *buffer, int size) {
    (void)cookie;
    int number_of_available_bytes;
    
    while (!(number_of_available_bytes = Serial.available()))
        ;
    
    int i;
    for (i = 0; i < size && i < number_of_available_bytes; i++)
        buffer[i] = Serial.read();
    
    return i;
}

static int arduinostream_put(void *cookie, const char *buffer, int size) {
    (void)cookie;
    // we could just print the whole buffer at once,
    // but we want to insert CR with each LF
    int i;
    for (i = 0; i < size; i++)
        Serial.print(buffer[i]);
    return i;
}
