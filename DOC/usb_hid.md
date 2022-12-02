# USB HID как развернуть

## CUBA
1. Connectivity > USB_OTG_FS:
    * Mode: Device_Only
    * Activate_SOF - да
    * Activate_VBUS - да
2. Middleware > USB_DEVICE
    * Class for FS IP: Custom Human Interface Device Class (HID)
    * USBD_CUSTOM_HID_REPORT_DESC_SIZE = 38
    * USBD_CUSTOMHID_OUTREPORT_BUF_SIZE = 20
    * Придумываем PRODUCT_STRING
    * Придумываем MANUFACTURER_STRING

## Дискриптор
1. ./USB_DEVICE/App/usbd_custom_hid_if.c находим CUSTOM_HID_ReportDesc_FS. Прорисываем следующий дискриптор:
```
  0x06, 0x00, 0xff,              //   USAGE_PAGE (Generic Desktop)
  0x09, 0x01,                    //   USAGE (Vendor Usage 1)
  // System Parameters
  0xa1, 0x01,                    //   COLLECTION (Application)
  0x85, 0x01,                    //   REPORT_ID (1)
  0x09, 0x01,                    //   USAGE (Vendor Usage 1)
  0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
  0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
  0x75, 0x08,                    //   REPORT_SIZE (8)
  0x95, REPORT_COUNT,            //   REPORT_COUNT (16)
  0xb1, 0x82,                    //   FEATURE (Data,Var,Abs,Vol)
  0x85, 0x01,                    //   REPORT_ID (1)
  0x09, 0x01,                    //   USAGE (Vendor Usage 1)
  0x91, 0x82,                    //   OUTPUT (Data,Var,Abs,Vol)

  0x85, 0x02,                    //   REPORT_ID (2)
  0x09, 0x02,                    //   USAGE (Vendor Usage 2)
  0x75, 0x08,                    //   REPORT_SIZE (8)
  0x95, REPORT_COUNT,            //   REPORT_COUNT (16)
  0x81, 0x82,                    //   INPUT (Data,Var,Abs,Vol)
  ```
  Этот дискриптер описывает формат передачи пакетов. Тут открываеться два типа сообщений: один на передачу, второй на прием.
  2. В начале файла подключаем usbhid.h для работы прерываний
  ```
  #include "usbhid.h"
  ```
  3. ./USB_DEVICE/Target/usbd_conf.h меняем константы:
  USBD_CUSTOM_HID_REPORT_DESC_SIZE = 38
  USBD_CUSTOMHID_OUTREPORT_BUF_SIZE = 45

  ## Прерывания
  Помимо прерываний, настраиваемых CUBA требуеться настроить три прерывания. В ./USB_DEVICE/App/usbd_custom_hid_if.h

  ### Прерывание по приему данных
  В функции CUSTOM_HID_OutEvent_FS вызываем первой строчкой
  vUSBreceiveHandler();

  ### Прерывание по подключению устройства
  В функции CUSTOM_HID_Init_FS вызываем
  vUSBplugHandler();

  ### Прерывание по отключению устройства
  В функции CUSTOM_HID_DeInit_FS вызываем
  vUSBunplugHandler();

  ## Инициализация

  Сразу после инициализации другой переферии (TIM, UART и т.д.) вызвать инициализацию USB: vUSBinit. Аргументами являються два пина - пин детектирования подключения устройства и пин внешнего pullup. Эти пины не иницилизируються CUBA и с точки зрения самого USB не являються обязательными, но с точки зрения схемотехники очень желательны, а порой обязательны.

  # Протокол верхнего уровня

  ## Общее описание
  Если протокол в дискрипторе просто обявляет общий принцип передачи байтов, то далее по верх него есть протокол для передачи осмысленных данных. ./common/Inc/usbhid.h и ./common/Src/usbhid.c описывают его. Под чтением понимаеться запрос к устройсву на получение данных. Под записью понимаеться запрос к устройству на изменение данных. Каждое сообщение имеет следующие поля:
  1. DIR - байт напроавления движения данных 
  2. CMD - байт команды
  3. STAT - байт состояния
  4. ADRx - байты адреса
  5. LENx - байты длинны данных
  6. DATA - байты данных

  Поля описаны в энуме USB_BYTES

  ### DIR
  Направление описываеться в USB_REPORT_ID. Для выбора направления рекомендуеться использовать макрос USB_GET_DIR_VAL.
  
  ### CMD
  Помимо очевидных чтение и запись в команды выделяються специфические действия для оптимизации протокола, такие как:
  1. USB_REPORT_CMD_NULL - отсутсвие команды
  1. USB_REPORT_CMD_START_WRITING - разрешение записи во флешь
  1. USB_REPORT_CMD_WRITE_SCRIPT - запись данных во флешь в раздел для скрипта
  1. USB_REPORT_CMD_END_WRITING - запрет записи во флешь
  1. USB_REPORT_CMD_READ_SCRIPT - чтение скрипта
  1. USB_REPORT_CMD_READ_DATA - чтение системных (неизменных в рантайме) данных (PDM_DATA) (NB! Здесь можно добавить еще модель устройства в серии):
      1. Уникальный номер устройства
      1. Версия бутлоудера
      1. Версия прошивки
      1. Версия платы
      1. Версия lua
  1. USB_REPORT_CMD_READ_TELEMETRY - чтение данных телеметрии
  1. USB_REPORT_CMD_UPDATE_TELEMETRY - обновить данные телеметрии (стоит посылать эту команду до чтения)
  1. USB_REPORT_CMD_RESTART_LUA - перезапус lua скрипта
  1. USB_REPORT_CMD_READ_ERROR_STR - передача строки ошибки lua

В последствии специфические действия, которые требуют только ответ об окончании, стоит выводить в отдельные команды. Данные стоит размещать либо в телеметрии либо в системные данные, в зависимости от их изменяемости. Неизменные в рантайме в сстемные данные, изменяемые в рантайме в телеметрию.

### STAT
Состояние ответа ( USB_REPORT_STATE ):
1. USB_REPORT_STATE_OK - успешное выполнение команды
1. USB_REPORT_STATE_BAD_REQ - неправильно составленный запрос (например запрос за пределами допустимой области памяти)
1. USB_REPORT_STATE_NON_CON - неправильная адресация
1. USB_REPORT_STATE_UNAUTHORIZED - ошибка авторизации (легаси)
1. USB_REPORT_STATE_FORBIDDEN - отказ в доступе (легаси)
1. USB_REPORT_STATE_INTERNAL - любая внутренняя ошибка устройства (ошибка чтения или невалидные данные при выполнении команды)

### ADR
Адрес к памяти в байтах. Имеет смысл только при чтении/записи. Во всех остальных случаях равен 0. В текущей версии поле длинной 2 байта.

### LEN
Длинна поля данных в байтах. Имеет смысл только при чтении/записи. Во всех остальных случаях равен 0. В текущей версии поле длинной 1 байт.

### DATA
Поля данных. Длинна зависит от длины передаваемых данных, описаннных в дискрипторе, и от длинны полей в протоколе. Вычисляеться тут: USB_DATA_SIZE.

### USB_REPORT
Для удобства работы поля протокола описаны в структуре USB_REPORT. Внимание: данные хранятьс в отдельных масивах.

## Добавление новых команд
1. Добавляем новую команду в список USB_REPORT_CMD. Рекомендуеться в конец
1. В vUSBtask заводим новый case для новой команды.
1. Если команда на чтение, то обработчик - vUSBsend, если запись или выполнение действий - vUSBget
1. В качетве аргументов указываем внутреннюю структру report и колбэк на обработку данных.
    1. В случае чтения, аргумент колбэка USB_REPORT* report, а функция возвращает void. Колбэк должен разместить: 
        * Проверить валидность запроса
        * в report->data необходимые данные, в соответсвием с запрашиваемым адресом. Длинна данных может быть от 1 до ( USB_DATA_SIZE - 1U )
        * в report->length длинну передаваемых данных
        * в report->stat состояние ответа
    1. В случае записи, аргумент колбэка const USB_REPORT* report, а функция возвращает USB_STATUS. Колбэк должен:
        * Отпарсить поле report->data, в соотвествии с адресом и длинной ( report->adr и report->length ).
        * В случае успеха вернуть USB_STATUS_DONE или код ошибки ( см. USB_STATUS ) 
1. Далее добавить обработку команды в приложении (см. раздел JS)

## Добавление новых системных данных
1. Добавить в структуру PDM_DATA (data.h) новое поле. Рекомендуеться в конце
1. Инициализировать данные в vDATAinit (data.c)
1. Далее добавить обработку команды в приложении (см. раздел JS)

## Добавление новых данных телеметрии
1. Добавить в структуру PDM_TELEMETRY (data.h) новое поле. Рекомендуеться в конце
1. В функции vDATAupdate добавить обновление новых полей данных
1. Далее добавить обработку команды в приложении (см. раздел JS)

# JS
За описание протокола нижнего уровня отвечает библиотека node-hid и NodeJS. Библиотека подключена через пакетный менеджер. Протокол верхнего уровня описаны в usb-message.js и в usb.js.

## usb-message.js
Этот файл описывает класс USBMessage. Класс позволяет парсить и генерировать все сообщения из протокола.

### Для добавления новой команды необходимо:
1. В список msgCMD добавить команду в соотвествии с USB_REPORT_CMD.
1. В функции parsingCommandByte добавить новый case проверки валидности команды.
1. Для команды создать  метод makeXxxxxxxxRequest. В качестве аргумента может быть адрес, если команда записи. Внутри надо воспользоваться функцией makeRequest. В качестве аргументов - команда и адрес. Если адреса нет в аргументах процедуры, то он равен 0.
1. Для команд записи создать  метод codeXxxxx, с произвольными аргументами, но процедура должна заканчиваться makeResponse.
1. Для команд чтения добавить case в метод parse. В output записываем массив данных, в type кладем тип данных. Это неободимо для описание рендера данных. В случае необходимости добавить новый тип данных в список msgType.

## usb.js
Этот файл описывает классы буфферов на чтение и запись, классы нижнего уровня передачи данных, использование класса USBMessage, описываються обработка прерываний. 

### Для добавления новой команды необходимо:
1. В классе USBtransport в функции writeHandler добавить каманду в список проверки
1. В классе PdmController создать функцию инициализации последовательности сообщений или добавить новую команду в существующую последовательность.
1. При необходимости добавить нужную процедуру для вызова инициализации последовательности и разместить ее в других частях кода при необходимости.

## render.js
Это файл описывает алгоритм использования данных, полученных по USB. При создании новых типов данных (см. usb-message.js), необходимо добавить соотвествующий case в parsingFullMessages. При необходимости вызвать специические функции для работы с DOM. Для отображения системных данных тут есть функция updateInterface.

## pdm.js
Описывает класс PDM. Он хранит все данные об устройстве. При изменении системных данных и данных телеметрии необходимо изменить соотвествующие методы парсинга.

## dashboard.js
Описывает работу с DOM соответсвующего раздела приложения. Для изменения отображаемых данных необходимо редактировать/добавлять объекты в массив values и cards. Правила отображения данных описаны в updateCards и updateValues.