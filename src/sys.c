#include "sys.h"

/* Save information about courses, students and teachers */
Student *students = NULL;
Teacher *teachers;
Course *courses;
Administrator *administrators;
Student *pStudent; // After login, the pointer points to the login student
Teacher *pTeacher; // After login, the pointer points to the login teacher
Administrator *pAdministrator;

/* static function */
static void init_teacher();
static void init_student();
static enum STATUS register_student(char *name, char *passwd);
static enum STATUS register_teacher(char *name, char *passwd);
static enum STATUS publish_course1(char *code, char *name, int capacity);
static enum STATUS select_course1(char *code);
static enum STATUS drop_out_course1(char *code);

static void free_info(Info *);

static void init_teacher()
{
    long start, end;
    char name[20];
    char password[20];
    char line[100];
    char info[100];

    char *course_code;
    char *course_name;
    int capacity;

    // teacher
    FILE *ftea = fopen("/home/rda/Downloads/StudentCourseSelectionSystem/src/teachers.txt", "r");
    assert(ftea);
    fseek(ftea, 0L, SEEK_END);
    end = ftell(ftea);
    fseek(ftea, 0L, SEEK_SET);
    start = ftell(ftea);
    while (start != end)
    {
        memset(name, 0, sizeof(name));
        memset(password, 0, sizeof(password));
        memset(line, 0, sizeof(line));
        memset(info, 0, sizeof(info));
        fgets(line, 100, ftea);
        sscanf(line, "%s %s %s\n", name, password, info);
        printf("%s %s %s\n", name, password, info);
        // register teacher
        register_teacher(name, password);
        if (info[0] != '\0')
        {
            char *pi = info;
            char *ti;

            while (*pi)
            {
                ti = strchr(pi, ';');
                *ti = '\0';

                char *qi = strchr(pi, ',');
                course_code = pi;
                *qi = '\0';
                qi++;
                course_name = qi;
                qi = strchr(qi, ',');
                *qi = '\0';
                capacity = atoi(qi + 1);
                publish_course1(course_code, course_name, capacity);
                pi = ti + 1;
            }
        }
        start = ftell(ftea);
    }
    fclose(ftea);
}

static void init_student()
{
    long start, end;
    char name[20];
    char password[20];
    char line[100];
    char info[100];

    // student
    FILE *fstu = fopen("/home/rda/Downloads/StudentCourseSelectionSystem/src/students.txt", "r");
    assert(fstu);

    fseek(fstu, 0L, SEEK_END);
    end = ftell(fstu);
    fseek(fstu, 0L, SEEK_SET);
    start = ftell(fstu);
    while (start != end)
    {
        memset(name, 0, sizeof(name));
        memset(password, 0, sizeof(password));
        memset(line, 0, sizeof(line));
        memset(info, 0, sizeof(info));
        fgets(line, 100, fstu);
        sscanf(line, "%s %s %s\n", name, password, info);
        printf("%s %s %s\n", name, password, info);
        // register student
        register_student(name, password);
        if (info[0] != '\0')
        {
            char course_code[20];
            char *pi = info;
            char *ti;
            while (*pi)
            {
                memset(course_code, 0, 20);
                ti = strchr(pi, ';');
                *ti = '\0';
                select_course1(pi);
                pi = ti + 1;
            }
        }
        start = ftell(fstu);
    }
    fclose(fstu);
}

static enum STATUS register_student(char *name, char *passwd)
{
    Student *p = students;
    Student *q = NULL;
    while (p && strcmp(p->name, name) < 0)
    {
        q = p;
        p = p->next;
    }
    if (!p || strcmp(p->name, name) > 0)
    {
        Student *student = (Student *)malloc(sizeof(Student));
        student->name = (char *)malloc(strlen(name) + 1);
        student->password = (char *)malloc(strlen(passwd) + 1);
        student->selected_course = NULL;
        strncpy(student->name, name, strlen(name) + 1);
        strncpy(student->password, passwd, strlen(passwd) + 1);

        // set the current student to the new register student, why ? It is convenient to read data at the beginning
        pStudent = student;
        student->next = p;
        if (q)
            q->next = student;
        else
            students = student;

        return SUCCESS;
    }
    else
        return ALREADY_EXIST;
}

static enum STATUS register_teacher(char *name, char *passwd)
{
    Teacher *p = teachers;
    Teacher *q = NULL;
    while (p && strcmp(p->name, name) < 0)
    {
        q = p;
        p = p->next;
    }
    if (!p || strcmp(p->name, name) > 0)
    {
        Teacher *teacher = (Teacher *)malloc(sizeof(Teacher));
        teacher->name = (char *)malloc(strlen(name) + 1);
        teacher->password = (char *)malloc(strlen(passwd) + 1);
        teacher->published_course = NULL;
        strncpy(teacher->name, name, strlen(name) + 1);
        strncpy(teacher->password, passwd, strlen(passwd) + 1);
        // set the current teacher to the new register teacher, why ? It is convenient to read data at the beginning

        pTeacher = teacher;
        teacher->next = p;
        if (q)
            q->next = teacher;
        else
            teachers = teacher;

        return SUCCESS;
    }
    else
        return ALREADY_EXIST;
}

static enum STATUS publish_course1(char *code, char *name, int capacity)
{

    // insert to teacher info
    Info *r = pTeacher->published_course;
    Info *t = NULL;
    while (r && strcmp(r->code, code) < 0)
    {
        t = r;
        r = r->next;
    }
    if (r && !strcmp(r->code, code))
        return ALREADY_DONE;

    Info *newInfo = (Info *)malloc(sizeof(Info));
    newInfo->code = (char *)malloc(strlen(code) + 1);
    newInfo->name = (char *)malloc(strlen(name) + 1);
    strncpy(newInfo->code, code, strlen(code) + 1);
    strncpy(newInfo->name, name, strlen(name) + 1);
    newInfo->next = NULL;

    // insert to courses
    Course *p = courses;
    Course *q = NULL;
    while (p && strcmp(p->code, code) < 0)
    {
        q = p;
        p = p->next;
    }
    if (!p || strcmp(p->code, code) > 0)
    {
        Course *course = (Course *)malloc(sizeof(Course));
        course->code = (char *)malloc(strlen(code) + 1);
        course->name = (char *)malloc(strlen(name) + 1);
        course->lecturer = (char *)malloc(strlen(pTeacher->name) + 1);
        course->capacity = capacity;
        course->selected_number = 0;
        course->next = NULL;
        strncpy(course->code, code, strlen(code) + 1);
        strncpy(course->name, name, strlen(name) + 1);
        strncpy(course->lecturer, pTeacher->name, strlen(pTeacher->name) + 1);

        course->next = p;
        if (q)
            q->next = course;
        else
            courses = course;

        newInfo->next = r;
        if (t)
            t->next = newInfo;
        else
            pTeacher->published_course = newInfo;
        return SUCCESS;
    }
    else
        return ALREADY_EXIST;
}

static enum STATUS select_course1(char *code)
{
    // insert to info
    Info *r = pStudent->selected_course;
    Info *t = NULL;

    while (r && strcmp(r->code, code) < 0)
    {
        t = r;
        r = r->next;
    }
    if (r && !strcmp(r->code, code))
        return ALREADY_DONE;

    Course *p = courses;
    while (p && strcmp(p->code, code) < 0)
        p = p->next;
    if (!p || strcmp(p->code, code) > 0)
        return NON_EXISTENT;
    else
    {
        if (p->selected_number < p->capacity)
        {
            p->selected_number++;
            Info *newInfo = (Info *)malloc(sizeof(Info));
            newInfo->code = (char *)malloc(strlen(code) + 1);
            newInfo->name = (char *)malloc(strlen(p->name) + 1);
            newInfo->next = NULL;
            strncpy(newInfo->code, code, strlen(code) + 1);
            strncpy(newInfo->name, p->name, strlen(p->name) + 1);

            newInfo->next = r;
            if (t)
                t->next = newInfo;
            else
                pStudent->selected_course = newInfo;

            return SUCCESS;
        }
        else
            return FAILURE;
    }
}

static enum STATUS drop_out_course1(char *code)
{
    Course *p = courses;
    while (p && strcmp(p->code, code) < 0)
        p = p->next;
    // the course of the code does not existed
    if (!p || strcmp(p->code, code) > 0)
        return NON_EXISTENT;
    else
    {
        // find the info of the course code
        Info *r = pStudent->selected_course;
        Info *t = NULL;

        while (r && strcmp(r->code, code) < 0)
        {
            t = r;
            r = r->next;
        }
        // have not selected the course of the code
        if (!r || strcmp(r->code, code) > 0)
            return FAILURE;
        p->selected_number--;
        if (t)
            t->next = t->next->next;
        else
            pStudent->selected_course = r->next;
        free(r);

        return SUCCESS;
    }
}
/* Student operation */
void select_course()
{
    puts("==========选课============");
    Course *p = courses;
    Info *q = pStudent->selected_course;
    char course_code[20];
    puts("课程代号\t课程名\t授课教师\t选课人数\t已选");
    while (p)
    {
        printf("%s\t%s\t%s\t%d/%d\t", p->code, p->name, p->lecturer, p->selected_number, p->capacity);
        if (q)
        {
            int i = strcmp(p->code, q->code);
            if (i > 0)
                q = q->next;
            else if (!i)
            {
                printf("√");
                q = q->next;
            }
        }
        putchar('\n');
        p = p->next;
    }
    printf("请输入选课课程代号: ");
    fgets(course_code, 20, stdin);
    course_code[strlen(course_code) - 1] = '\0';
    int status = select_course1(course_code);
    if (status == SUCCESS)
        printf("选课成功\n");
    else if (status == ALREADY_DONE)
        printf("该课程已选\n");
    else if (status == NON_EXISTENT)
        printf("课程不存在\n");
    else
        printf("该课程已满\n");
}

void query_course_selected()
{
    puts("==========查看已选课程============");
    puts("课程代号\t课程名\t授课教师\n");
    Course *p = courses;
    Info *q = pStudent->selected_course;
    while (p && q)
    {
        int i = strcmp(p->code, q->code);
        if (i < 0)
            p = p->next;
        else if (i > 0)
            q = q->next;
        else
        {
            printf("%s\t%s\t%s\n", p->code, p->name, p->lecturer);
            p = p->next;
            q = q->next;
        }
    }
}
void drop_out_course()
{
    puts("==========退课============");
    char course_code[20];
    printf("请输入退课课程代号: ");
    fgets(course_code, 20, stdin);
    course_code[strlen(course_code) - 1] = '\0';
    int status = drop_out_course1(course_code);
    if (status == SUCCESS)
        printf("退课成功!\n");
    else if (status == NON_EXISTENT)
        printf("退课失败，该课程不存在!\n");
    else if (status == FAILURE)
        printf("退课失败，未选该课程!\n");
}

/* Teacher operation */
void publish_course()
{
    puts("==========课程注册============");
    char course_code[20];
    char course_name[20];
    int capacity;

    printf("输入课程代号: ");
    fgets(course_code, 20, stdin);
    course_code[strlen(course_code) - 1] = '\0';
    printf("输入课程名称: ");
    fgets(course_name, 20, stdin);
    course_name[strlen(course_name) - 1] = '\0';
    printf("输入选课人数限制: ");
    scanf("%d", &capacity);
    getchar();

    int status = publish_course1(course_code, course_name, capacity);
    if (status == SUCCESS)
        printf("Operation successful!\n");
    else
        printf("the course code %s already exists!\n", course_code);
}

void query_course_published()
{
    puts("================课程信息====================");
    puts("代号\t课程名\t选课人数");
    Course *p = courses;
    Info *q = pTeacher->published_course;
    while (p && q)
    {
        int i = strcmp(p->code, q->code);
        if (i < 0)
            p = p->next;
        else if (i > 0)
            q = q->next;
        else
        {
            printf("%s\t%s\t%d/%d\n", p->code, p->name, p->selected_number, p->capacity);
            p = p->next;
            q = q->next;
        }
    }
}

/* Administrators */

void logon(int i)
{
    puts("========学生/教师注册==========");
    char name[20];
    char passwd[20];
    // register student
    if (i == STUDENT)
    {
        printf("请输入学生姓名: ");
        fgets(name, 20, stdin);
        name[strlen(name) - 1] = '\0';
        printf("请输入学生密码: ");
        fgets(passwd, 20, stdin);
        passwd[strlen(passwd) - 1] = '\0';
        enum STATUS status = register_student(name, passwd);
        if (status == SUCCESS)
            printf("Operation successful!\n");
        else
            printf("the student %s has already existed!\n", name);
    }
    else
    {
        // register teacher
        printf("请输入教师姓名: ");
        fgets(name, 20, stdin);
        name[strlen(name) - 1] = '\0';
        printf("请输入教师密码: ");
        fgets(passwd, 20, stdin);
        passwd[strlen(passwd) - 1] = '\0';
        enum STATUS status = register_teacher(name, passwd);
        if (status == SUCCESS)
            printf("Operation successful!\n");
        else
            printf("the teacher %s has already existed!\n", name);
    }
}

void view_students_info()
{
    puts("================学生信息====================");
    puts("姓名\t密码\t已选课程");
    Student *p = students;
    while (p)
    {
        printf("%s\t%s\t%s", p->name, p->password, p->selected_course ? p->selected_course->code : "null");
        Info *q = p->selected_course ? p->selected_course->next : NULL;
        while (q)
        {
            printf(", %s", q->code);
            q = q->next;
        }
        putchar('\n');
        p = p->next;
    }
}
void view_teacher_info()
{
    puts("================教师信息====================");
    puts("姓名\t密码\t注册课程");
    Teacher *p = teachers;
    while (p)
    {
        printf("%s\t%s\t%s", p->name, p->password, p->published_course ? p->published_course->code : "null");
        Info *q = p->published_course ? p->published_course->next : NULL;
        while (q)
        {
            printf(", %s", q->code);
            q = q->next;
        }
        putchar('\n');
        p = p->next;
    }
}
void view_course_info()
{
    puts("================课程信息====================");
    puts("代号\t课程名\t授课老师\t选课人数");
    Course *p = courses;
    while (p)
    {
        printf("%s\t%s\t%s\t%d/%d\n", p->code, p->name, p->lecturer, p->selected_number, p->capacity);
        putchar('\n');
        p = p->next;
    }
}

// login in
int login()
{
    int i = 1;
    int choice;
    char name[20];
    char passwd[20];
    bool success = false;

    while (true)
    {
        memset(name, 0, sizeof(name));
        memset(passwd, 0, sizeof(passwd));
        success = false;
        puts("=========欢迎登录学生选课系统=========");
        puts("[1] 退出\t[2] 登录");
        printf("请选择: ");
        scanf("%d", &choice);
        getchar();
        if (choice == 1)
        {
            puts("Bye!");
            log_out();
            exit(0);
        }
        puts("请问您的身份是： ");
        puts("[1] 学生\t[2] 教师\t[3]管理员");
        printf("请选择: ");
        scanf("%d", &i);
        getchar();
        while (i < 1 || i > 3)
        {
            puts("[1] 学生\t[2] 教师\t[3]管理员");
            printf("请重新选择: ");
            scanf("%d", &i);
            getchar();
        }

        printf("请输入姓名: ");
        fgets(name, 20, stdin);
        name[strlen(name) - 1] = '\0';
        printf("请输入密码: ");
        fgets(passwd, 20, stdin);
        passwd[strlen(passwd) - 1] = '\0';
        if (i == 1)
        {
            Student *p = students;
            while (p && strcmp(p->name, name))
                p = p->next;
            if (!p)
                printf("学生用户不存在!\n");
            else if (strcmp(p->password, passwd))
                printf("密码错误\n");
            else
            {
                pStudent = p;
                success = true;
            }
        }
        else if (i == 2)
        {
            Teacher *p = teachers;
            while (p && strcmp(p->name, name))
                p = p->next;
            if (!p)
                printf("教师用户不存在!\n");
            else if (strcmp(p->password, passwd))
                printf("密码错误\n");
            else
            {
                pTeacher = p;
                success = true;
            }
        }
        else
        {
            Administrator *p = administrators;
            while (p && strcmp(p->name, name))
                p = p->next;
            if (!p)
                printf("管理员用户不存在!\n");
            else if (strcmp(p->password, passwd))
                printf("密码错误");
            else
            {
                pAdministrator = p;
                success = true;
            }
        }
        if (success)
            home(i);
    }
}

void init()
{
    char *admin = "admin";
    char *passwd = "123";
    administrators = (Administrator *)malloc(sizeof(Administrator));
    administrators->name = (char *)malloc(6);
    administrators->password = (char *)malloc(4);
    strncpy(administrators->name, admin, 6);
    strncpy(administrators->password, passwd, 4);
    administrators->next = NULL;
    assert(!strcmp(administrators->name, admin));
    assert(!strcmp(administrators->password, passwd));

    init_teacher();
    init_student();
}

// home page
void home(int id)
{
    int choice;
    bool exited = false;
    switch (id)
    {
        // student login
    case 1:
    {
        while (!exited)
        {
            printf("=========欢迎%s同学=========\n", pStudent->name);
            puts("[1] 查询已选课程\t[2] 选课\t[3] 退课\t[4] 退出");
            printf("请选择: ");
            scanf("%d", &choice);
            getchar();
            switch (choice)
            {
            case 1:
                query_course_selected();
                break;
            case 2:
                select_course();
                break;
            case 3:
                drop_out_course();
                break;
            case 4:
                printf("Bye!\n");
                exited = true;
                break;
            default:
                break;
            }
        }
    }
    break;
    case 2:
    {
        while (!exited)
        {
            printf("=========欢迎%s老师=========\n", pTeacher->name);
            puts("[1] 发布课程\t[2] 查看选课情况\t[3] 退出");
            printf("请选择: ");
            scanf("%d", &choice);
            getchar();
            switch (choice)
            {
            case 1:
                publish_course();
                break;
            case 2:
                query_course_published();
                break;
            case 3:
                printf("Bye!\n");
                exited = true;
                break;
            default:
                break;
            }
        }
    }
    break;
    // admin login
    case 3:
    {
        while (!exited)
        {
            printf("=========欢迎%s同志=========\n", pAdministrator->name);
            puts("[1] 注册学生\t[2] 注册教师\t[3] 查看学生信息\t[4] 查看教师信息\t[5] 查看课程信息\t[6] 退出");
            printf("请选择: ");
            scanf("%d", &choice);
            getchar();
            switch (choice)
            {
            case 1:
                logon(STUDENT);
                break;
            case 2:
                logon(TEACHER);
                break;
            case 3:
                view_students_info();
                break;
            case 4:
                view_teacher_info();
                break;
            case 5:
                view_course_info();
                break;
            case 6:
                printf("Bye!\n");
                exited = true;
                break;
            default:
                break;
            }
        }
    }
    break;
    default:
        fprintf(stderr, "Home Error\n");
        break;
    }
}

void log_out()
{
    Student *p = students;
    Teacher *q = teachers;
    Course *r = courses;
    // save into file
    FILE *fstu = fopen("/home/rda/Downloads/StudentCourseSelectionSystem/src/students.txt", "w+");
    assert(fstu);

    while (p)
    {
        fprintf(fstu, "%s %s ", p->name, p->password);
        Info *info = p->selected_course;
        while (info)
        {
            fprintf(fstu, "%s;", info->code);
            info = info->next;
        }
        fprintf(fstu, "\n");
        p = p->next;
    }
    fclose(fstu);

    FILE *ftea = fopen("/home/rda/Downloads/StudentCourseSelectionSystem/src/teachers.txt", "w+");
    // free all structures
    assert(ftea);

    while (q)
    {
        fprintf(ftea, "%s %s ", q->name, q->password);
        Info *info = q->published_course;
        while (info)
        {
            while (strcmp(r->code, info->code) < 0)
                r = r->next;
            fprintf(ftea, "%s,%s,%d;", info->code, info->name, r->capacity);
            info = info->next;
        }
        fprintf(fstu, "\n");
        q = q->next;
        r = courses;
    }
    fclose(ftea);

    // free all structures

    pStudent = students;
    while (pStudent)
    {
        free(pStudent->name);
        free(pStudent->password);
        free_info(pStudent->selected_course);
        pStudent = pStudent->next;
    }

    pTeacher = teachers;
    while (pTeacher)
    {
        free(pTeacher->name);
        free(pTeacher->password);
        free_info(pTeacher->published_course);
        pTeacher = pTeacher->next;
    }

    free(administrators->name);
    free(administrators->password);

    Course *pc = courses;
    while (pc)
    {
        free(pc->name);
        free(pc->code);
        free(pc->lecturer);
        pc = pc->next;
    }
}

static void free_info(Info *pi)
{
    while (pi)
    {
        free(pi->name);
        free(pi->code);
        pi = pi->next;
    }
}