#include <AceButton.h> // used for button handling
#include <EEPROM.h>

#include "Keyboard.h"
#include "Timer.h"

const uint8_t buttonPin = 8; /// pin where the button is connected
const uint8_t ledPin = 9; /// pin where the LED is connected
bool ledState = false; /// current state of the LED (false = off, true = on)
bool ledShouldFlicker = false; /// state variable whether the LED should flicker

char myChar = ' '; /// character to be sent to the PC on button presses

ace_button::AceButton button; /// the button
uint8_t flickered = 0; /// the amount of LED flickers

Timer ledTimer("LedTimer", 50); /// timer for LED flickers (LED flickering every 50ms)

void handleEvent(ace_button::AceButton* button, uint8_t eventType, uint8_t buttonState)
{
    switch (eventType)
    {
    case ace_button::AceButton::kEventPressed:
        Keyboard.write(myChar);
        ledShouldFlicker = true;
        flickered = 0;
        break;
    default:
        break;
    }
}

String split(const String& s, const char parser, const int index)
{
    int parserIndex = index;
    int parserCnt = 0;
    int rFromIndex = 0;
    int rToIndex = -1;
    while (index >= parserCnt)
    {
        rFromIndex = rToIndex + 1;
        rToIndex = s.indexOf(parser, rFromIndex);
        if (index == parserCnt)
        {
            if (rToIndex == 0 || (rToIndex == -1 && rFromIndex == -1))
            {
                return "";
            }
            else
            {
                rToIndex = s.length();
            }
            return s.substring(rFromIndex, rToIndex);
        }
        else
        {
            parserCnt++;
        };
    }
    return "";
}

class Menu
{
public:
    Menu() { Serial.setTimeout(100); }

public:
    void update()
    {
        if (printMenu)
        {
            printMenu = false;
            Serial.println(F("-----------------------------------------------------------"));
            Serial.println(F("Menu:"));
            Serial.println(F("help or h             - To show this information"));
            Serial.println(F("exit or e             - To exit menu"));
            Serial.println(F("read or r             - To show the currently set character"));
            Serial.println(F("set  or s <character> - To change the character"));
            Serial.println();
            Serial.println();
            Serial.println(F("Further reading: Whenever the button is pressed it will"));
            Serial.println(F("                 press the set character like a keyboard"));
            Serial.println(F("                 does. In order to change the character use"));
            Serial.println(F("                 the set command and replace <character>"));
            Serial.println(F("                 with the character to be pressed, for"));
            Serial.println(F("                 example: \"set g\" or \"s g\""));
            Serial.println();
            Serial.println();
            Serial.println(F("-----------------------------------------------------------"));
        }
        // check for incoming serial data:
        if (Serial.available() > 0)
        {
            // read incoming serial data:
            String data = Serial.readString();
            data.replace("\n", "");

            // Serial.println("Read: " + data);

            if (data.equalsIgnoreCase("help") || data.equalsIgnoreCase("h"))
            {
                printMenu = true;
            }
            else if (data.equalsIgnoreCase("exit") || data.equalsIgnoreCase("e"))
            {
                Serial.println("Exiting menu");
                mIsDone = true;
            }
            else if (data.equalsIgnoreCase("read") || data.equalsIgnoreCase("r"))
            {
                Serial.print("Current character: \'");
                Serial.print(myChar);
                Serial.println("\'");
            }
            else if (data.startsWith("set") || data.startsWith("s"))
            {
                myChar = split(data, ' ', 1).charAt(0);
                EEPROM.write(23, myChar);
                Serial.print("Character set to: \'");
                Serial.print(myChar);
                Serial.println("\'");
            }
        }
        delay(10);
    }

    bool isDone() { return mIsDone; }

private:
    bool mIsDone = false;
    bool printMenu = true;
};

void setup()
{
    // open the serial port:
    Serial.begin(115200);
    // initialize keyboard
    Keyboard.begin();
    // read the stored character
    myChar = EEPROM.read(23);

    // init GPIO
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, 0);
    pinMode(buttonPin, INPUT_PULLUP);
    button.init(buttonPin);

    // check whether we should enter configuration mode
    if (button.isPressedRaw())
    {
        Menu menu {};
        digitalWrite(ledPin, HIGH);
        while (!Serial) { }
        while (!menu.isDone())
        {
            menu.update();
        }
        digitalWrite(ledPin, LOW);
    }

    // setup event handler for button
    ace_button::ButtonConfig* config = button.getButtonConfig();
    config->setFeature(ace_button::ButtonConfig::kFeatureClick);
    button.setEventHandler(handleEvent);
}

void loop()
{
    button.check(); // handle button clicks

    if (ledShouldFlicker)
    {
        const uint32_t time = millis();
        if (ledTimer.check(time))
        {
            ledState = !ledState;
            digitalWrite(ledPin, ledState);
            ++flickered;
            if (flickered >= 40)
            {
                flickered = 0;
                ledShouldFlicker = false;
            }
        }
    }
    else
    {
        if (ledState)
        {
            ledState = !ledState;
            digitalWrite(ledPin, ledState);
        }
    }
}
