#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "configkit.h"

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
 /**
     \fn  Nombre de la Funcion
     \brief Descripcion
     \param [in] parametros de entrada
     \param [out] parametros de salida
     \return tipo y descripcion de retorno
 */
 /**
     \fn  Constructor MainWindow
     \brief Inicializa todo lo necesario para el funcionamiento del programa
 */
MainWindow::MainWindow(QWidget *parent) :
		QMainWindow(parent),
		ui(new Ui::MainWindow)
{
    m_init = true;
    ui->setupUi(this);

    timer_plot = NULL;
    timerTimeOutSerie = NULL;

    PrepararTimeOutSerie();
    mostrarMarcador();
    estadoInicial();
    ejecutarTimer();
    UpdateTabs();
    EnumerarPuertos();
    ActualizarEstadoConexion();
		puerto.UpdateBytesTotales();
    m_init = false;
    Temperatura_y = 0;
    Humedad_y = 0;
    PrepararGrafico();
}
/**
    \fn  Destructor de MainWindow
    \brief libera la memoria asignada dinamicamente
*/
MainWindow::~MainWindow()
{
    delete ui;
}
/**
    \fn  ejecutarTimer
    \brief Le da start a un timer encargado de muestrear la hora
*/
void::MainWindow:: ejecutarTimer()
{
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this, SLOT(mostrarReloj()));
    timer->start();
}
/**
    \fn  Nombre de la Funcion
    \brief Descripcion
    \param [in] parametros de entrada
    \param [out] parametros de salida
    \return tipo y descripcion de retorno
*/
void MainWindow::configActive()
{
    QMessageBox::warning(this, QString::fromUtf8("config"),f);      //Creo q no se usa
}
/**
    \fn   estadoInicial
    \brief Inicializa las variables a sus valores de reset
*/
void::MainWindow:: estadoInicial()
{
	confiPorSf=false;
	bytesStatus = new QLabel(this);
	ui->statusBar->addWidget(bytesStatus);
}
/**
    \fn  mostrarMarcador
    \brief Inicializa los displays encargados de mostrar la humedad y temperatura
*/
void MainWindow::mostrarMarcador()
{
    ui->lcdHumedad->setStyleSheet("color: lightgreen; background-color: black; border: 2px solid lightgreen;");
    ui->lcdHumedad->setFrameShape(QFrame::Box);
    ui->lcdHumedad->setFrameShadow(QFrame::Raised);
    ui->lcdHumedad->setMidLineWidth(0);
    ui->lcdHumedad->setSmallDecimalPoint(true);
    ui->lcdHumedad->setDigitCount(4);
    ui->lcdHumedad->setMode(QLCDNumber::Dec);
    ui->lcdHumedad->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdHumedad->setProperty("value", QVariant(24.3));

    ui->lcdTemperatura->setStyleSheet("color: lightgreen; background-color: black; border: 2px solid lightgreen;");
    ui->lcdTemperatura->setFrameShape(QFrame::Box);
    ui->lcdTemperatura->setFrameShadow(QFrame::Raised);
    ui->lcdTemperatura->setMidLineWidth(0);
    ui->lcdTemperatura->setSmallDecimalPoint(true);
    ui->lcdTemperatura->setDigitCount(4);
    ui->lcdTemperatura->setMode(QLCDNumber::Dec);
    ui->lcdTemperatura->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdTemperatura->setProperty("value", QVariant(24.3));
}
/**
    \fn  mostrarReloj
    \brief Muestra la hora actual en formato HHMMSS
*/
void MainWindow::mostrarReloj()
{
   time = QTime::currentTime();
	 time_text = time.toString("hh:mm:ss");
   ui->Hora->setText(time_text);

	 if(puerto.Conectado())
       if(confiPorSoft)
       {
           QMessageBox::warning(this, QString::fromUtf8("config"),f);
           confiPorSoft=false;
       }
}
/**
    \fn  enviarRTC
    \brief Envia la hora actual por el puerto serie
*/
void MainWindow::enviarRTC()
{
    QTime   time_aux = QTime::currentTime();
    QString  time_text_aux = time_aux.toString("hhmmss");
		puerto.EnviarComando("R" + time_text_aux);
}
/**
    \fn  UpdateTabs
    \brief Habilita ambos tabs
*/
void MainWindow::UpdateTabs()
{
    ui->tabWidget->setTabEnabled(0, true);
    ui->tabWidget->setTabEnabled(1, true);
}

/**
    \fn  ActualizarEstadoConexion
    \brief Actualiza la interfaz para mostrar que se esta conectado o no
*/
void MainWindow::ActualizarEstadoConexion()
{
		if (puerto.Conectado())
    {
        ui->editEstado->setStyleSheet("font-weight: bold; color: green; background-color: black;");
        ui->editEstado->setText("Conectado");
        ui->buttonConexion->setText("Desconectar");
    }
    else
    {
        ui->editEstado->setStyleSheet("font-weight: bold; color: red; background-color: black;");
        ui->editEstado->setText("Desconectado");
        ui->buttonConexion->setText("Conectar");

        DesconectarLluvia();
        ui->labelModoActual->setText("Desconectado");
        ui->labelModoActual->setStyleSheet("QLabel { font-weight: bold; color : black; }");

        ui->labelEstadoActual->setText("Desconectado");
        ui->labelEstadoActual->setStyleSheet("QLabel { font-weight: bold; color : black; }");

        ui->lcdHumedad->setProperty("value", -1);
        ui->lcdTemperatura->setProperty("value", -1);

        //Detengo el timer para graficar en tiempo real
        if(timer_plot!= NULL)
            timer_plot->stop();
        //Detengo el timer de timeout
        if( timerTimeOutSerie!= NULL)
            timerTimeOutSerie->stop();
    }
}


/**
		\fn  NoConectadoError
		\brief Funcion que informa que el puerto no esta conectado
*/
void MainWindow::NoConectadoError()
{
		if (!m_init)
				QMessageBox::warning(this, QString::fromUtf8("Error de conexión"), "Sin conexión");
}

/**
    \fn  FijarAnchoString
    \brief Le da un tamaño determinado a la string
    \param [in] QString str: string a fijar
    \param [in] int tam: cantidad de elementos deseado
    \return QString str: string con "tam" elementos
*/
QString FijarAnchoString(QString str, int tam)
{
    if (str.size() == tam)
        return str;
    else if (str.size() > tam)
        return str.mid(0, tam);
    else
        while (str.size() < tam)
            str += ' ';

    return str;
}

/**
    \fn  on_buttonConexion_clicked
    \brief Slot de clickeado del boton Conexion. Conecta el puerto, inicia los timers y cambia la interfaz
*/
void MainWindow::on_buttonConexion_clicked()
{
		if(puerto.IniciarConexion(ui->comboBoxPuertos->currentText()))
		{
			//Inicio un timer para graficar en tiempo real
			timer_plot->start( TIEMPOPLOT );

			//Inicio el timer
			timerTimeOutSerie->start( TIMEOUTSERIE );

			puerto.EnviarComando( "S" );   //Envio comando status para saber el estado actual de la maquina

		 if(confiPorSoft)
					 puerto.EnviarComando(f);
		}
		else
    {
       timerTimeOutSerie->stop();
			 puerto.TerminarConexion();
    }
     ActualizarEstadoConexion();
}

/**
    \fn  on_pushButtonManual_clicked
    \brief slot que se encarga de cambiar el estado a manual
*/
void MainWindow::on_pushButtonManual_clicked()
{
	 if (puerto.Conectado())
			 puerto.EnviarComando("M");
   else
			 NoConectadoError();

		puerto.UpdateBytesTotales();
}

/**
		\fn  on_pushButtonTemporizado_clicked
    \brief slot que se encarga de cambiar el estado a temporizado
*/
void MainWindow::on_pushButtonTemporizado_clicked()
{
		if (puerto.Conectado())
				puerto.EnviarComando("T");
    else
				NoConectadoError();

		puerto.UpdateBytesTotales();
}

/**
    \fn  on_pushButtonManual_clicked
    \brief slot que se encarga de cambiar el estado a automatico
*/
void MainWindow::on_pushButtonAutomatico_clicked()
{
		if (puerto.Conectado())
				puerto.EnviarComando("A");
    else
        NoConectadoError();

		puerto.UpdateBytesTotales();
}


/**
    \fn  on_pushButtonOk_clicked
    \brief slot que envia el comando Ok
*/
void MainWindow::on_pushButtonOk_clicked()
{
		if(puerto.Conectado())
				puerto.EnviarComando("O");
    else
        NoConectadoError();

		puerto.UpdateBytesTotales();
}

/**
    \fn  on_buttonActualizar_clicked
    \brief slot que actualiza el comboBox de los puertos disponibles
*/
void MainWindow::on_buttonActualizar_clicked()
{
    EnumerarPuertos();
}

/**
    \fn  on_pushButtonEnviarConfiguracion_clicked
    \brief Descripcion
*/
void MainWindow::on_pushButtonEnviarConfiguracion_clicked()
{
		if(puerto.Conectado())
    {
        QString enviarConfig;

				if( ui->spinBoxHumMa->value() < 10)
            enviarConfig += ('0' + ui->spinBoxHumMa->text());
        else
            enviarConfig += ui->spinBoxHumMa->text();

				if( ui->spinBoxHumMi->value() < 10)
             enviarConfig+= ('0' + ui->spinBoxHumMi->text());
        else
            enviarConfig += ui->spinBoxHumMi->text();

        enviarConfig+=ui->timeEditTiempoRiego->text();
				enviarConfig+=ui->timeEditHoraRiego->text();
        enviarConfig=("C"+enviarConfig);
				puerto.EnviarComando(enviarConfig);
    }
    else
        NoConectadoError();
}

/**
    \fn  on_button_tecla_2_clicked
    \brief slot que envia la hora actual por puerto serie
*/
void MainWindow::on_button_tecla_2_clicked()
{
		if(puerto.Conectado())
        enviarRTC();
    else
        NoConectadoError();
}

/**
    \fn  PrepararGrafico
    \brief Inicializa todo lo necesario para el grafico
*/
void MainWindow::PrepararGrafico()
{
    //Hacemos visible la etiqueta de referencia y seteamos la fuente a "Helvetica" con un tamaño de 22 en la esquina superior izquierda:
    ui->customPlot->legend->setVisible( true ); //La hacemos visible
    ui->customPlot->legend->setFont( QFont( "Helvetica" , 12 ) );   //Seteamos la fuente
    ui->customPlot->axisRect()->insetLayout()->setInsetAlignment( 0 , Qt::AlignLeft|Qt::AlignTop ); //La ubicamos

    //Seteamos idioma y pais para tener puntos decimales correctos:
    ui->customPlot->setLocale( QLocale( QLocale::Spanish , QLocale::Argentina ) );

    //Habilitamos que se pueda arrastrar el grafico solo en la componente horizontal:
    ui->customPlot->setInteraction( QCP::iRangeDrag , true );   //Habilitamos arrastrar
    ui->customPlot->axisRect()->setRangeDrag( Qt::Horizontal ); //Solo en direccion horizontal

    //Creamos un nuevo ticker con formato de tiempo para asignar al eje x:
    QSharedPointer<QCPAxisTickerTime> timeTicker( new QCPAxisTickerTime );    //Lo creamos
    timeTicker->setTimeFormat( "%h:%m:%s" );  //Le damos formato
    timeTicker->setTickCount( 6 );    //Seteamos la cantidad de subticks entre cada tick
    timeTicker->setTickStepStrategy( QCPAxisTicker::tssMeetTickCount ); //Seteamos la estrategia de los ticks para que se ajusten a la cantidad de subticks deseado
    ui->customPlot->xAxis->setTicker( timeTicker );
    //ui->customPlot->axisRect()->setupFullAxesBox();

    //Creamos el primer grafico (temperatura):
    ui->customPlot->addGraph( ui->customPlot->xAxis , ui->customPlot->yAxis ); //Asignamos los ejes x(inferior) e y(izquierdo).
    ui->customPlot->graph(0)->setPen( QPen( Qt::red ) );    //Seteamos el color rojo.
    ui->customPlot->graph(0)->setScatterStyle( QCPScatterStyle( QCPScatterStyle::ssDisc , 5 ) ); //Seteamos el tipo de punto ssDisc.
    ui->customPlot->graph(0)->setLineStyle( QCPGraph::lsLine );   //Seteamos el tipo de linea lsLine.
    ui->customPlot->graph(0)->setName( "Temperatura" );   //Seteamos el nombre Temperatura.

    //ui->customPlot->graph(0)->setBrush(QBrush(QColor(255, 0, 0, 20))); // first graph will be filled with translucent red

    //Agregamos un nuevo grafico (humedad):
    ui->customPlot->addGraph( ui->customPlot->xAxis , ui->customPlot->yAxis2 );    //Asignamos los ejes x(inferior) e y2(derecho).
    ui->customPlot->graph(1)->setPen( QPen( Qt::blue ) );    //Seteamos el color azul.
    ui->customPlot->graph(1)->setScatterStyle( QCPScatterStyle( QCPScatterStyle::ssCircle , 5 ) ); //Seteamos el tipo de punto ssCircle.
    ui->customPlot->graph(1)->setLineStyle( QCPGraph::lsLine );   //Seteamos el tipo de linea lsLine.
    ui->customPlot->graph(1)->setName( "Humedad" );   //Seteamos el nombre "Humedad".

    //Agregamos un nuevo grafico (riego):
    ui->customPlot->addGraph( ui->customPlot->xAxis , ui->customPlot->yAxis2 );    //Asignamos los ejes x(inferior) e y2(derecho).
    ui->customPlot->graph(2)->setPen( QPen( Qt::green ) );    //Seteamos el color verde.
    ui->customPlot->graph(2)->setScatterStyle( QCPScatterStyle( QCPScatterStyle::ssNone , 5 ) ); //Seteamos el tipo de punto ssNone.
    ui->customPlot->graph(2)->setLineStyle( QCPGraph::lsLine );   //Seteamos el tipo de linea lsLine.
    ui->customPlot->graph(2)->setName( "Riego" );   //Seteamos el nombre "Riego".
    ui->customPlot->graph(2)->setBrush(QBrush(QColor(0, 255, 0, 20))); //Le agregamos relleno transparente verde

    //Agregamos un nuevo grafico (lluvia):
    ui->customPlot->addGraph( ui->customPlot->xAxis , ui->customPlot->yAxis2 );    //Asignamos los ejes x(inferior) e y2(derecho).
    ui->customPlot->graph(3)->setPen( QPen( QColor( 0 , 255 , 255 , 255 ) ) );    //Seteamos el color celeste.
    ui->customPlot->graph(3)->setScatterStyle( QCPScatterStyle( QCPScatterStyle::ssNone , 5 ) ); //Seteamos el tipo de punto ssNone.
    ui->customPlot->graph(3)->setLineStyle( QCPGraph::lsLine );   //Seteamos el tipo de linea lsLine.
    ui->customPlot->graph(3)->setName( "Lluvia" );   //Seteamos el nombre "Lluvia".
    ui->customPlot->graph(3)->setBrush(QBrush(QColor(0, 255, 255, 20))); //Le agregamos relleno transparente celeste

    //Le agregamos etiquetas a los ejes:
    //Eje x:
    ui->customPlot->xAxis->setLabel( "Hora" );  //Seteamos el nombre "Hora".
    //Eje y (izquierdo):
    ui->customPlot->yAxis->setLabel( "Temperatura [ºC]" );   //Seteamos el nombre "Temperatura [ºC]".
    ui->customPlot->yAxis->setLabelColor( QColor( Qt::red  ));  //Seteamos el color de la etiqueta a rojo.
    ui->customPlot->yAxis->setTickLabelColor( QColor( Qt::red ) );  //Seteamos el color de los numeros de referencia a rojo.
    //Eje y2 (derecho):
    ui->customPlot->yAxis2->setLabel("Humedad [%]");   //Seteamos el nombre "Humedad [%]".
    ui->customPlot->yAxis2->setLabelColor( QColor( Qt::blue ) );  //Seteamos el color de la etiqueta a azul.
    ui->customPlot->yAxis2->setTickLabelColor( QColor( Qt::blue ) );  //Seteamos el color de los numeros de referencia a blue

    //Configuramos los ejes secundarios que no vienen habilitados por defecto:
    ui->customPlot->xAxis2->setVisible(true);   //Hacemos visible el eje x2 (superior) paraformar un cuadrado y quede más estetico.
    ui->customPlot->xAxis2->setTicks(false);    //No habilitamos los ticks del eje x2 para que quede "liso"
    ui->customPlot->xAxis2->setTickLabels(false);//No habilitamos las etiquetas de los ticks del eje x2.

    ui->customPlot->yAxis2->setVisible(true);   //Hacemos visible el eje y2 (derecho).
    ui->customPlot->yAxis2->setTicks(true);     //Habilitamos los ticks del eje y2
    ui->customPlot->yAxis2->setTickLabels(true);//Habilitamos las etiquetas de los ticks del eje y2

    //Seteamos el rango inicial del grafico:
    ui->customPlot->yAxis->setRange(0, 40); //Eje de temperatura va desde 0ºC a 40ºC
    ui->customPlot->yAxis2->setRange(0, 100);//Eje de humedad va desde 0% a 100%
    ui->customPlot->xAxis->setRange(0, 8, Qt::AlignLeft);

    ui->customPlot->replot();   //Hacemos un replot

    //Timer para graficar en tiempo real que se dispara al conectar el puerto:
    timer_plot = new QTimer(this);  //Lo creamos
    connect(timer_plot, SIGNAL(timeout()),this,SLOT(realtimePlot()));   //Conectamos su señal de timeout con un slot para graficar en tiempo real
}

/**
    \fn  realtimePlot
    \brief se encarga de cargar los datos al grafico en tiempo real
*/
void MainWindow::realtimePlot( void )
{
    QTime time(QTime::currentTime());

    double time_dbl = time.hour() * 3600;
    time_dbl += time.minute() * 60;
    time_dbl += time.second();

    ui->customPlot->graph(0)->addData(time_dbl, Temperatura_y );
    ui->customPlot->graph(1)->addData(time_dbl, Humedad_y );
    //ui->customPlot->graph(2)->addData(time_dbl, m_Valvula );
    ui->customPlot->graph(3)->addData(time_dbl, m_Lluvia );

    ui->customPlot->xAxis->setRange(time_dbl, 8, Qt::AlignRight);
    ui->customPlot->replot();
}

/**
    \fn  CerrarPuertoSerie
    \brief slot que cierra el puerto y actualiza la interfaz
*/
void MainWindow::TimeoutPuertoSerie( void )
{
		puerto.TerminarConexion();
    ActualizarEstadoConexion();
}

/**
    \fn  PrepararTimeOutSerie
    \brief Inicializa el timer que se encarga del timeout del puerto serie
*/
void MainWindow::PrepararTimeOutSerie( void )
{
    //Pido memoria dinamica
    timerTimeOutSerie = new QTimer(this);
    //Conecto un timer de timeout para el puerto serie. Se dispara cuando se conecta el puerto
		connect(timerTimeOutSerie, SIGNAL(timeout()),this,SLOT(TimeoutPuertoSerie()));
}
/**
    \fn  PrenderLluvia
    \brief Refleja en la gui que esta lloviendo y le da valor a m_Lluvia para que grafique la lluvia
*/
void MainWindow::PrenderLluvia( void )
{
    m_Lluvia = LLUVIA_ON;
    ui->label_ELluvia->setText("Llueve");
    ui->label_ELluvia->setStyleSheet("QLabel { font-weight: bold; color : cyan; }");
}
/**
    \fn  ApagarLluvia
    \brief Refleja en la gui que no esta lloviendo y le da valor a m_Lluvia para que no grafique la lluvia
*/
void MainWindow::ApagarLluvia( void )
{
    m_Lluvia = LLUVIA_OFF;
    ui->label_ELluvia->setText("No llueve");
    ui->label_ELluvia->setStyleSheet("QLabel { font-weight: bold; color : red; }");
}
/**
    \fn  DesconectarLluvia
    \brief Refleja en el label de lluvia que esta desconectado y pone m_Lluvia en off
*/
void MainWindow::DesconectarLluvia( void )
{
    m_Lluvia = LLUVIA_OFF;
    ui->label_ELluvia->setText("Desconectado");
    ui->label_ELluvia->setStyleSheet("QLabel { font-weight: bold; color : black; }");
}
