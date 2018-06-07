#ifndef CLINET_H
#define CLINET_H

#include <QMainWindow>

namespace Ui {
class Clinet;
}

class Clinet : public QMainWindow
{
    Q_OBJECT

public:
    explicit Clinet(QWidget *parent = 0);
    ~Clinet();
    Ui::Clinet *ui;

private slots:
    void on_Buttom_Fitness_clicked();
    void invisible();
    void visible();
    void invisible_diet();
    void visible_diet();
    void invisible_FIT();
    void visible_FIT();
    void clear_FIT();

    void on_Button_Diet_clicked();

    void on_Button_chatroom_clicked();

    void on_Button_send_clicked();

    void on_Button_back_clicked();

    void on_Button_connect_clicked();

    void checkchatroom();

    void on_Button_back_diet_clicked();

    void on_Button_calculate_clicked();

    void on_Button_send_FIT_clicked();

    void on_Button_day_FIT_clicked();

    void on_Button_back_FIT_clicked();

private:

};

void* connect_thread(void*);
char* write_read_to_server(int , char* );
void* connect_thread2(void* );
#endif // CLINET_H
