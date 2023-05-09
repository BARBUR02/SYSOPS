#ifndef QUEUE_UTIL_H
#define QUEUE_UTIL_H

#include<string.h>
#include<stdio.h>
#include<stdbool.h>


char pop(char * q);
void push(char * q, char el);
bool isFull(char * q);
bool isEmpty(char * q);

#endif