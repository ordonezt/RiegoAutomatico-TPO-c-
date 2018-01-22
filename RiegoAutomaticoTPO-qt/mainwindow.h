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
#define DEFAULT     '5'
#define MANUAL      '1'
#define TEMPORIZADO '2'
#define AUTOMATICO  '3'
#define TEMPERATURA 't'
#define HUMEDAD     'h'
#define RECIBIDO     'W'
#define SENSOR2     'Y'
#define SENSOR3     'Z'
#define RTC         'C'
#define MODOMANUAL  'M'
#define MODOAUTOMATICO  'A'
#define MODOTEMPORIZADO 'T'
#define CONFIGURACION   'C'
#define BOTONOK         'O'

#define MENSAJE_LLUVIA_ON	'L'
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
     int m_cant_bytes_recibidos;
     int m_cant_bytes_enviados;
     int cantAutomatico;
     int cantManual;
     int cantTemporizado;
     int EstadoGeneral;
     bool estadoManual;
     bool estadoTemporal;
     bool estadoAutomatico;
     void prenderEstado();
     void apagarEstado();
     void activarModoManual();
     void activarModoTemporizado();
     void activarModoAutomatico();
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
    enum ColorRGB
    {
        RGB_R, RGB_G, RGB_B
    };

    struct EstadoGeneral
    {
        EstadoGeneral()
        {
            expansion = 3;
            for (int i = 0; i < 8; i++)
            {
                if (i < 2)
                    inputs[i] = false;
                if (i < 4)
                    leds_relays[i] = false;

                keys[i] = false;
                leds[i] = false;
                DIPinputs[i] = false;
            }

            seg7[0] = 123;
            seg7[1] = 456;

            adc = 0;
            dac = 0;
            rgb_r = false;
            rgb_g = false;
            rgb_b = false;
            buzzer = false;

            lcd[0] = "SISTEMA DE RIEGO";
            lcd[1] = "    AUTOMATICO";

            dotmatrix_text = "INFO";
            dotmatrix_rotation = 4;

            expansion = 0;
        }
        int expansion;
        bool leds_relays[4];
        bool rgb_r;
        bool rgb_g;
        bool rgb_b;
        bool buzzer;
        bool DIPinputs[8];
        bool inputs[2];
        int seg7[2];
        QString dotmatrix_text;
        int dotmatrix_rotation;
        QString lcd[2];
        bool keys[8];
        int adc;
        int dac;
        bool leds[8];
        QDateTime rtc;
    } m_estado_general;

    bool m_init;
      QVector<double> x_x1, y_y1;
      QVector<double> x_x2, y_y2;
      QVector<double> x_x3, y_y3;
      QVector<double> x_x4, y_y4;

public:
    void errorDeMensaje();
    void errorDeLongitudMensaje(QString cant);
    void EnumerarPuertos();
    void ActualizarEstadoConexion();
    void mostrarMarcador();

    void CerrarPuerto();

    bool Conectado();
    void NoConectadoError();
    void UpdateBytesTotales();

    void UpdateLedRelay(int idx);
    void UpdateLedRelayEstado(QPushButton* relay, bool m_estado);
    void InvertirLedRelay(int idx);

    void EnviarComando(QString comando);

    void PressTeclado(int idx, int corto = true);
    void PressTeclas(int idx, int corto = true);
    void UpdateTabs();

    void iniciarSensor3();
    void iniciarSensor2();
    void actualizarPlot();
    void actualizarGraficos();
    void actualizarPuntos(double x, double y, double xex, double yex);
    QQueue<int> m_teclado_cola;
    QQueue<int> m_teclas_cola;
    QByteArray m_datos_recibidos;

    void Parceo();
    void ProcesarComando(QString comando);
    void TitilarTeclado(int idx, int corto = true);
    void TitilarTeclas(int idx, int corto = true);


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
