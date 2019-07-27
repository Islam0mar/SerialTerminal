#define _POSIX_C_SOURCE 200809L

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cs50.h>
#include <libserialport.h>  // cross platform serial port lib
#include <readline/history.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <time.h>  // for sleep function

#include "que.h"

#define BUADRATE (int)115200

struct sp_port *port;
char byte_buff[512] = "\0";

int list_ports(Queue *pq) {
  int i = 0;
  struct sp_port **ports;
  enum sp_return error = sp_list_ports(&ports);
  if (error == SP_OK) {
    QueueEntry portName;
    char *x;
    for (i = 0; ports[i]; i++) {
      x = sp_get_port_name(ports[i]);
      portName.string = malloc(strlen(x) + 1);
      snprintf(portName.string, strlen(x) + 1, "%s", x);
      AddToTail(portName, pq);
    }
    sp_free_port_list(ports);
  } else {
    printf("No serial devices detected\n");
  }
  return i;
}

void parse_serial(char *byte_buff, int byte_num) {
  for (int i = 0; i < byte_num; i++) {
    printf("%c", byte_buff[i]);
  }
  // printf("\n");
}

const char *words[] = {"EMIT",  "DUP", "DROP", "C2FLASH",
                       "C2RAM", "AND", "OR",   "XOR"};

// Generator function for word completion.
char *my_generator(const char *text, int state) {
  static int list_index, len;
  const char *name;

  if (!state) {
    list_index = 0;
    len = strlen(text);
  }

  while ((name = words[list_index])) {
    list_index++;
    if (strncmp(name, text, len) == 0) return strdup(name);
  }

  // If no names matched, then return NULL.
  return ((char *)NULL);
}

// Custom completion function
static char **my_completion(const char *text, int start, int end) {
  rl_completer_quote_characters = "\"'";
  rl_attempted_completion_over = 1;

  char **matches = (char **)NULL;
  matches = rl_completion_matches((char *)text, &my_generator);
  return matches;
}

int main() {
  Queue avialablePorts;
  CreateQueue(&avialablePorts);
  int x = list_ports(&avialablePorts);
  QueueNode *ptrs = avialablePorts.head;
  for (x = 1; ptrs; ptrs = ptrs->next) {
    printf("%d: %s\n", x, ptrs->entry.string);
    x++;
  }

  printf("Enter the port number to connect: ");
  int i = get_int("Int: ");
  if (i >= x || i < 1 || x < 2) return 1;
  x = i;
  i = 1;
  ptrs = avialablePorts.head;
  for (; ptrs; ptrs = ptrs->next) {
    if (i == x) {
      printf("Opening port '%s' \n", ptrs->entry.string);
      break;
    }
    i++;
  }

  enum sp_return error = sp_get_port_by_name(ptrs->entry.string, &port);

  ClearQueue(&avialablePorts);

  if (error == SP_OK) {
    sp_set_baudrate(port, BUADRATE);
    error = sp_open(port, SP_MODE_READ_WRITE);
    sp_set_baudrate(port, BUADRATE);
    char *s;
    // enable auto-complete
    rl_bind_key('\t', rl_complete);
    rl_attempted_completion_function = my_completion;

    while (true) {
      if (error == SP_OK) {
        s = readline(">> ");

        if (!strcmp(s, "bye")) {
          sp_close(port);
          free(s);
          return 0;
        }

        char *x = strstr(s, "#include");
        if (x != NULL) {
          x += 8;
          while (x[0] == (int)' ') x++;
          FILE *file = fopen(x, "r");
          char line[256];
          if (file) {
            add_history(s);
            while (fgets(line, sizeof(line), file)) {
              sp_nonblocking_write(port, line, strlen(line));
              sp_drain(port);
              long milliseconds = 80;
              struct timespec ts;
              ts.tv_sec = milliseconds / 1000;
              ts.tv_nsec = (milliseconds % 1000) * 1000000;
              nanosleep(&ts, NULL);
            }
            fclose(file);
          }
          continue;
        }

        if (s[0] != 0) add_history(s);

        sp_nonblocking_write(port, s, strlen(s));
        i = sp_nonblocking_write(port, " ", 1);
        if (i < 0) {
          printf("Error writing serial device\n");
          break;
        }
        sp_drain(port);
        while (sp_output_waiting(port) > 0) {
          ;
        }
        long milliseconds = 180;
        struct timespec ts;
        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;
        nanosleep(&ts, NULL);
        int bytes_waiting = sp_input_waiting(port);
        if (i < 0) {
          printf("System Error\n");
          break;
        }
        if (bytes_waiting > 0) {
          int byte_num = 0;
          byte_num = sp_nonblocking_read(port, byte_buff, 512);
          if (byte_num < 0) {
            printf("Error reading serial device\n");
            break;
          }
          parse_serial(byte_buff, byte_num);
          printf(" OK\n");
        }
        fflush(stdout);
      } else {
        printf("Error opening serial device\n");
        break;
      }
    }
    free(s);
    sp_close(port);
  } else {
    printf("Error serial device not found\n");
  }
  return 0;
}

