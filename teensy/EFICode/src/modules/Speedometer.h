#ifndef SPEEDOMETER_H
#define SPEEDOMETER_H

// in INCHES

// 2 ways to calculate speed
// 1) Same way as RPM (find time passed over n revolutions)
// +: less susceptible to double counts of the hall sensor
// -: could be very inaccurate as wheel spins a lot slower than engine
// 2) Find time elapsed over 1 rotation of wheel
// +: adequately accurate, also easy to implement
// -: could potentially be noisy if sensor/wiring is bad (can be minimized with the addition of some sort of filter)

// interface for implementation 2 (without filter)
class Speedometer {
    public:
        static Speedometer* create();
        void updateSpd(); // increments totalRotations, rotations, upon interrupt of wheel-mounted hall sensor
        double getSpd();
    private:
        Speedometer();
        double m_spd;
        unsigned long m_totalRotations; // analogous to totalRevolutions for RPM
        unsigned long m_lastRotationTimestamp; 
};

#endif