#ifndef PUERTOSERIE_H
#define PUERTOSERIE_H
#include <QSerialPort>
#include <QTime>

class PuertoSerie : public QSerialPort
{
	public:
		PuertoSerie();
		~PuertoSerie();
		void Parseo();
		static bool Conectado();
		void EnviarComando(QString comando);
		void ProcesarComando(QString comando);
		void EnumerarPuertos(void);
		void TerminarConexion(void);
		bool IniciarConexion(QString Puerto);
		void UpdateBytesTotales();

	private:
		QByteArray m_datos_recibidos;
		int m_cant_bytes_recibidos;
		int m_cant_bytes_enviados;
		enum Comandos
		{
			DEFAULT = '5',
			MANUAL = '1',
			TEMPORIZADO,
			AUTOMATICO,
			TEMPERATURA = 't',
			HUMEDAD = 'h',
			RECIBIDO = 'W',
			SENSOR2 = 'Y'
		}Comando;

	private slots:
		void onDatosRecibidos();
	public slots:
		void TimeoutPuertoSerie();

};

#endif // PUERTOSERIE_H


