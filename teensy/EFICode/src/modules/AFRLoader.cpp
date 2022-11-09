#include "AFRLoader.h"
#include "SD.h"
#include "Arduino.h"

static const char* AFR_TABLES_FILENAME = "afrlist.txt";
double DEFAULT_AFR_TABLE[NUM_AFR_TABLE_ROWS][NUM_AFR_TABLE_COLS] =
    {
    {14.5,14.8,15.0,15.3,15.3,15.2,14.6,14.3,13.8,88.8},  // minimum pressure
    {14.5,14.8,15.0,15.3,15.3,15.2,14.6,14.3,13.8,13.6},
    {14.5,14.8,15.0,15.3,15.3,15.2,14.6,14.3,13.8,13.6},
    {14.5,14.8,15.5,15.3,16.0,16.0,15.7,14.3,13.8,13.6},
    {14.5,16.0,17.2,17.2,16.0,16.0,15.7,14.3,13.8,13.6},
    {14.5,17.0,17.2,17.2,16.0,16.0,15.7,14.3,13.8,13.6},
    {14.5,17.0,17.2,17.2,15.0,15.2,14.6,14.3,13.8,13.6},
    {14.5,14.8,17.2,17.2,15.0,15.2,14.6,14.3,13.8,13.6},
    {14.5,14.8,17.2,17.2,15.3,15.2,14.6,14.3,13.8,13.6},
    {14.5,14.8,15.2,15.3,15.3,15.2,14.6,14.3,13.8,13.6},
    {14.5,14.8,15.0,15.3,15.3,15.2,14.6,14.3,13.8,13.6}   // maximum pressure
    };

AFRLoader* AFRLoader::create(){
    AFRLoader* afrLoader = new AFRLoader();
    return afrLoader;
}

AFRLoader::AFRLoader(){
    char* afrList[MAX_NUM_OF_AFR_TABLES];
    int afrListSize = 0;
    AFRLoader::readAFRList(AFR_TABLES_FILENAME, afrList, afrListSize);

    for (int i = 0; i < afrListSize; i++){
        m_afrList[i] = afrList[i]; 
    }
    m_afrListSize = afrListSize;

    if (!m_afrList){
        for (int i = 0; i < NUM_AFR_TABLE_ROWS; i++){
            for (int j = 0; j < NUM_AFR_TABLE_COLS; j++){
                m_fuelRatioTable[i][j] = DEFAULT_AFR_TABLE[i][j];
            }
        }
    } else {
        char* default_afrFile = m_afrList[0];
        Serial.print("Default AFR Tables: ");
        Serial.println(default_afrFile);
    }
}

void AFRLoader::readAFRList(const char* filename, char* (&afrList)[MAX_NUM_OF_AFR_TABLES], int& afrListSize) {
    Serial.print("Initializing SD card...");

    if (!SD.begin(BUILTIN_SDCARD)) {
        Serial.println("SD initialization failed!");
        return;
    }
    Serial.println("SD initialization done.");

    int afrListIndex = 0;
    File afrListFile = SD.open(filename);

    if (afrListFile) {
        char afrFileBuffer [100];
        int afrIndex = 0;
        while (afrListFile.available()) {
            char curByte = afrListFile.read();
            if (curByte != '\n'){
                afrFileBuffer[afrIndex] = curByte;
                afrIndex++;
            } else {
                afrFileBuffer[afrIndex] = '\0';
                char* afrFilenameCopy = malloc(strlen(afrFileBuffer) + 1);;
                strcpy(afrFilenameCopy, afrFileBuffer);
                afrList[afrListIndex] = afrFilenameCopy;
                // Serial.println(afrList[afrListIndex]);

                strcat(afrFileBuffer,"");;
                afrIndex = 0;
                afrListIndex++;
            }
        }
        afrListSize = afrListIndex;
        afrListFile.close();
    } 
}

bool AFRLoader::updateAFR(const char* tablename) {
    File afrTableFile = SD.open(tablename);
    double tempFuelRatioTable[NUM_AFR_TABLE_ROWS][NUM_AFR_TABLE_COLS];

    if (afrTableFile){
        int afrColIndex = 0;
        int afrRowIndex = 0;
        char afrValueBuffer[10];
        int afrValueIterator = 0;
        while (afrTableFile.available()){
            if (afrRowIndex > NUM_AFR_TABLE_ROWS || afrColIndex >= NUM_AFR_TABLE_COLS){
                Serial.println("Incompatible AFR Table Rows and Columns");
                return false;
            }
            char curByte = afrTableFile.read();
            if (curByte == ','){
                afrValueBuffer[afrValueIterator] = '\0';
                afrValueIterator = 0;
                tempFuelRatioTable[afrRowIndex][afrColIndex] = atof(afrValueBuffer);
                afrColIndex++;
            } else if (curByte == '\n'){
                afrValueBuffer[afrValueIterator] = '\0';
                afrValueIterator = 0;
                tempFuelRatioTable[afrRowIndex][afrColIndex] = atof(afrValueBuffer);
                afrRowIndex++;
                afrColIndex = 0;
            } else {
                afrValueBuffer[afrValueIterator] = curByte;
                afrValueIterator++;
            }
        }
        noInterrupts();
        for (int i = 0; i < NUM_AFR_TABLE_ROWS; i++){
            for (int j = 0; j < NUM_AFR_TABLE_COLS; j++){
                m_fuelRatioTable[i][j] = tempFuelRatioTable[i][j];
            }
        }
        interrupts();
        this->dumpFuelRatios();
        return true;
    }
    return false;
}

void AFRLoader::getFuelRatioTable(double (&fuelRatioTable)[NUM_AFR_TABLE_ROWS][NUM_AFR_TABLE_COLS]) const {
    for (int i = 0; i < NUM_AFR_TABLE_ROWS; i++){
        for (int j = 0; j < NUM_AFR_TABLE_COLS - 1; j++){
            fuelRatioTable[i][j] = m_fuelRatioTable[i][j];
        }
    }
}

void AFRLoader::dumpAFRList() const {
    Serial.println("===== Dumping AFR List =====");
    for (int i = 0; i < m_afrListSize; i++){
        Serial.println(m_afrList[i]);
    }
    Serial.println("====== End of List =====");
}

void AFRLoader::dumpFuelRatios() const {
    Serial.println("===== Dumping current Fuel Ratios =====");
    for (int i = 0; i < NUM_AFR_TABLE_ROWS; i++){
        for (int j = 0; j < NUM_AFR_TABLE_COLS - 1; j++){
            Serial.print(m_fuelRatioTable[i][j]);
            Serial.print(", ");
        }
        Serial.println(m_fuelRatioTable[i][NUM_AFR_TABLE_COLS - 1]);
    }
    Serial.println("====== End of List =====");
}

