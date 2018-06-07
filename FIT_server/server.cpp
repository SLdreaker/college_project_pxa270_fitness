#include "server.h"
#include "ui_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sockop.h>

#include <sys/types.h>
#include <signal.h>
#include <QTimer>
#include <QDate>
#include <QTime>

#define BUFSIZE 1024
#define NUMTHREADS 3
void* connect_thread(void*);
void* connect_thread2(void*);
void handler(int signo, siginfo_t *info, void* context);
int stop = 0;
char *chat_data_from_other = "";
char *member="";
char temp_for_chatroom[BUFSIZE] = "";
char* send_text="Hello";
pid_t tid;  //
int count2 = 0, count4 = 0;
char *arr_3[3], *arr_4[4];
int refresh = 0;
int send_data = 0;
QByteArray byteArray;

Server::Server(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);
    ui->Button_send->setVisible(false);
    ui->LE_send->setVisible(false);
    ui->TB_chatroom->setVisible(false);
}

Server::~Server()
{
    delete ui;
}

void Server::on_pushButton_clicked()
{
    pthread_t threads[NUMTHREADS];
    pthread_attr_t attr;
    int rc;

        /* Initialize and set thread detached attribute */
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

        printf("In main(): creating thread %d\n", 0);
        rc = pthread_create(&threads[0], &attr, connect_thread, (void*)ui);
        if(rc)
        {
            printf("ERROR; return code from pthread create() is %d\n", rc);
            exit(-1);
        }
        rc = pthread_create(&threads[1], &attr, connect_thread2, (void*)ui);
        if(rc)
        {
            printf("ERROR; return code from pthread create() is %d\n", rc);
            exit(-1);
        }
    ui->pushButton->setVisible(false);
    ui->Button_send->setVisible(true);
    ui->LE_send->setVisible(true);
    ui->TB_chatroom->setVisible(true);
    ui->TB_chatroom->append("System: System open");
    QTimer* timer=new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkchatroom()));
    timer->start(100);
}

void Server::on_Button_send_clicked()
{

    QString content = ui->LE_send->text();
    QString menber = "System: ";
    ui->TB_chatroom->append(menber + content);
    ui->LE_send->clear();

    byteArray=content.toLocal8Bit();
    send_text=byteArray.data();
    send_data = 1;
}

void Server::on_pushButton_2_clicked()
{
    ui->TB_chatroom->append(send_text);
}

void handler(int signo, siginfo_t *info, void* context)
{
    stop = 1;
}

void* connect_thread(void* ui)
{
        int sockfd , connfd; /* socket descriptor */
        struct sockaddr_in addr_cln;
        socklen_t sLen = sizeof(addr_cln);
        char* port = "8740";
        char snd[BUFSIZE], rcv[BUFSIZE], temp[BUFSIZE];
        int n;
        struct sigaction my_action;
        stop = 1;
        tid = getpid(); //
        Ui::Server* at = dynamic_cast<Ui::Server*>(static_cast<Ui::Server*>(ui));
        /* register handler to SIGUSR1 */
        memset(&my_action, 0, sizeof(struct sigaction));
        my_action.sa_flags = SA_SIGINFO;
        my_action.sa_sigaction = handler;
        sigaction(SIGUSR1, &my_action, NULL);

        sockfd = passivesock(port, "tcp", 10);
        connfd = ::accept(sockfd, (struct sockaddr*)&addr_cln ,&sLen);
        if(connfd == -1)
            errexit("Error : accept()\n");

        while(1)
        {
            /* read message from client */
            if((n = read(connfd, rcv, BUFSIZE)) == -1)
                errexit("Error : read()\n");
            if(rcv[0] == '1')
            {
                count2 = 0;
                arr_3[count2] = strtok(rcv, ",");

                while(arr_3[count2] != NULL)
                {
                      count2 = count2 + 1;
                      arr_3[count2] = strtok(NULL, ",");
                 }
                member = arr_3[1];
                chat_data_from_other = arr_3[2];

                n = sprintf(snd, "100");

                  if(( n = write(connfd, snd, n)) == -1)
                       errexit("Error: write() \n");
                  refresh = 1;
//                 memset(rcv, 0, BUFSIZE);    //clear data
//                 memset(snd, 0, BUFSIZE);    //clear data
            }
            else if(rcv[0] == '3')
            {
                count2 = 0;
                arr_3[count2] = strtok(rcv, ",");

                while(arr_3[count2] != NULL)
                {
                      count2 = count2 + 1;
                      arr_3[count2] = strtok(NULL, ",");
                 }
                member = arr_3[1];
                chat_data_from_other = arr_3[2];

                n = sprintf(snd, "100");

                  if(( n = write(connfd, snd, n)) == -1)
                       errexit("Error: write() \n");
                  refresh = 1;

            }
            else if(rcv[0] == '4')
            {
                count2 = 0;
                arr_3[count2] = strtok(rcv, ",");

                while(arr_3[count2] != NULL)
                {
                      count2 = count2 + 1;
                      arr_3[count2] = strtok(NULL, ",");
                 }
                member = arr_3[1];
                chat_data_from_other = arr_3[2];

                n = sprintf(snd, "100");

                  if(( n = write(connfd, snd, n)) == -1)
                       errexit("Error: write() \n");
                  refresh = 1;
            }
            else if(rcv[0] == '5')
            {
                count2 = 0;
                arr_3[count2] = strtok(rcv, ",");

                while(arr_3[count2] != NULL)
                {
                      count2 = count2 + 1;
                      arr_3[count2] = strtok(NULL, ",");
                 }
                member = arr_3[1];
                chat_data_from_other = arr_3[2];

                n = sprintf(snd, "100");

                  if(( n = write(connfd, snd, n)) == -1)
                       errexit("Error: write() \n");
                  refresh = 1;
            }
         }
         ::close(connfd);
         ::close(sockfd);
         pthread_exit(NULL);
}

void* connect_thread2(void* ui)
{
        int sockfd , connfd; /* socket descriptor */
        struct sockaddr_in addr_cln;
        socklen_t sLen = sizeof(addr_cln);
        char* port = "8750";
        char snd[BUFSIZE], rcv[BUFSIZE], temp[BUFSIZE];
        int n;

        sockfd = passivesock(port, "tcp", 10);
        connfd = ::accept(sockfd, (struct sockaddr*)&addr_cln ,&sLen);
        if(connfd == -1)
            errexit("Error : accept()\n");

        while(1)
        {
            /* read message from client */
            if((n = read(connfd, rcv, BUFSIZE)) == -1)
                errexit("Error : read()\n");
            if(rcv[0] == 's')
            {
                if(send_data == 1)
                {
                    n = sprintf(snd, "2,System:,%s", send_text);
                    if(( n = write(connfd, snd, n)) == -1)
                         errexit("Error: write() \n");
                    send_data = 0;
                    memset(snd, 0, BUFSIZE);    //clear data
                }
                else
                {
                    n = sprintf(snd, "3");
                    if(( n = write(connfd, snd, n)) == -1)
                         errexit("Error: write() \n");
                }
            }

         }
         ::close(connfd);
         ::close(sockfd);
         pthread_exit(NULL);
}

void Server::checkchatroom()
{
    if(refresh == 1)
    {
        QString content_r = QString::fromUtf8(chat_data_from_other);
        QString menber_r = QString::fromUtf8(member);
        QString test(send_text);
        QDate dt_now = QDate::currentDate();
        QTime tm_now = QTime::currentTime();

        ui->TB_chatroom->append(menber_r + content_r + "    " + dt_now.toString("yyyy/MM/dd") + " " + tm_now.toString("hh:mm:ss"));
        refresh = 0;
        memset(chat_data_from_other, 0, strlen(chat_data_from_other));    //clear data
        memset(member, 0, strlen(member));    //clear data
    }
}
