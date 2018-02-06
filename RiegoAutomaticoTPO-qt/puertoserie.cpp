#include "puertoserie.h"

PuertoSerie::PuertoSerie() : QSerialPort(), timerTimeOutSerie()
{
	//Conecto un timer de timeout para el puerto serie. Se dispara cuando se conecta el puerto
    connect(&timerTimeOutSerie, &QTimer::timeout, this, &PuertoSerie::TimeoutPuertoSerie);
    connect(this, SIGNAL(readyRead()),SLOT(onDatosRecibidos()));
}

PuertoSerie::~PuertoSerie()
{

}
/**
		\fn  onDatosRecibidos
		\brief Slot llamado con la señal readyRead cuando hay datos disponibles del puerto serie.
*/
void PuertoSerie::onDatosRecibidos()
{
    if(this->bytesAvailable() != 0)
    {
        m_datos_recibidos = this->readLine();

        qDebug() << "bytes recibidos:" << m_datos_recibidos;

        cantBytesRecibidos += m_datos_recibidos.length();

        UpdateBytesTotales();

        Parseo();
    }
}

/**
		\fn  Parseo
		\brief Parsea los datos recibidos del puerto serie
*/
void PuertoSerie::Parseo()
{
	QString comando="";
	QString depurado = "ATMCcOIERWLlRiohHt#$0123456789";

	for (int i = 0; i < m_datos_recibidos.count(); i++)
	{
		unsigned char dato = m_datos_recibidos[i];

		switch (dato)
		{
			case '$':
				if (comando != "")
				{
					qDebug() << "Trama interpretada";
					emit ComandoProcesado(comando);
                    comando = "";
					//Reset del timer de timeout
					timerTimeOutSerie.start();
				}
				break;
			default:
				if(depurado.contains(dato))
					comando += dato;
				break;
		}
	}
}

/**
		\fn  EnviarComando
		\brief Recive una string, le agrega el inicio y el fin de trama y la envia por puerto serie
		\param [in] QString comando: String a enviar
*/
void PuertoSerie::EnviarComando(QString comando)
{
	QByteArray send = ("#" + comando + "$").toLatin1();
	this->write(send);
	cantBytesRecibidos += send.size();
}

/**
		\fn  Conectado
		\brief Informa si el puerto esta conectado o desconectado
*/
bool PuertoSerie::Conectado()
{
	bool status = this->isOpen();

	return status;
}

/**
		\fn  UpdateBytesTotales
		\brief Muestra la cantidad de bytes enviados y recividos
*/
QString PuertoSerie::UpdateBytesTotales()
{
	return (QString::number(cantBytesRecibidos) + " bytes recibidos, " + QString::number(cantBytesEnviados) + " byte enviados.");
}

/**
		\fn  EnumerarPuertos
		\brief Enumera los puertos disponibles en un comboBox
*/
QList<QString> PuertoSerie::EnumerarPuertos()
{
	QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();
	QList<QString> portNameList;

	for(int i=0;i<portList.size();i++)
		portNameList.append(portList.at(i).QSerialPortInfo::portName());

	return portNameList;
}

/**
 * \fn IniciarConexion
 * \brief Inicia la conexion del puerto
 */
bool PuertoSerie::IniciarConexion(QString Puerto)
{
	bool success = false;

	this->setPortName(Puerto);
	this->setBaudRate(QSerialPort::Baud9600);
	this->setDataBits(QSerialPort::Data8);
	this->setFlowControl(QSerialPort::NoFlowControl);
	this->setStopBits(QSerialPort::OneStop);
	this->setParity(QSerialPort::NoParity);

	if(this->open(QIODevice::ReadWrite))
    {
		success = true;

		//Inicio el timer
		timerTimeOutSerie.start( TIMEOUTSERIE );
	}
	else
		TerminarConexion();


	return success;
}

/**
		\fn  TimeoutPuertoSerie
		\brief slot que cierra el puerto y actualiza la interfaz
*/
void PuertoSerie::TimeoutPuertoSerie()
{
	TerminarConexion();
	emit DesconexionTimeout();
}

/**
		\fn  TerminarConexion
		\brief Cierra la conexion con el Puerto
*/
void PuertoSerie::TerminarConexion()
{
    timerTimeOutSerie.stop();
	this->close();
}
