#include "configkit.h"
#include "ui_configkit.h"

ConfigKit::ConfigKit(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConfigKit)
{
    ui->setupUi(this);
}

ConfigKit::~ConfigKit()
{
    delete ui;
}

void ConfigKit:: ErrorIngresaNumero(){
    QMessageBox::critical(this, QString::fromUtf8("Ingresa solo Numeros"), "Solo se permiten numeros enteros mayores a cero");

}

void ConfigKit:: validarParametros(QString paraConfig){

    if(paraConfig.isEmpty())
    {
        QMessageBox::information(this, QString::fromUtf8("Parametros de configuracion"), "Ingreso solo números mayores a 0");
        return;
    }
    if(paraConfig.toInt() < 0 ){
         QMessageBox::critical(this, QString::fromUtf8("Error de configuración"), "No puede tener valores negativos");
        return;
    }
    if (paraConfig.toInt() == 0 ){
        ErrorIngresaNumero();
        return;
     }


}
void ConfigKit::on_pushButtonEnviarConfiguracion_clicked()
{
	 QString MensajeConfig,humMin,humMax,tiemRie,horaRieg;
	 if(ui->spinBoxHumeMax->value())
	 {
		 humMin = '0' + ui->spinBoxHumeMin->text();
	 }
	 else
	 {
		 humMin = ui->spinBoxHumeMin->text();
	 }
	 if(ui->spinBoxHumeMax->value() < 10)
	 {
		 humMax = '0' + ui->spinBoxHumeMin->text();
	 }
	 else
	 {
		 humMax = ui->spinBoxHumeMin->text();
	 }
	 tiemRie+=ui->timeEditTiempoRiego->text();
	 horaRieg+=ui->timeEditHoraRiego->text();
	 f =("#C"+humMin+humMax+tiemRie+horaRieg).toLatin1();// variable global compartida
	 confiPorSoft=true;//variable global compartida, con esta disparas en primera instancia cuando te conectas
	 confiPorSf=false;//variable global compartida, activas de nuevo la configuracion, en todo caso solo te deja abrir una ventana hasta que la cierrs
	 this->close();
}


void ConfigKit::closeEvent (QCloseEvent *event)
{
        confiPorSf=false;
        event->accept();
}
