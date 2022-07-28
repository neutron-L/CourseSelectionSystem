#ifndef SYS_H_
#define SYS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

struct Course
{
    char *code;
    char *name;
    char *lecturer;
    int capacity;
    int selected_number;
    struct Course *next;
};

typedef struct Course Course;

// Courses selected by students or published by teachers
struct Info
{
    char *code; // course number or student account
    char *name; // course name or student name
    struct Info *next;
};

typedef struct Info Info;

// Student
struct Student
{
    char *name;
    char *password;
    Info *selected_course;
    struct Student *next;
};

typedef struct Student Student;

struct Teacher
{
    char *name;
    char *password;
    Info *published_course;
    struct Teacher *next;
};

typedef struct Teacher Teacher;

struct Administrator
{
    char *name;
    char *password;
    struct Administrator *next;
};
typedef struct Administrator Administrator;

// select course or publish course status
enum STATUS
{
    SUCCESS,
    ALREADY_EXIST,
    ALREADY_DONE,
    NON_EXISTENT,
    FAILURE
};

enum ID
{
    STUDENT,
    TEACHER
};

/* Student operation */
void select_course();         // Course selection
void query_course_selected(); // Query course selection
void drop_out_course();       // Drop out course

/* Teacher operation */
void publish_course();         // Course selection
void query_course_published(); // Query course selection

/* Administrators */
void logon(int);           // Student / teacher registration
void view_students_info(); // View registered students
void view_teacher_info();  // View registered teachers
void view_course_info();   // View published courses

void home(int); // home page
// login in
int login();
void init();
void log_out();

#endif