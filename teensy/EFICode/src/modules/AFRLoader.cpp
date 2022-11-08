#include "AFRLoader.h"
#include "SD.h"
#include "Arduino.h"

static const char* AFR_TABLES_FILENAME = "afrlist.txt";

AFRLoader* AFRLoader::create(){
    AFRLoader* afrLoader = new AFRLoader();
    if (!afrLoader->readAFRList(AFR_TABLES_FILENAME))
        return nullptr;
    return afrLoader;
}

AFRLoader::AFRLoader(){
}

bool AFRLoader::readAFRList(const char* filename) {
    Serial.print("Initializing SD card...");

    if (!SD.begin(4)) {
        Serial.println("SD initialization failed!");
        while (1);
    }
    Serial.println("SD initialization done.");

    File afrListFile;
    afrListFile = SD.open(filename);
    int afrIndex = 0;
    if (afrListFile) {
        while (afrListFile.available()) {
            m_afrList[afrIndex] = afrListFile.read();
            afrIndex++;
            Serial.write(m_afrList[afrIndex-1]);
        }
        afrListFile.close();
        return true;
    } else {
        return false;
    }
}


void AFRLoader::dumpPulseTimes() const {
    for (int i = 0; i < NUM_AFR_TABLE_ROWS; i++){
        for (int j = 0; j < NUM_AFR_TABLE_COLS - 1; j++){
            Serial.print(injectorBasePulseTimes[i][j]);
        }
        Serial.println(injectorBasePulseTimes[i][NUM_AFR_TABLE_COLS - 1]);
    }
}

