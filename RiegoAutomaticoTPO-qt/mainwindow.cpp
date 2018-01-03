#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "configkit.h"
//#include <qextserialenumerator.h>

#include <QMessageBox>
#include <QDebug>

#include <QTimer>
#include <QPixmap>

#define SS_RELAY_ON         "background-color: yellow; color: black; font-weight: bold;"
#define SS_RELAY_OFF        "background-color: brown; color: white; font-weight: normal;"

#define SS_LED_ON           "background-color: red; color: white; font-weight: bold;"
#define SS_LED_OFF          "background-color: black; color: white; font-weight: normal;"

#define SS_TECLA_PRESS      "background-color: blue; border: 2px solid lightblue;"
#define SS_TECLA_RELEASE	"background-color: none;"

#define SS_INPUT_ON         "background-color: green;"
#define SS_INPUT_OFF        "background-color: red;"

#define LOG_MAXCOUNT    500
QByteArray f;
bool confiPorSoft;
 bool confiPorSf;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    puerto(NULL), m_cant_bytes_recibidos(0),
    m_cant_bytes_enviados(0), ui(new Ui::MainWindow)
{
    m_init = true;
    ui->setupUi(this);
    mostrarMarcador();
    estadoInicial();
    ejecutarTimer();
    UpdateTabs();
    EnumerarPuertos();
    ActualizarEstadoConexion();
    UpdateBytesTotales();
    m_init = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}
void::MainWindow:: ejecutarTimer(){
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this, SLOT(mostrarReloj()));
    timer->start();
//    QTimer *timerConfig = new QTimer(this);
//    connect(timerConfig, SIGNAL(timeout()), this, SLOT(configActive()));
//    timer->start();
}

void MainWindow::configActive(){

    //if(confiPorSoft){
     //  EnviarComando(f);
       QMessageBox::warning(this, QString::fromUtf8("config"),f);

     //}


}
void::MainWindow:: estadoInicial(){

    EstadoGeneral=0;
    cantAutomatico=0;
    cantManual=0;
    cantTemporizado=0;
    confiPorSf=false;
    estadoAutomatico=false;
    estadoManual=false;
    estadoTemporal=false;
    m_status_bytes_recibidos = new QLabel;
    ui->statusBar->addWidget(m_status_bytes_recibidos);
}


void MainWindow::mostrarMarcador(){

//    ui->lcdTemperatura->setStyleSheet("color: lightgreen; background-color: black; border: 2px solid lightgreen;");
//    ui->lcdTemperatura->setFrameShape(QFrame::Box);
//    ui->lcdTemperatura->setFrameShadow(QFrame::Raised);
//    ui->lcdTemperatura->setMidLineWidth(0);
//    ui->lcdTemperatura->setSmallDecimalPoint(true);
//    ui->lcdTemperatura->setDigitCount(3);
//    ui->lcdTemperatura->setMode(QLCDNumber::Dec);
//    ui->lcdTemperatura->setSegmentStyle(QLCDNumber::Flat);
//    ui->lcdTemperatura->setProperty("value", QVariant(18));
    ui->lcdHumedad->setStyleSheet("color: lightgreen; background-color: black; border: 2px solid lightgreen;");
    ui->lcdHumedad->setFrameShape(QFrame::Box);
    ui->lcdHumedad->setFrameShadow(QFrame::Raised);
    ui->lcdHumedad->setMidLineWidth(0);
    ui->lcdHumedad->setSmallDecimalPoint(true);
    ui->lcdHumedad->setDigitCount(4);
    ui->lcdHumedad->setMode(QLCDNumber::Dec);
    ui->lcdHumedad->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdHumedad->setProperty("value", QVariant(24.3));
  //  ui->lcdHumedad->setProperty("intValue", QVariant(24));

    ui->lcdHumedadActual->setStyleSheet("color: lightgreen; background-color: black; border: 2px solid lightgreen;");
    ui->lcdHumedadActual->setFrameShape(QFrame::Box);
    ui->lcdHumedadActual->setFrameShadow(QFrame::Raised);
    ui->lcdHumedadActual->setMidLineWidth(0);
    ui->lcdHumedadActual->setSmallDecimalPoint(true);
    ui->lcdHumedadActual->setDigitCount(4);
    ui->lcdHumedadActual->setMode(QLCDNumber::Dec);
    ui->lcdHumedadActual->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdHumedadActual->setProperty("value", QVariant(24.3));
}


void MainWindow::mostrarReloj(){
   time = QTime::currentTime();
   time_text = time.toString("hh : mm : ss");
   ui->Hora->setText(time_text);

   if(Conectado()){
       if(confiPorSoft){
           QMessageBox::warning(this, QString::fromUtf8("config"),f);
           confiPorSoft=false;
        }

   }

}

void MainWindow::enviarRTC(){

    QTime   time_aux = QTime::currentTime();
    QString  time_text_aux = time_aux.toString("hhmmss");
    EnviarComando("R"+time_text_aux);
}

void MainWindow::UpdateTabs()
{
    ui->tabWidget->setTabEnabled(1, true);
}

void MainWindow::CerrarPuerto()
{
    puerto->close();
    delete puerto;
    puerto=NULL;
}

void MainWindow::ActualizarEstadoConexion()
{
    if (Conectado())
    {
        ui->editEstado->setStyleSheet("font-weight: bold; color: green; background-color: black;");
        ui->editEstado->setText("CONECTADO");
        ui->buttonConexion->setText("Desconectar");
    }
    else
    {
        ui->editEstado->setStyleSheet("font-weight: bold; color: red; background-color: black;");
        ui->editEstado->setText("Desconectado");
        ui->buttonConexion->setText("Conectar");
    }
}


void MainWindow::EnviarComando(QString comando)
{

    QTime time = QTime::currentTime();
    QString time_text = time.toString("hh : mm : ss");
    QByteArray send = ("#"+comando + "$").toLatin1();
    puerto->write(send);
    ui->listEnvio->addItem("Datos Enviados : ["+send+"] Hora -> :"+"["+time_text+"]");
    ui->listEnvio->setCurrentRow(ui->listEnvio->count()-1);
    m_cant_bytes_enviados += send.size();
}

void MainWindow::InvertirLedRelay(int idx)
{
    m_estado_general.leds_relays[idx] = !m_estado_general.leds_relays[idx];
    EnviarComando("3" + QString::number(idx));
}

QString FijarAnchoString(QString str, int ancho)
{
    if (str.size() == ancho)
        return str;
    else if (str.size() > ancho)
        return str.mid(0, ancho);
    else
        while (str.size() < ancho)
            str += ' ';

    return str;
}

bool MainWindow::Conectado()
{
    return puerto && puerto->isOpen();

}

void MainWindow::NoConectadoError()
{
    if (!m_init)
        QMessageBox::warning(this, QString::fromUtf8("Error de conexión"), "Sin conexión");
}



void MainWindow::onDatosRecibidos()
{

    QByteArray bytes;
    int cant = puerto->bytesAvailable();
    bytes.resize(cant);
    puerto->read(bytes.data(), bytes.size());

    m_datos_recibidos += bytes;

    /*qDebug() << "bytes read:" << bytes.size();
    qDebug() << "bytes:" << bytes;*/
    qDebug() << "bytes recibidos:" << bytes;

//    ui->text_log->appendPlainText(bytes);
//    ui->text_log->moveCursor(QTextCursor::End);

//    ui->texto_log->appendPlainText("Datos Recibidos :"+bytes);
//    ui->texto_log->moveCursor(QTextCursor::End);
    m_cant_bytes_recibidos += cant;

    UpdateBytesTotales();

    ProcesarComandos();
}

void MainWindow::ProcesarComandos()
{
    QString comando="";
    QString depurado = "ATMCOIiohHt#$0123456789";

    for (int i = 0; i < m_datos_recibidos.count(); i++)
    {
        unsigned char dato = m_datos_recibidos[i];

        switch (dato)
        {

            case '$':
                if (comando != "")
                {
                    ProcesarComando(comando);
                    comando = "";
                }
                break;

            default:
            if(depurado.contains(dato))
                    comando += dato;
                break;
        }
    }

    m_datos_recibidos = comando.toLatin1();
}

void MainWindow::ProcesarComando(QString comando)
{
    float temp;
    QTime time = QTime::currentTime();
    QString time_text = time.toString("hh : mm : ss");
    ui->listErrores->addItem("Datos Recibidos : ["+comando+"]  Hora -> :" +" ["+time_text+"]");
    ui->listErrores->setCurrentRow(ui->listErrores->count()-1);

    unsigned char base = comando[0].toLatin1();

    if(base != '#'  ){
        //QMessageBox::warning(this, QString::fromUtf8("Error al comienzo de trama"), "No se respeta el protocolo");
        return;
    }
    base = comando[1].toLatin1();
    switch (base)
    {
    case TEMPERATURA:
        temp = comando.mid(2,-1).toFloat()/10;
        ui->lcdHumedadActual->setProperty("value", temp);
        break;
    case HUMEDAD:
        if((comando.mid(2,-1)).toInt() != 0)
        ui->lcdHumedad->setProperty("value", QVariant(comando.mid(2,-1)));
        break;
    case MODOMANUAL:
        ui->labelModoActual->setText("Manual");
        break;
    case MODOAUTOMATICO:
         ui->labelModoActual->setText("Automatico");
        break;
    case MODOTEMPORIZADO:
            ui->labelModoActual->setText("Temporizado");
        break;
    case BOTONOK:
        if(EstadoGeneral){
            ui->labelEstadoActual->setText("Prendido");
             EstadoGeneral=0;
        }
        else{
          ui->labelEstadoActual->setText("Apagado");
          EstadoGeneral=1;
        }
    case 'o':
        ui->labelEstadoActual->setText("Prendido");
        break;
    case 'i':
      ui->labelEstadoActual->setText("Apagado");
        break;
    case CONFIGURACION:
        break;
    case SENSOR1:
    {
          break;
    }
     case SENSOR2:
    {
         break;
    }

     case SENSOR3:
        break;
    default:  m_status_bytes_recibidos->setText("Comando no valido :"+base);
        break;
    }
}


void MainWindow::UpdateBytesTotales()
{
    m_status_bytes_recibidos->setText(QString::number(m_cant_bytes_recibidos) + " bytes recibidos, " + QString::number(m_cant_bytes_enviados) + " byte enviados.");
}




void MainWindow:: activarModoManual(){
     QString enviar;
     enviar+="M";
     EnviarComando(enviar);
     EstadoGeneral=MANUAL;
}

void MainWindow:: activarAutomatico(){
    QString enviar;
    enviar+="A";
    EnviarComando(enviar);
    EstadoGeneral=AUTOMATICO;
}

void MainWindow:: activarTemporal(){
    QString enviar;
    enviar+="T";
    EnviarComando(enviar);
    EstadoGeneral=TEMPORIZADO;
}

void MainWindow::EnumerarPuertos()
{
    ui->comboBoxPuertos->clear();
    puerto=NULL;
    QList<QSerialPortInfo> port= QSerialPortInfo::availablePorts();

    for(int i=0;i<port.size();i++)
        {
            ui->comboBoxPuertos->addItem(port.at(i).QSerialPortInfo::portName());
        }

}

void MainWindow::on_buttonConexion_clicked()
{

    QMessageBox msgBox;


    if(!puerto)
    {
            puerto = new QSerialPort(ui->comboBoxPuertos->currentText());
            puerto->setBaudRate(QSerialPort::Baud9600);
            puerto->setDataBits(QSerialPort::Data8);
            puerto->setFlowControl(QSerialPort::NoFlowControl);
            puerto->setStopBits(QSerialPort::OneStop);
            puerto->setParity(QSerialPort::NoParity);

            if(puerto->open(QIODevice::ReadWrite))//==true
            {
//                QMessageBox::question(this, QString::fromUtf8("Conexion con el sistema"), QString::fromUtf8("Estas seguro de acceder?"));
                connect(puerto, SIGNAL(readyRead()),this,SLOT(onDatosRecibidos()));
               if(confiPorSoft)
                     EnviarComando(f);
            }else
                   CerrarPuerto();

    }

   else
       CerrarPuerto();

     ActualizarEstadoConexion();

}




void MainWindow::errorDeMensaje()
{
    if (!m_init)
        QMessageBox::warning(this, QString::fromUtf8("Escribí un texto"), "Escribí un Comando válido");
}

void MainWindow::errorDeLongitudMensaje(QString  cant)
{
    if (!m_init)
        QMessageBox::warning(this, QString::fromUtf8("Escribí un texto"), "No puede superar los "+cant+" caracteres");
}
// mensajes al LCD








void MainWindow::on_pushButtonManual_clicked()
{
   if (Conectado()){

     //   if(EstadoGeneral==DEFAULT){
             activarModoManual();
             ui->labelModoActual->setText("MNAUAL");

       //      cantManual++;
       // }else if (cantManual==0 && EstadoGeneral==MANUAL){
         //           activarModoManual();
           //         PressTeclas(5);
       //}

    }
//      //  InvertirLedRelay(3);
    else
        NoConectadoError();

    UpdateBytesTotales();

}

void MainWindow::on_pushButtonTemporizado_clicked()
{
    if (Conectado()){
    //    if(EstadoGeneral==DEFAULT){

            activarTemporal();
              ui->labelModoActual->setText("Temporizado");
      //      cantTemporizado++;
        //}else if(cantTemporizado==0 && EstadoGeneral==TEMPORIZADO){
          //       activarTemporal();
            //     PressTeclas(6);
            //  }

    }

      //  InvertirLedRelay(3);
    else
        NoConectadoError();


    UpdateBytesTotales();
}

void MainWindow::on_pushButtonAutomatico_clicked()
{
    if (Conectado()){
     //   if(EstadoGeneral==DEFAULT){
            activarAutomatico();
             ui->labelModoActual->setText("Automatico");
       //     cantAutomatico++;
        //}else if (cantAutomatico ==0 && EstadoGeneral==AUTOMATICO){
          //  activarAutomatico();
           // PressTeclas(7);
        //}

    }
    else
        NoConectadoError();

    UpdateBytesTotales();
}



void::MainWindow::prenderEstado(){
     QString enviar;
     enviar+="I";
     EnviarComando(enviar);
}

void::MainWindow::apagarEstado(){
    QString enviar;
    enviar+="O";
    EnviarComando(enviar);
}
void MainWindow::on_pushButtonOk_clicked()
{
    if(Conectado())
    {
        EnviarComando("O");
        if(EstadoGeneral){
             ui->labelEstadoActual->setText("Prendido");
                EstadoGeneral=0;
        }
        else{
             ui->labelEstadoActual->setText("Apagado");
             EstadoGeneral=1;
         }
        /*switch(EstadoGeneral)
        {
            case MANUAL:
                if( estadoManual)
                {
                   prenderEstado();

                    estadoManual=false;

                }else{
                    apagarEstado();
                    estadoManual=true;
                }
                 TitilarTeclas(5);
                 cantManual=0;
            break;

            case TEMPORIZADO:
            if(estadoTemporal){
                prenderEstado();
                estadoTemporal=false;
            }else{
                    apagarEstado();
                    estadoTemporal=true;
            }
             TitilarTeclas(6);
             cantTemporizado=0;
            break;

            case AUTOMATICO:
            if(estadoAutomatico){
                prenderEstado();
                estadoAutomatico=false;
            }else{
                apagarEstado();
                estadoAutomatico=true;
            }
             TitilarTeclas(7);
             cantAutomatico=0;
            break;

            default:
            break;
        }
        EstadoGeneral=DEFAULT;*/
    }else{
        NoConectadoError();
    }
        UpdateBytesTotales();

}

void MainWindow::on_buttonActualizar_clicked()
{
    EnumerarPuertos();
}



void MainWindow::on_pushButtonEnviarConfiguracion_clicked()
{

    if(Conectado()){
        QString enviarConfig;//,send;
//         enviarConfig+=ui->spinBoxHumMi->text();
//        enviarConfig+=ui->timeEditTiempoRiego->text();
//        enviarConfig+=ui->timeEditHoraRie->text();
//        enviarConfig+=ui->timeEditTiempoRiego->text();
//        enviarConfig+=("C"+enviarConfig);
//        EnviarComando(enviarConfig);

        if( ui->spinBoxHumMa->value() <10)
        {
            enviarConfig += ('0' + ui->spinBoxHumMa->text());
        }
        else
        {
            enviarConfig += ui->spinBoxHumMa->text();
        }
        if(ui->spinBoxHumMi->value() < 10)
        {
             enviarConfig+= ('0' + ui->spinBoxHumMi->text());
        }
        else
        {
            enviarConfig += ui->spinBoxHumMi->text();
        }

        enviarConfig+=ui->timeEditTiempoRiego->text();
         enviarConfig+=ui->timeEditHoraRie->text();
        enviarConfig=("C"+enviarConfig);
        EnviarComando(enviarConfig);


    }else{

        NoConectadoError();
    }

}

void MainWindow::on_button_tecla_2_clicked()
{
    if(Conectado()){
        enviarRTC();
    }else
        NoConectadoError();
}
