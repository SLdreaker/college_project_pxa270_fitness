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
int chatroom = 0, diet = 0, fit = 0, exercise_data = 0;
char* test_num = "100";
char* chat_data_from_other = "";
char* member="";
char* temp_for_chatroom = "";
char* send_text = "";
char* send_weight = "";
char* send_height = "";
char* send_age = "";
char* send_gender = "";
char* send_goal = "";
char* send_minute = "";

float fBMR = 0;
int chat_send_ok = 0;
int count2 = 0;
int sport = 0;
char *arr_3[3];
int can_check_chat = 1;
int new_from_server = 0;
QByteArray byteArray, byteArray_weight, byteArray_height, byteArray_age, byteArray_gender;
QByteArray byteArray_goal, byteArray_minute;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

Clinet::Clinet(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Clinet)
{
    ui->setupUi(this);

    invisible();
    invisible_diet();
    invisible_FIT();
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
    visible_FIT();
}

void Clinet::on_Button_Diet_clicked()
{
    invisible();
    visible_diet();
    ui->SP_exercise->setRange(0,7);
}

void Clinet::on_Button_calculate_clicked()
{
    int heavy_or_not = 0;
    QString weight = ui->LE_weight->text();
    float fWeight = weight.toFloat();
    QString height = ui->LE_height->text();
    float fHeight = height.toFloat();
    QString exercise = ui->SP_exercise->text();
    float fExercise = exercise.toFloat();

    byteArray_weight=weight.toLocal8Bit();
    send_weight=byteArray_weight.data();

    byteArray_height=height.toLocal8Bit();
    send_height=byteArray_height.data();

    float fBMI = 0;
    fBMI = fWeight/((fHeight/100.0)*(fHeight/100.0));
    QString qBMI = QString::number(fBMI, 'f', 2);
    ui->label_BMI_NUM->setText(qBMI);
    if(fBMI>=25)
    {
        ui->label_heavy->setText("Too heavy");
        heavy_or_not = 1;
    }
    else if(fBMI<25 && fBMI>18.5)
    {
        ui->label_heavy->setText("Normal");
        heavy_or_not = 0;
    }
    else if(fBMI<18.5)
    {
        ui->label_heavy->setText("Too light");
        heavy_or_not = -1;
    }

    float fTDEE = 0;
    if(fExercise <= 2)
    {
        if(heavy_or_not == -1)
            fTDEE = 35*fWeight;
        else if(heavy_or_not == 0)
            fTDEE = 30*fWeight;
        else if(heavy_or_not == 1)
            fTDEE = 22.5*fWeight;
    }
    else if(fExercise > 2 && fExercise <= 5)
    {
        if(heavy_or_not == -1)
            fTDEE = 40*fWeight;
        else if(heavy_or_not == 0)
            fTDEE = 35*fWeight;
        else if(heavy_or_not == 1)
            fTDEE = 30*fWeight;
    }
    else if(fExercise > 5)
    {
        if(heavy_or_not == -1)
            fTDEE = 45*fWeight;
        else if(heavy_or_not == 0)
            fTDEE = 40*fWeight;
        else if(heavy_or_not == 1)
            fTDEE = 35*fWeight;
    }
    QString qTDEE = QString::number(fTDEE, 'f', 2);
    ui->label_TDEE_NUM->setText(qTDEE + " kcal");
    diet = 1;
}


void Clinet::on_Button_back_diet_clicked()
{
    visible();
    invisible_diet();
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

void Clinet::on_Button_send_FIT_clicked()
{
    invisible_FIT();
    visible_FIT();

    int heavy_or_not = 0;
    QString weight_FIT = ui->LE_weight_FIT->text();
    float fWeight_FIT = weight_FIT.toFloat();
    QString height_FIT = ui->LE_height_FIT->text();
    float fHeight_FIT = height_FIT.toFloat();
    QString age_FIT = ui->SB_age_FIT->text();
    float fAge_FIT = age_FIT.toFloat();

    bool male = ui->RB_Male_FIT->isChecked();
    bool Female = ui->RB_Female_FIT->isChecked();

    QString gender_FIT = "0";
    fBMR = 0;
    if(male == true && Female == false)
    {
        gender_FIT = "1";
        fBMR = 13.7*fWeight_FIT + 5*fHeight_FIT - 6.8*fAge_FIT + 66;
    }
    else if(male == false && Female == true)
    {
        gender_FIT = "0";
        fBMR = 9.6*fWeight_FIT + 1.8*fHeight_FIT - 4.7*fAge_FIT + 655;
    }

    QString qBRM = QString::number(fBMR, 'f', 2);
    ui->label_BMR_NUM_FIT->setText(qBRM + " kcal/day");

    byteArray_weight=weight_FIT.toLocal8Bit();
    send_weight=byteArray_weight.data();

    byteArray_height=height_FIT.toLocal8Bit();
    send_height=byteArray_height.data();

    byteArray_age=age_FIT.toLocal8Bit();
    send_age=byteArray_age.data();

    byteArray_gender=gender_FIT.toLocal8Bit();
    send_gender=byteArray_gender.data();

    ui->Button_send_FIT->setEnabled(false);

    ui->Button_day_FIT->setVisible(true);
    ui->label_minute_FIT->setVisible(true);
    ui->label_sport_FIT->setVisible(true);
    ui->label_goal_FIT->setVisible(true);

    ui->SB_goal_FIT->setVisible(true);
    ui->SB_minute_FIT->setVisible(true);
    ui->SB_goal_FIT->setRange(0,int(fWeight_FIT) - 10);
    ui->SB_minute_FIT->setRange(0,300);

    ui->FCB_sport_FIT->setVisible(true);
    ui->FCB_sport_FIT->clear();
    QStringList sport;
    sport << "Running" << "Cycling" << "Badminton" << "Baseball" << "Basketball" << "Bowling" << "Boxing" << "Dancing" << "Ice skating";
    ui->FCB_sport_FIT->addItems(sport);

    fit = 1;
}

void Clinet::on_Button_day_FIT_clicked()
{
    QString goal_FIT = ui->SB_goal_FIT->text();
    float fGoal_FIT = goal_FIT.toFloat();
    QString minute_FIT = ui->SB_minute_FIT->text();
    float fMinute_FIT = minute_FIT.toFloat();
    float sport_cal[9] ={18,8,7,5,8,3,6,6.5,7};

    sport = ui->FCB_sport_FIT->currentIndex();
    int total_cal = 0;
    float using_cal = 0;
    total_cal = fGoal_FIT * 7700;
    using_cal = ((fBMR*sport_cal[sport]) / 24)*fMinute_FIT / 60;
    float fday = (total_cal/using_cal);

    ui->label_day_FIT->setVisible(true);
    ui->label_day_exercise_FIT->setVisible(true);
    QString qday = QString::number(fday, 'f', 0);
    ui->label_day_FIT->setText(qday);

    ui->Button_send_FIT->setEnabled(true);

    byteArray_goal=goal_FIT.toLocal8Bit();
    send_goal=byteArray_goal.data();

    byteArray_minute=minute_FIT.toLocal8Bit();
    send_minute=byteArray_minute.data();

    exercise_data = 1;
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


void Clinet::on_Button_back_FIT_clicked()
{
    invisible_FIT();
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

void Clinet::invisible_diet()
{
    ui->LE_height->setVisible(false);
    ui->LE_weight->setVisible(false);
    ui->SP_exercise->setVisible(false);
    ui->Button_calculate->setVisible(false);

    ui->label_advice->setVisible(false);
    ui->label_report->setVisible(false);
    ui->Button_back_diet->setVisible(false);
    ui->label_BMI->setVisible(false);
    ui->label_BMI_NUM->setVisible(false);
    ui->label_cm->setVisible(false);
    ui->label_excercie->setVisible(false);
    ui->label_height->setVisible(false);
    ui->label_kg->setVisible(false);
    ui->label_TDEE->setVisible(false);
    ui->label_TDEE_NUM->setVisible(false);
    ui->label_weight->setVisible(false);
    ui->label_heavy->setVisible(false);
    ui->label_day_FIT->setVisible(false);
    ui->label_day_exercise_FIT->setVisible(false);
}

void Clinet::visible_diet()
{
    ui->LE_height->setVisible(true);
    ui->LE_weight->setVisible(true);
    ui->SP_exercise->setVisible(true);
    ui->Button_calculate->setVisible(true);

    ui->label_advice->setVisible(true);
    ui->label_report->setVisible(true);
    ui->Button_back_diet->setVisible(true);
    ui->label_BMI->setVisible(true);
    ui->label_BMI_NUM->setVisible(true);
    ui->label_cm->setVisible(true);
    ui->label_excercie->setVisible(true);
    ui->label_height->setVisible(true);
    ui->label_kg->setVisible(true);
    ui->label_TDEE->setVisible(true);
    ui->label_TDEE_NUM->setVisible(true);
    ui->label_weight->setVisible(true);
    ui->label_heavy->setVisible(true);
}

void Clinet::invisible_FIT()
{
    ui->Button_send_FIT->setVisible(false);
    ui->Button_day_FIT->setVisible(false);
    ui->Button_back_FIT->setVisible(false);
    ui->LE_height_FIT->setVisible(false);
    ui->LE_weight_FIT->setVisible(false);
    ui->FCB_sport_FIT->setVisible(false);

    ui->RB_Female_FIT->setVisible(false);
    ui->RB_Male_FIT->setVisible(false);
    ui->SB_age_FIT->setVisible(false);
    ui->SB_goal_FIT->setVisible(false);
    ui->SB_minute_FIT->setVisible(false);

    ui->label_age_FIT->setVisible(false);
    ui->label_cm_FIT->setVisible(false);
    ui->label_height_FIT->setVisible(false);
    ui->label_kg_FIT->setVisible(false);
    ui->label_weight_FIT->setVisible(false);
    ui->label_BMR_NUM_FIT->setVisible(false);
    ui->label_BMR_FIT->setVisible(false);
    ui->label_minute_FIT->setVisible(false);
    ui->label_sport_FIT->setVisible(false);
    ui->label_goal_FIT->setVisible(false);
    ui->label_day_FIT->setVisible(false);
    ui->label_day_exercise_FIT->setVisible(false);
}

void Clinet::visible_FIT()
{
    ui->Button_send_FIT->setVisible(true);
    ui->Button_back_FIT->setVisible(true);
    ui->LE_height_FIT->setVisible(true);
    ui->LE_weight_FIT->setVisible(true);

    ui->RB_Female_FIT->setVisible(true);
    ui->RB_Male_FIT->setVisible(true);
    ui->SB_age_FIT->setVisible(true);

    ui->label_age_FIT->setVisible(true);
    ui->label_cm_FIT->setVisible(true);
    ui->label_height_FIT->setVisible(true);
    ui->label_kg_FIT->setVisible(true);
    ui->label_weight_FIT->setVisible(true);
    ui->label_BMR_NUM_FIT->setVisible(true);
    ui->label_BMR_FIT->setVisible(true);
}

void Clinet::clear_FIT()
{
    ui->LE_height_FIT->clear();
    ui->LE_weight_FIT->clear();
    ui->FCB_sport_FIT->clear();

    ui->SB_age_FIT->clear();
    ui->SB_goal_FIT->clear();
    ui->SB_minute_FIT->clear();

    ui->label_BMR_NUM_FIT->clear();
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
        if(diet == 1)
        {
            can_check_chat = 0;
            n = sprintf(snd, "3,David(S): ,%s kg|%s cm",send_weight, send_height);
            read_server = write_read_to_server(connfd, snd);
            if(strncmp(read_server,"100",2) == 0)
            {
                test_num = "1";
            }
            can_check_chat = 1;
            memset(snd, 0, BUFSIZE);    //clear data
            memset(read_server, 0, BUFSIZE);    //clear data
            diet = 0;
        }
        if(fit == 1)
        {
            can_check_chat = 0;
            n = sprintf(snd, "4,David(S): ,%s|%s|%s|%s",send_weight, send_height,send_age,send_gender);
            read_server = write_read_to_server(connfd, snd);
            if(strncmp(read_server,"100",2) == 0)
            {
                test_num = "1";
            }
            can_check_chat = 1;
            memset(snd, 0, BUFSIZE);    //clear data
            memset(read_server, 0, BUFSIZE);    //clear data
            fit = 0;
        }
        if(exercise_data == 1)
        {
            can_check_chat = 0;
            n = sprintf(snd, "5,David(S): ,%s|%s|%d",send_goal, send_gender,sport);
            read_server = write_read_to_server(connfd, snd);
            if(strncmp(read_server,"100",2) == 0)
            {
                test_num = "1";
            }
            can_check_chat = 1;
            memset(snd, 0, BUFSIZE);    //clear data
            memset(read_server, 0, BUFSIZE);    //clear data
            exercise_data = 0;
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

