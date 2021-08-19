#ifndef ZABBIX_H
#define ZABBIX_H

#include <sys/socket.h>
#include <netinet/ip.h>

typedef struct zabbix_msg_st zabbix_msg_t;
struct zabbix_msg_st {
  struct sockaddr_in *zabbix_server;
  int max_size;

  char *data;
  int size;

  char is_item;
};

void zabbix_message_reinit(zabbix_msg_t *msg);

void zabbix_message_add_item(zabbix_msg_t *msg, const char *host, const char *item, const char *value);
void zabbix_message_add_item_t(zabbix_msg_t *msg, const char *host, const char *item, const char *value, int time);
void zabbix_message_add_item_f(zabbix_msg_t *msg, const char *host, const char *item, float value);
void zabbix_message_add_item_tf(zabbix_msg_t *msg, const char *host, const char *item, float value, int time);

int zabbix_message_finish(zabbix_msg_t *msg);
int zabbix_write(int fd, zabbix_msg_t *msg);

int zabbix_listen(int *sh);
int zabbix_send(zabbix_msg_t *msg);

#endif
