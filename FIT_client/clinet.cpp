#include "clinet.h"
#include "ui_clinet.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h> /* Primitive System Data Types */
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sockop.h>
#include <time.h>

#include <sys/types.h>
#include <signal.h>
#include <QTimer>

#define BUFSIZE 1024
int connfd;
int connfd2;
int chatroom = 0;
char* test_num = "100";
char* chat_data_from_other = "";
char* member="";
char* temp_for_chatroom = "";
char* send_text = "";
int chat_send_ok = 0;
int count2 = 0;
char *arr_3[3];
int can_check_chat = 1;
int new_from_server = 0;
QByteArray byteArray;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

Clinet::Clinet(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Clinet)
{
    ui->setupUi(this);

    invisible();
    ui->Button_send->setVisible(false);
    ui->LE_send->setVisible(false);
    ui->TB_chatroom->setVisible(false);
    ui->Button_back->setVisible(false);
}

Clinet::~Clinet()
{
    delete ui;
}

void Clinet::on_Button_connect_clicked()
{

    pthread_t thread;
    pthread_attr_t attr;
    pthread_t thread2;
    pthread_attr_t attr2;
    int rc;

    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    printf("In main(): creating thread %d\n", 0);
    rc = pthread_create(&thread, &attr, connect_thread, (void*)ui);
    if(rc)
    {
        printf("ERROR; return code from pthread create() is %d\n", rc);
        exit(-1);
    }

    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr2);
    pthread_attr_setdetachstate(&attr2, PTHREAD_CREATE_JOINABLE);

    printf("In main(): creating thread %d\n", 0);
    rc = pthread_create(&thread2, &attr2, connect_thread2, (void*)ui);
    if(rc)
    {
        printf("ERROR; return code from pthread create() is %d\n", rc);
        exit(-1);
    }

    rc = pthread_mutex_lock(&mutex);
    ui->Button_connect->setVisible(false);
    rc = pthread_mutex_unlock(&mutex);

    visible();
}

void Clinet::on_Buttom_Fitness_clicked()
{
    invisible();
}

void Clinet::on_Button_Diet_clicked()
{
    invisible();
}

void Clinet::on_Button_chatroom_clicked()
{
    invisible();
    ui->Button_send->setVisible(true);
    ui->LE_send->setVisible(true);
    ui->TB_chatroom->setVisible(true);
    ui->Button_back->setVisible(true);
    ui->TB_chatroom->append("System: David login");
    chatroom = 1;
    QTimer* timer=new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkchatroom()));
    timer->start(100);
}

//chatroom operation
void Clinet::on_Button_send_clicked()
{
    int n;
    char snd[BUFSIZE];
    char* read_server;

    QString content = ui->LE_send->text();
    QString menber = "David: ";
    ui->TB_chatroom->append(menber + content);
    ui->LE_send->clear();
    byteArray=content.toLocal8Bit();
    send_text=byteArray.data();

    chat_send_ok = 1;
}

void Clinet::on_Button_back_clicked()
{
    ui->TB_chatroom->clear();
    ui->Button_send->setVisible(false);
    ui->LE_send->setVisible(false);
    ui->TB_chatroom->setVisible(false);
    ui->Button_back->setVisible(false);
    visible();
}


void Clinet::invisible()
{
    ui->Buttom_Fitness->setVisible(false);
    ui->Button_Diet->setVisible(false);
    ui->Button_chatroom->setVisible(false);
    ui->label_chatroom->setVisible(false);
    ui->label_diet->setVisible(false);
    ui->label_fitness->setVisible(false);
}

void Clinet::visible()
{
    int rc;
    rc = pthread_mutex_lock(&mutex);
    ui->Buttom_Fitness->setVisible(true);
    ui->Button_Diet->setVisible(true);
    ui->Button_chatroom->setVisible(true);
    ui->label_chatroom->setVisible(true);
    ui->label_diet->setVisible(true);
    ui->label_fitness->setVisible(true);
    rc = pthread_mutex_unlock(&mutex);
}

void* connect_thread(void* ui)
{
    int rc, n;
    char snd[BUFSIZE], snd2[BUFSIZE];
    char* read_server;
//    Ui::Clinet* at = dynamic_cast<Ui::Clinet*>(static_cast<Ui::Clinet*>(ui));
    connfd = connectsock("140.113.156.33", "8740", "tcp");

    while(1)
    {
        if(chatroom == 1)
        {
            if(chat_send_ok ==1)
            {
                can_check_chat = 0;
                n = sprintf(snd, "1,David: ,%s",send_text);
                read_server = write_read_to_server(connfd, snd);
                if(strncmp(read_server,"100",2) == 0)
                {
                    test_num = "1";
                }
                chat_send_ok = 0;
                memset(snd, 0, BUFSIZE);    //clear data
                memset(read_server, 0, BUFSIZE);    //clear data
                can_check_chat = 1;
            }
        }
    }

    pthread_exit(NULL);
}

void* connect_thread2(void* ui)
{
    int rc, n;
    char snd2[BUFSIZE];
    char read_server[BUFSIZE];

    connfd2 = connectsock("140.113.156.33", "8750", "tcp");

    while(1)
    {
        if(chatroom == 1)
        {
            if(can_check_chat == 1)
            {
                n = sprintf(snd2, "system");

                /* write message to server */
                if((n = ::write(connfd2 ,snd2, strlen(snd2) ) ) == -1)
                    errexit("Error : write() \n");

                /* read message from the server and print */
                memset(snd2, 0 ,BUFSIZE);
                if(( n = ::read(connfd2 ,read_server ,BUFSIZE) ) == -1)
                    errexit("Error : read() \n");

                if(read_server[0] == '2')
                {
                    new_from_server = 1;
                    count2 = 0;

                    arr_3[count2] = strtok(read_server, ",");

                    while(arr_3[count2] != NULL )
                    {
                          count2 = count2 + 1;
                          arr_3[count2] = strtok(NULL, ",");
                     }
                    member = arr_3[1];
                    chat_data_from_other = arr_3[2];
                }
//                memset(snd2, 0, BUFSIZE);    //clear data
//                memset(read_server, 0, BUFSIZE);    //clear data
            }
        }
    }

    pthread_exit(NULL);
}

char* write_read_to_server(int connfd, char* data)
{
    int n;
    char buf[BUFSIZE];

    /* write message to server */
    if((n = ::write(connfd ,data, strlen(data) ) ) == -1)
        errexit("Error : write() \n");

    /* read message from the server and print */
    memset(buf, 0 ,BUFSIZE);
    if(( n = ::read(connfd ,buf ,BUFSIZE) ) == -1)
        errexit("Error : read() \n");
    test_num = "1";
    return buf;
}

void Clinet::checkchatroom()
{
    if(new_from_server == 1)
    {
        QString content_r(chat_data_from_other);
        QString menber_r(member);
        ui->TB_chatroom->append(menber_r + content_r);
        new_from_server = 0;
        memset(chat_data_from_other, 0, strlen(chat_data_from_other));    //clear data
        memset(member, 0, strlen(member));    //clear data
    }
}
