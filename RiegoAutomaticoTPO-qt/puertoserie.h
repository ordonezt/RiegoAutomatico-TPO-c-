#ifndef PUERTOSERIE_H
#define PUERTOSERIE_H
#include <QSerialPort>


class PuertoSerie : public QSerialPort
{
	public:
		PuertoSerie();
		~PuertoSerie();
		void Parseo();
		static bool Conectado();
		void EnviarComando(QString comando);
		void ProcesarComando(QString comando);
		static void NoConectadoError();
		void UpdateBytesTotales();
	private:

};

#endif // PUERTOSERIE_H
