#ifndef UTILS_H
#define UTILS_H

#include<stdio.h>
#include<stdlib.h>

#define HOME_P getenv("HOME")
// EXECS

#define BARBER_EXEC "./barber"
#define CUSTOMER_EXEC "./customer"

#define QUEUE_SIZE 2
#define BARBER_COUNT 3
#define CHAIR_COUNT 2
#define CUSTOMER_COUNT 7

#define BUFFER_SIZE 4096

#define SEM_QUEUE "1"
#define SEM_BARBER "2"
#define SEM_CHAIRS "3"
#define MUTEX "4"


#endif