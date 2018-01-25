#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
# include <Qtimer>
# include <QPixmap>
#include <QQueue>
#include  "qserialport.h"
#include <QSerialPortInfo>
#include <QSerialPort>
#include "qserialportinfo.h"
#include <Configkit.h>
#include <QList>
#include <QMap>
#include <QPointer>
#include <QVector>
#include <QDateTime>
#define DEFAULT					'5'
#define MANUAL					'1'
#define TEMPORIZADO			'2'
#define AUTOMATICO			'3'
#define TEMPERATURA			't'
#define HUMEDAD					'h'
#define RECIBIDO				'W'
#define SENSOR2					'Y'
#define SENSOR3					'Z'
#define RTC							'C'
#define MODOMANUAL			'M'
#define MODOAUTOMATICO	'A'
#define MODOTEMPORIZADO	'T'
#define CONFIGURACION		'C'
#define BOTONOK					'O'

#define MENSAJE_LLUVIA_ON		'L'
#define	MENSAJE_LLUVIA_OFF	'l'

#define LLUVIA_ON   100
#define LLUVIA_OFF  -1

#define TIEMPOPLOT   1000  //Tiempo de refresco del grafico
#define TIMEOUTSERIE 4000 //Tiempo de timeout del tiempo serie en ms
//
extern QByteArray f;
extern bool confiPorSoft;
extern bool confiPorSf;
namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:

     QSerialPort *puerto;

     double Temperatura_y, Humedad_y, m_Lluvia;

     QTime time;
     QTimer *timer, *timer_plot, *timerTimeOutSerie;
     QString time_text;
     QLabel *m_status_bytes_recibidos;
		 QByteArray m_datos_recibidos;
     int m_cant_bytes_recibidos;
		 int m_cant_bytes_enviados;
		 bool m_init;
     void estadoInicial();
     void ejecutarTimer();
     void iniciarGrafico();
     void enviarRTC();
     void PrepararGrafico();
     void PrepararTimeOutSerie();
     void PrenderLluvia();
     void ApagarLluvia();
     void DesconectarLluvia();

public:
		void EnumerarPuertos();
		void ActualizarEstadoConexion();
    void mostrarMarcador();

    void CerrarPuerto();

    void UpdateTabs();

		void actualizarPlot();

private slots:
    void configActive();
    void onDatosRecibidos();
    void mostrarReloj();
    void on_buttonConexion_clicked();
    void on_pushButtonManual_clicked();
    void on_pushButtonTemporizado_clicked();
    void on_pushButtonAutomatico_clicked();
    void on_pushButtonOk_clicked();
    void on_buttonActualizar_clicked();

    void on_pushButtonEnviarConfiguracion_clicked();

    void on_button_tecla_2_clicked();

    void CerrarPuertoSerie( void );

    void realtimePlot(void);

private:
    Ui::MainWindow *ui;
signals:

};

#endif // MAINWINDOW_H
