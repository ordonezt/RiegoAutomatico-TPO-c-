#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include <Qtimer>
#include <QPixmap>
#include <QQueue>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QList>
#include <QMap>
#include <QPointer>
#include <QVector>
#include <QDateTime>
#include "puertoserie.h"

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
//

namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
	double Temperatura_y, Humedad_y, m_Lluvia;
	QTime time;
	QTimer *timer, *timer_plot;
	QString time_text;
	QLabel *bytesStatus;
	bool m_init;
	void estadoInicial();
	void ejecutarTimer();
	void iniciarGrafico();
	void enviarRTC();
	void PrepararGrafico();
	void PrenderLluvia();
	void ApagarLluvia();
	void DesconectarLluvia();

public:
	void mostrarMarcador();
	void UpdateTabs();
	void NoConectadoError();
	void actualizarPlot();

private slots:
	void mostrarReloj();
	void on_buttonConexion_clicked();
	void on_pushButtonManual_clicked();
	void on_pushButtonTemporizado_clicked();
	void on_pushButtonAutomatico_clicked();
	void on_pushButtonOk_clicked();
	void on_buttonActualizar_clicked();
	void on_pushButtonEnviarConfiguracion_clicked();
	void realtimePlot(void);

	void on_button_EnviarHora_clicked();

	public slots:
	void ProcesarComando(QString comando);
	void ActualizarEstadoConexion();

private:
	Ui::MainWindow *ui;
	PuertoSerie puerto;

};

#endif // MAINWINDOW_H
