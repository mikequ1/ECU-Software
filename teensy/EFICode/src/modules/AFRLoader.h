#ifndef AFRLOADER_H
#define AFRLOADER_H

static const int NUM_AFR_TABLE_ROWS = 11;
static const int NUM_AFR_TABLE_COLS = 10;
static const int MAX_NUM_OF_AFR_TABLES = 10;

class AFRLoader {
public:
    static AFRLoader* create();
    static void readAFRList(const char* filename, char* (&afrList)[MAX_NUM_OF_AFR_TABLES], int& afrListSize);
    bool updateAFR(const char* tablename);

    void getFuelRatioTable(double (&fuelRatioTable)[NUM_AFR_TABLE_ROWS][NUM_AFR_TABLE_COLS]) const;
    void dumpAFRList() const;
    void dumpFuelRatios() const;
private:
    AFRLoader();
    double m_fuelRatioTable[NUM_AFR_TABLE_ROWS][NUM_AFR_TABLE_COLS];
    char* m_afrList[MAX_NUM_OF_AFR_TABLES];
    int m_afrListSize;
};

#endif