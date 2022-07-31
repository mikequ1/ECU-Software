#ifndef SPEEDOMETER_H
#define SPEEDOMETER_H

double DISTANCE_PER_ROTATION = 0; // How far will the car roll in one rear wheel revolution
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
        Speedometer();
        void updateSpd(); // increments totalRotations, rotations, upon interrupt of wheel-mounted hall sensor
        double getSpd();
    private:
        double spd;
        int totalRotations; // analogous to totalRevolutions for RPM
        long lastRotationTimestamp; 
};

#endif