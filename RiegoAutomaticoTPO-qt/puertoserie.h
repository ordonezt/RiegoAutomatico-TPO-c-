#ifndef PUERTOSERIE_H
#define PUERTOSERIE_H
#include <QTime>
#include <QTimer>
#include <QDebug>
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>


class PuertoSerie : public QSerialPort
{
	Q_OBJECT
public:
	PuertoSerie();
	virtual ~PuertoSerie();
	void Parseo();
	bool Conectado();
	void EnviarComando(QString comando);
	QList<QString> EnumerarPuertos(void);
	void TerminarConexion(void);
	bool IniciarConexion(QString Puerto);
	QString UpdateBytesTotales();

private:
	QByteArray m_datos_recibidos;
	int cantBytesEnviados;
	int cantBytesRecibidos;
	const int TIMEOUTSERIE = 4000;
	QTimer timerTimeOutSerie;
//		enum Comandos
//		{
//			DEFAULT = '5',
//			MANUAL = '1',
//			TEMPORIZADO,
//			AUTOMATICO,
//			TEMPERATURA = 't',
//			HUMEDAD = 'h',
//			RECIBIDO = 'W',
//			SENSOR2 = 'Y'
//		}Comando;

private slots:
	void onDatosRecibidos();
	void TimeoutPuertoSerie();

signals:
	void ComandoProcesado(QString comando);
	void DesconexionTimeout();
};

#endif // PUERTOSERIE_H


