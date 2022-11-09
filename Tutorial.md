# HAL Library programming

El documento que describe la libreria HAL es el siguiente

UM1850 User manual Description of STM32F1xx HAL drivers

Vale la pena tenerlo a mano para descartar dudas, otros dos documentos son datasheet y reference manual de la micro (estan en la carpeta doc/ del repo)

El programa de la micro consta con 2 partes principales: (1) inicializacion y configuracion de hardware (por lo general lo trato hacer siempre en la funcion HardwareInit()) y (2) applicacion la cual controla los perifericos y hace el procesamiento (por eso microcontrolador).
Hay que tener en cuenta que hay nucleo y perifiericos estan conectados via buses AHB y APB1 y APB2

## Hardware initialization

Inicializando cualquier bloque hay que hablilitar el reloj en el usando un macro, por ejemplo

__HAL_RCC_GPIOA_CLK_ENABLE();

habilita el reloj en el bloque GPIOA

HAL encapsula los modulos (por lo general los perifiericos) usando las estructuras, por lo tanto la configuracion se hace mediante inicializacion de las estructuras con los valores deseados y posteriormente hay que pasarla al respectivo bloque usando una funcion de configuracon

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); 

en detalles se va creando un estructura GPIO_InitStruct, y posteriormente se inicializa sus campos (fields) con valores, una vez terminado se envia la direccion (operador &) a la funcion HAL_GPIO_Init. En ese partucular caso al GPIOC

Posteriormente los modulos tendran algunas funciones las cuales sirven para manejarlos (generalmente en la applicacion despues de configuracion)

    HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12);
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

generalmente se podia decir que es todo lo que uno necesita para programar sin interrupciones. La mayoria del trabajo esta acumulada en lectura de RefManual y HAL UserManual para: (1) determinar como funcionan los modulos, (2) identificar cuales son los parametros de configuracion, (3) identificar la configuracion deseada (eso depende de escenario de uso), (4) vincular la configuracion con los valores de estructuras a inicializar.

En el archivo HardwareInit.c  ese esquema se repite.
