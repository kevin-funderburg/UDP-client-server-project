/***
 * TCP Client/Server Project
 * @file server.c
 * @authors: Kevin Funderburg
 */

#include "server.h"

/**
 * add a student to the database
 */
int add(int ID, char *Fname, char *Lname, int score) {
    FILE *fp;
    fp = fopen(datafile, "a");
    if (!fp) {
        printf("Could not open file %s", datafile);
        return 1;
    }
    getStudentData();
    if (!studentExists(ID)) {
        fprintf(fp, "%d,%s,%s,%d\n", ID, Fname, Lname, score);
        studentCount++;
        students[studentCount].id = ID;
        strcpy(students[studentCount].Fname, Fname);
        strcpy(students[studentCount].Lname, Lname);
        students[studentCount].score = score;
        fclose(fp);
    }
    return 0;
}

/**
 * delete student from database
 */
int delete(int ID)
{
    FILE *fp1, *fp2;
    fp1 = fopen(datafile, "r");
    if (!fp1) {
        printf("File not found or unable to open the input file!!\n");
        return 0;
    }
    fp2 = fopen("temp.csv", "w"); // open the temporary file in write mode
    if (!fp2) {
        printf("Unable to open a temporary file to write!!\n");
        fclose(fp1);
        return 0;
    }

    getStudentData();
    fprintf(fp2, "%s", "studentID,firstName,lastName,score\n");
    for (int i = 0; i < studentCount; ++i) {
        if (students[i].id != ID) {
            fprintf(fp2, "%d,%s,%s,%d\n", students[i].id, students[i].Fname, students[i].Lname, students[i].score);
        }
    }

    fclose(fp1);
    fclose(fp2);
    remove(datafile);  		// remove the original file
    rename("temp.csv", datafile); 	// rename the temporary file to original name
    studentCount--;
    return 0;
}

/**
 * output every student in database
 */
int display_all()
{
    printf("TRYING TO DISPLAY ALL\n");
    getStudentData();
    tableHeader();
    for (int i = 0; i < studentCount; ++i)
        showStudent(students[i]);
    strcat(svrMessage, "------------------------------------------------------\n\n");
    return 0;
}

/**
 * show every student who has a score >= score
 */
int display(int score)
{
    getStudentData();
    tableHeader();
    for (int i = 0; i < studentCount; ++i)
        if (students[i].score >= score)
            showStudent(students[i]);
    strcat(svrMessage, "------------------------------------------------------\n\n");
    return 0;
}

bool studentExists(int ID)
{
    getStudentData();
    for (int i = 0; i < studentCount; ++i)
        if (students[i].id == ID) return true;
    return false;
}

int displayStudent(int ID)
{
    getStudentData();
    tableHeader();
    for (int i = 0; i < studentCount; ++i)
        if (students[i].id == ID)
            showStudent(students[i]);
    strcat(svrMessage, "------------------------------------------------------\n\n");
    return 0;
}

/**
 * check student ID is valid
 */
bool idValid(char* id) { return (strlen(id) == 6); }

/**
 * parse the data from data.csv to create an array of students
 */
int getStudentData()
{
    printf("TRYING TO GET STUDENT DATA\n");
    FILE *fp = fopen("data.csv", "r");
    if (!fp) {
        printf("Can't open file\n");
        return 0;
    }

    char buf[MAXCHAR];
    studentCount = 0;
    int field_count = 0;
    int row_count = 0;
    while (fgets(buf, MAXCHAR, fp)) {
        field_count = 0;
        row_count++;

        if (row_count == 1) continue;
        studentCount++;

        char *field = strtok(buf, ",");
        while (field)
        {
            switch (field_count)
            {
                case 0:
                    students[row_count - 2].id = atoi(field);
                    break;
                case 1:
                    strcpy(students[row_count - 2].Fname, field);
                    break;
                case 2:
                    strcpy(students[row_count - 2].Lname, field);
                    break;
                case 3:
                    students[row_count - 2].score = atoi(field);
                    break;
                default:
                    printf("invalid field_count");
                    return -1;
            }
            field = strtok(NULL, ",");
            field_count++;
        }
    }
    fclose(fp);
    return 0;
}

/**
 * add a header for outputting the table
 */
void tableHeader()
{
    char str[MAXCHAR];
    snprintf(str, sizeof(str), "\n\n------------------------------------------------------\n"
                               "%-15s%-15s%-15s%-15s\n"
                               "------------------------------------------------------\n",
                               "Student ID", "First Name", "Last Name", "Grade");
    strcat(svrMessage, str);
}

/**
 * create a compatible string from student data
 * to be passed back to passed to the client
 * @param s student structure
 */
void showStudent(struct student s)
{
    char str[MAXCHAR];
    snprintf(str, sizeof(str), "%-15d%-15s%-15s%-5d\n",
             s.id, s.Fname, s.Lname, s.score);
    strcat(svrMessage, str);
}

/**
 * show usage listing
 */
void showusage()
{
    strcpy(svrMessage, "\nUSAGE\n"
                          "\t[adadssdeh] [a add] [da delete_all] [ds display_student] [s showscores] [d delete] [e exit] [h help]\n"
                          "\ta add\n"
                          "\t\tadds a student to the database with the following parameters:\n"
                          "\t\t[student ID] [First Name] [Last Name] [Grade]\n"
                          "\tda display_all\n"
                          "\t\tdisplay all student data in database\n"
                          "\tds display_student\n"
                          "\t\tdisplay a student from database with student ID = [student ID]\n"
                          "\ts showscores\n"
                          "\t\tshow all students with a grade higher than [grade]\n"
                          "\td delete\n"
                          "\t\tdelete a student from database with student ID = [student ID]\n"
                          "\te exit\n"
                          "\t\texit the application\n"
                          "\th help\n"
                          "\t\toutputs usage listing\n\n");
}


void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(){
  int udpSocket, nBytes;
  char buffer[MAXCHAR];
  struct sockaddr_in serverAddr, clientAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size, client_addr_size;
  bool exit = false;
  int i;

  /*Create UDP socket*/
  udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

  /*Configure settings in address struct*/
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(7891);
  // serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // local host
  serverAddr.sin_addr.s_addr = inet_addr("147.26.231.156"); // zues server
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  /*Bind socket with address struct*/
  bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  /*Initialize size variable to be used later on*/
  addr_size = sizeof serverStorage;

  while(1)
  {
    memset(svrMessage, 0, sizeof svrMessage);
    bzero(buffer, MAXCHAR);

    /* Try to receive any incoming UDP datagram. Address and port of
      requesting client will be stored on serverStorage variable */
    nBytes = recvfrom(udpSocket,buffer,1024,0,(struct sockaddr *)&serverStorage, &addr_size);

    printf("Here is the message: %s\n", buffer);

    char args[10][25];
    char *pch;
    // parse args
    pch = strtok(buffer, " ");
    strcpy(args[0], pch);
    int argc = 0;
    while (pch != NULL)
    {
        argc++;
        pch = strtok(NULL, " ,.-\n");
        if (pch != NULL)
            strcpy(args[argc], pch);
    }
    printf("parsed args:\n");
    for (int i = 0; i < argc; ++i)
        printf("args[%d]\t%s\n", i, args[i]);


    bool valid = false;
    if ((strncmp("add", args[0], 3) == 0) || (strncmp("a", args[0], 1) == 0))
    {
        if (argc < 5) {
            printf("expected 4 arguments and got %d\n", argc);
            snprintf(svrMessage, sizeof(svrMessage), "expected 4 arguments and got %d\n", argc - 1);
        } else if (!idValid(args[1])) {
            strcpy(svrMessage, "Invalid student ID, must be 6 digits long.\n");
        } else if (studentExists(atoi(args[1]))) {
            strcpy(svrMessage, "Student already in database.\n");
        } else
            valid = true;

        if (valid) {
            add(atoi(args[1]), args[2], args[3], atoi(args[4]));
            strcpy(svrMessage, "Student added successfully.\n");
        }
    }
    else if (strncmp("display_all", args[0], 11) == 0 || (strncmp("da", args[0], 2) == 0))
    {
        display_all();
        printf("\n%s", svrMessage);
    }
    else if (strncmp("display_student", args[0], 10) == 0 || (strncmp("ds", args[0], 2) == 0))
    {
        if (argc < 2) {
            printf("expected 2 arguments and got %d\n", argc);
            snprintf(svrMessage, sizeof(svrMessage), "expected 2 arguments and got %d\n", argc);
        } else if (!idValid(args[1])) {
            strcpy(svrMessage, "Invalid student ID, must be 6 digits long.\n");
        } else if (!studentExists(atoi(args[1]))) {
            strcpy(svrMessage, "Student not found.\n");
        } else
            valid = true;

        if (valid) {
            displayStudent(atoi(args[1]));
        }
    }
    else if (strncmp("showscores", args[0], 10) == 0 || (strncmp("s", args[0], 1) == 0))
    {
        if (argc < 2) {
            printf("expected 2 arguments and got %d\n", argc);
            snprintf(svrMessage, sizeof(svrMessage), "expected 2 arguments and got %d\n", argc);
        } else if (strlen(args[1]) == 0 || (strlen(args[1])) > 3) {
            strcpy(svrMessage, "Invalid score, must be 1 to 3 digits long.\n");
        } else
            valid = true;

        if (valid) {
            display(atoi(args[1]));
            printf("\n%s", svrMessage);
        }
    }
    else if (strncmp("delete", args[0], 6) == 0 || (strncmp("d", args[0], 1) == 0))
    {
        if (argc < 2) {
            printf("expected 2 arguments and got %d\n", argc);
            snprintf(svrMessage, sizeof(svrMessage), "expected 2 arguments and got %d\n", argc);
        } else if (!idValid(args[1])) {
            strcpy(svrMessage, "Invalid student ID, must be 6 digits long.\n");
        } else if (!studentExists(atoi(args[1]))) {
            strcpy(svrMessage, "Student not found.\n");
        } else
            valid = true;

        if (valid) {
            delete(atoi(args[1]));
            strcpy(svrMessage, "Student deleted successfully.\n");
        }
    }
    else if (strncmp("help", args[0], 6) == 0 || (strncmp("h", args[0], 1) == 0))
    {
        showusage();
    }
    else if (strncmp("exit", args[0], 4) == 0 || (strncmp("e", args[0], 1) == 0))
    {
        // if msg contains "Exit" then server exit and chat ended.
        printf("Server Exit...\n");
        strcpy(svrMessage, "exit");
        exit = true;
    }
    else {
        printf("%s is not a valid argument\n", args[0]);
        snprintf(svrMessage, sizeof(svrMessage), "%s is not a valid argument\n", args[0]);
    }

    // copy svrMessage to server buffer
    strcpy(buffer, svrMessage);
    nBytes = strlen(svrMessage);
    /*Send uppercase message back to client, using serverStorage as the address*/
    sendto(udpSocket,buffer,nBytes,0,(struct sockaddr *)&serverStorage,addr_size);

    if (exit) break;
  }

  return 0;
}
