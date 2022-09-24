#ifndef EFI_HARDWARE_H
#define EFI_HARDWARE_H

class EFIHardware {
public:
    static EFIHardware* create();
    void pulseOn();
    void pulseOff();
    void enableINJ();
    void disableINJ();
    bool isInjDisabled() const;
    unsigned long getTotalFuelUsed() const;
    void setInjectorPulseTime(unsigned long injectorPulseTime);
    int getPulseCount() const;
private:
    EFIHardware();
    bool m_injIsDisabled;
    unsigned long m_totalPulseTime;
    unsigned long m_injectorPulseTime;
    unsigned long m_lastPulseTime;
    int m_pulseCount;
};

#endif