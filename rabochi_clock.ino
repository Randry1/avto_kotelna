
// CONNECTIONS:
// DS1302 CLK/SCLK --> 5
// DS1302 DAT/IO --> 4
// DS1302 RST/CE --> 2
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND

#include <ThreeWire.h>
#include <RtcDS1302.h>

ThreeWire myWire(4, 5, 2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);
long rtcOld = 0;
long rtcDelta = 0; //Время прошедшее между циклами
bool loading = false;

long konveer = 600; //600000 10 минут между срабатываниями
long konveerOld = 10;
int konveerPin = 8; //пин включения конвеера
int konveerPinOff = 9; // пин выключения конвеера
long konveerDelay = 5;


long knife = 180; // 180000 3 минуты между срабатываниямм
long knifeOld = 0;
int knifePin = 7; //Номер пина

long shiber = 3600; // 3600000 1 час между срабатываниями
long shiberOld = 0;
int shiberPin = 10;
long shiberDelay = 10;
long shiberStartCicke = 0;

void setup ()
{
  Serial.begin(57600);

  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);

  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();

  pinMode (konveerPin, OUTPUT);
  pinMode (konveerPinOff, OUTPUT);
  RtcDateTime konveerOld = Rtc.GetDateTime(); //TODO не срабатывает на каждой загрузке 
  digitalWrite(konveerPin, HIGH); //включаем на старте пины чтобы выключить реле(реле срабатывают когда на пине нет питания

  pinMode (knifePin, OUTPUT);
  RtcDateTime knifeOld = Rtc.GetDateTime();
  digitalWrite(knifePin, HIGH);

  pinMode (shiberPin, OUTPUT);
  RtcDateTime shiberOld = Rtc.GetDateTime();
  digitalWrite(shiberPin, HIGH);

  konveerOld = Rtc.GetDateTime();
  knifeOld = Rtc.GetDateTime();
  shiberOld = Rtc.GetDateTime();


  if (!Rtc.IsDateTimeValid())
  {
    // Common Causes:
    //    1) first time you ran and the device wasn't running yet
    //    2) the battery on the device is low or even missing

    Serial.println("RTC lost confidence in the DateTime!");
    Rtc.SetDateTime(compiled);
  }

  if (Rtc.GetIsWriteProtected())
  {
    Serial.println("RTC was write protected, enabling writing now");
    Rtc.SetIsWriteProtected(false);
  }

  if (!Rtc.GetIsRunning())
  {
    Serial.println("RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled)
  {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  }
  else if (now > compiled)
  {
    Serial.println("RTC is newer than compile time. (this is expected)");
  }
  else if (now == compiled)
  {
    Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }
}

void loop ()
{
  // проверяем загружались ли часы
  if (loading == false) {
    delay(9000);
    loading = true;
   
  }

  if (millis() % 1000 == 0) {       //  задержка в одну секунду перед выводом времени в последовательный порт

    RtcDateTime now = Rtc.GetDateTime();


//    printDateTime(now);
    if (now.IsValid())
    {
//      Serial.println();
      //      rtcDelta = now - rtcOld;
      //      Serial.print(rtcDelta);
      //      Serial.println();

      if ( ((now - shiberOld) >= shiber) & ((now - konveerOld) > 80) & ((now - knifeOld) > 40) ) //если прошло достаточно времени то запустить шибер
      {
        Serial.print(now - knifeOld);
        Serial.print(" сек. сработал шибер ");
        printDateTime(now);
        Serial.println();
        shiberOld = now;

        digitalWrite(shiberPin, LOW);
        delay(1000);
        digitalWrite(shiberPin, HIGH);
      }

      if ( ((now - knifeOld) >= knife) & ((now - shiberOld) > 55)  & ((now - konveerOld) > 80 ) ) //если прошло достаточно времени то запустить ножи
      {
        Serial.print(now - knifeOld);
        Serial.print(" сек. сработали ножи ");
        printDateTime(now);
        Serial.println();
        knifeOld = now;

        digitalWrite(knifePin, LOW);
        delay(1000);
        digitalWrite(knifePin, HIGH);
      }

      if ( ((now - konveerOld) >= konveer) & ((now - shiberOld) > 75) & ((now - knifeOld) > 50) )
      {
        Serial.print(now - knifeOld);
        Serial.print(" сек. сработал конвеер ");
        printDateTime(now);
        Serial.println(); 
        konveerOld = now;

        digitalWrite(konveerPin, LOW);
        delay(1000);
        digitalWrite(konveerPin, HIGH);

        delay(5000);
        digitalWrite(konveerPinOff, LOW);
        delay(1000);
        digitalWrite(konveerPinOff, HIGH);
      }





      if (!now.IsValid())
      {
        // Common Causes:
        //    1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
      }
      delay(700);
      rtcOld = now;
//      Serial.println();
    }
    //    delay(10000); // ten seconds
  }
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second() );
  Serial.print(datestring);
}

bool trigerTime(long timeNow, long startObject, long delayObject) {
  if ((timeNow - startObject) > delayObject)
  {
    return true;
  } else {
    return false;
  }
}
