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
		int cant = puerto->bytesAvailable();
		bytes.resize(cant);
		puerto->read(bytes.data(), bytes.size());

		m_datos_recibidos += bytes;

		qDebug() << "bytes recibidos:" << bytes;

		m_cant_bytes_recibidos += cant;

		UpdateBytesTotales();

		Parseo();
}
/**
		\fn  Parceo
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
		\fn  Conectado
		\brief Informa si el puerto esta conectado o desconectado
*/
bool PuertoSerie::Conectado()
{
		return puerto && puerto->isOpen();
}
/**
		\fn  NoConectadoError
		\brief Funcion que informa que el puerto no esta conectado
*/
void PuertoSerie::NoConectadoError()
{
		if (!m_init)
				QMessageBox::warning(this, QString::fromUtf8("Error de conexión"), "Sin conexión");
}
