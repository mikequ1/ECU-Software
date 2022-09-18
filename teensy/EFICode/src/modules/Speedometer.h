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
        /**
         * @brief Create a speedometer object
         * 
         * @return pointer to the newly-created Speedometer object
         */
        static Speedometer* create();


        /**
         * @brief increments totalRotations, rotations, upon interrupt of wheel-mounted hall sensor
         * 
         */
        void updateSpd();


        /**
         * @brief Get the current speed in MPH
         * 
         * @return double, current speed in MPH
         */
        double getSpd();

    private:
        /**
         * @brief Construct a new Speedometer object
         * 
         */
        Speedometer();

        // Current speed in mph
        double m_spd;
        // Number of total rear wheel rotations
        unsigned long m_totalRotations;
        // Timestamp of the most recently recorded wheel rotation
        unsigned long m_lastRotationTimestamp; 
};

#endif