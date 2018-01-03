#ifndef CONFIGKIT_H
#define CONFIGKIT_H

#include <QMainWindow>
#include <QCloseEvent>
#include  "mainwindow.h"
#include <QSerialPort>
namespace Ui {
class ConfigKit;
}

class ConfigKit : public QMainWindow
{
    Q_OBJECT

public:
    explicit ConfigKit(QWidget *parent = 0);
    ~ConfigKit();

private slots:
    void on_pushButtonEnviarConfiguracion_clicked();


private:
    Ui::ConfigKit *ui;
    void closeEvent(QCloseEvent *event);
    void ErrorIngresaNumero();
    void validarParametros(QString );

};

#endif // CONFIGKIT_H
