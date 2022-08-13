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
    double getTotalFuelUsed() const;
private:
    EFIHardware();
    bool m_injIsDisabled;
    bool m_haveInjected;
    unsigned long m_totalPulseTime;
    unsigned long m_injectorPulseTime;
    unsigned long m_lastPulseTime;
};

#endif