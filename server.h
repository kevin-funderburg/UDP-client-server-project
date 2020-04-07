//
// Created by Kevin Funderburg on 2/11/20.
//

#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAXCHAR 1024

struct student
{
    int id, score;
    char Fname[25], Lname[25];
};

struct student students[100];
char* datafile = "data.csv";
int studentCount;
char svrMessage[MAXCHAR];    //message sent to client

int add(int, char*, char*, int);
int delete(int);
int display_all();
int display(int);
int getStudentData();
bool studentExists(int);
bool idValid(char*);
void tableHeader();
void showStudent(struct student);
void error(char*);
void showusage();

#endif //SERVER_H
