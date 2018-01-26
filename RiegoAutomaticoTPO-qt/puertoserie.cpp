#include "puertoserie.h"

PuertoSerie::PuertoSerie()
{


}
/**
		\fn  onDatosRecibidos
		\brief Slot llamado con la señal readyRead cuando hay datos disponibles del puerto serie.
*/
void PuertoSerie::onDatosRecibidos()
{
		QByteArray bytes;
		int cant = this->bytesAvailable();
		bytes.resize(cant);
		this->read(bytes.data(),bytes.length());

		m_datos_recibidos += bytes;

		qDebug() << "bytes recibidos:" << bytes;

		m_cant_bytes_recibidos += cant;

		UpdateBytesTotales();

		Parseo();
}
/**
		\fn  Parseo
		\brief Parsea los datos recibidos del puerto serie
*/
void PuertoSerie::Parseo()
{
		QString comando="";
		QString depurado = "ATMCOIERWLliohHt#$0123456789";

		for (int i = 0; i < m_datos_recibidos.count(); i++)
		{
				unsigned char dato = m_datos_recibidos[i];

				switch (dato)
				{

						case '$':
								if (comando != "")
								{
										qDebug() << "Trama interpretada";
										ProcesarComando(comando);
										comando = "";

										//Reset del timer de timeout
										timerTimeOutSerie->start();
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
/**
		\fn  ProcesarComando
		\brief Procesa el comando recibido
*/
void PuertoSerie::ProcesarComando(QString comando)
{
		QTime time = QTime::currentTime();
		QString time_text = time.toString("hh:mm:ss");

		unsigned char base = comando[0].toLatin1();

		if(base != '#')
		{
				//QMessageBox::warning(this, QString::fromUtf8("Error al comienzo de trama"), "No se respeta el protocolo");
				return;
		}

		base = comando[1].toLatin1();

		switch (base)
		{
				case TEMPERATURA:
						Temperatura_y = comando.mid(2,-1).toFloat()/10;
						ui->lcdTemperatura->setProperty("value", Temperatura_y);
						break;

				case HUMEDAD:
						Humedad_y = (comando.mid(2,-1)).toInt();
						ui->lcdHumedad->setProperty("value", Humedad_y);
						break;

				case MODOMANUAL:
						ui->labelModoActual->setText("Manual");
						ui->labelModoActual->setStyleSheet("QLabel { font-weight: bold; color : blue; }");

						ui->listRecibido->addItem( QString::fromUtf8("Se cambio al modo Manual a las ") + time_text + "hs." );
						//ui->listRecibido->setCurrentRow(ui->listRecibido->count()-1);
						break;

				case MODOAUTOMATICO:
						ui->labelModoActual->setText("Automatico");
						ui->labelModoActual->setStyleSheet("QLabel { font-weight: bold; color : red; }");

						ui->listRecibido->addItem( QString::fromUtf8("Se cambio al modo Automatico a las ") + time_text + "hs." );
						//ui->listRecibido->setCurrentRow(ui->listRecibido->count()-1);
						break;

				case MODOTEMPORIZADO:
						ui->labelModoActual->setText("Temporizado");
						ui->labelModoActual->setStyleSheet("QLabel { font-weight: bold; color : yellow; }");

						ui->listRecibido->addItem( QString::fromUtf8("Se cambio al modo Temporizado a las ") + time_text + "hs." );
						//ui->listRecibido->setCurrentRow(ui->listRecibido->count()-1);
						break;

				case 'o':
						ui->labelEstadoActual->setText("Apagado");
						ui->labelEstadoActual->setStyleSheet("QLabel { font-weight: bold; color : red; }");

						ui->listRecibido->addItem( QString::fromUtf8("Se apago el riego a las ") + time_text + "hs." );
						//ui->listRecibido->setCurrentRow(ui->listRecibido->count()-1);
						break;

				case 'i':
						ui->labelEstadoActual->setText("Prendido");
						ui->labelEstadoActual->setStyleSheet("QLabel { font-weight: bold; color : green; }");

						ui->listRecibido->addItem( QString::fromUtf8("Se encendio el riego a las ") + time_text + "hs." );
						break;

				case CONFIGURACION:
						ui->labelModoActual->setText("Configuracion");
						ui->labelModoActual->setStyleSheet("QLabel { font-weight: bold; color : green; }");
						break;

				case RECIBIDO:

							break;

				case MENSAJE_LLUVIA_ON:
						PrenderLluvia();
						break;

				case MENSAJE_LLUVIA_OFF:
						ApagarLluvia();
						break;

				default:
						m_status_bytes_recibidos->setText("Comando no valido :"+base);
						break;
		}

		ui->listRecibido->setCurrentRow(ui->listRecibido->count()-1);
}

/**
		\fn  EnviarComando
		\brief Recive una string, le agrega el inicio y el fin de trama y la envia por puerto serie
		\param [in] QString comando: String a enviar
*/
void PuertoSerie::EnviarComando(QString comando)
{
		QByteArray send = ("#" + comando + "$").toLatin1();
		puerto->write(send);

		QTime time = QTime::currentTime();
		QString time_text = time.toString("hh : mm : ss");

		switch( comando[0].unicode() )
		{
				case 'M':
						ui->listEnviado->addItem(QString("Se envio el comando Manual a las ") + time_text + "hs.");
						break;
				case 'T':
						ui->listEnviado->addItem(QString("Se envio el comando Temporizado a las ") + time_text + "hs.");
						break;
				case 'A':
						ui->listEnviado->addItem(QString("Se envio el comando Automatico a las ") + time_text + "hs.");
						break;
				case 'O':
						ui->listEnviado->addItem(QString("Se envio el comando Ok a las ") + time_text + "hs.");
						break;
				case 'S':
						ui->listEnviado->addItem(QString("Se envio el comando Status a las ") + time_text + "hs.");
						break;
				default:
						break;
		}

		ui->listEnviado->setCurrentRow(ui->listEnviado->count()-1);
		m_cant_bytes_enviados += send.size();
}

/**
		\fn  Conectado
		\brief Informa si el puerto esta conectado o desconectado
*/
bool PuertoSerie::Conectado()
{
		return puerto && puerto->isOpen();
}

/**
		\fn  UpdateBytesTotales
		\brief Muestra la cantidad de bytes enviados y recividos
*/
QString PuertoSerie::UpdateBytesTotales()
{
		statusBytes = QString::number(cantBytesRecibidos) + " bytes recibidos, " + QString::number(cantBytesEnviados) + " byte enviados.";
}

/**
		\fn  EnumerarPuertos
		\brief Enumera los puertos disponibles en un comboBox
*/
void PuertoSerie::EnumerarPuertos()
{
		ui->comboBoxPuertos->clear();
		puerto=NULL;
		QList<QSerialPortInfo> port= QSerialPortInfo::availablePorts();

		for(int i=0;i<port.size();i++)
				ui->comboBoxPuertos->addItem(port.at(i).QSerialPortInfo::portName());
}

/**
 * \fn IniciarConexion
 * \brief Inicia la conexion del puerto
 */
bool PuertoSerie::IniciarConexion(QString Puerto)
{
	this->setPortName(Puerto);
	this->setBaudRate(QSerialPort::Baud9600);
	this->setDataBits(QSerialPort::Data8);
	this->setFlowControl(QSerialPort::NoFlowControl);
	this->setStopBits(QSerialPort::OneStop);
	this->setParity(QSerialPort::NoParity);

	if(this->open(QIODevice::ReadWrite))
			connect(this, SIGNAL(readyRead()),SLOT(onDatosRecibidos()));
	else
			CerrarPuerto();
}

/**
		\fn  TerminarConexion
		\brief Cierra la conexion con el Puerto
*/
void PuertoSerie::TerminarConexion()
{
	this->close();
}

/**
		\fn  CerrarPuertoSerie
		\brief slot que cierra el puerto y actualiza la interfaz
*/
void PuertoSerie::TimeoutPuertoSerie( void )
{
		CerrarPuerto();
		ActualizarEstadoConexion();
}
