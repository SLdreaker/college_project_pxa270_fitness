#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>

namespace Ui {
class Server;
}

class Server : public QMainWindow
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = 0);
    ~Server();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_Button_send_clicked();

    void checkchatroom();
private:
    Ui::Server *ui;
};

#endif // SERVER_H
