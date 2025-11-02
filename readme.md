# STM32 Wheel-Leg Robot 

[中文文档](readme_cn.md)

**Disclaimer:** This project is developed based on Mu Shibo's open-source project [Micro-Wheeled_leg-Robot](https://github.com/MuShibo/Micro-Wheeled_leg-Robot).

**Unmodified Sections:**
- Mechanical Structure

**Major Modifications:**
- PCB Design
- Source Code

---

## 1. PCB Files

The original project, designed using LCEDA, consists of four PCB boards. This modification involves two of them:

- **ControllerPCB → CONTROL_STM32F407**  
  Replaced the original ESP32 with an STM32F407 and modified the bus serial port related circuits to a logic gate configuration.

- **IMUPCB → IMU_ESP8266**  
  Changed the MPU6050 module to a SMD (Surface-Mount Device) type and added an ESP8266 module for wireless communication.

---

## 2. Source Code

- Refactored some of the original project code into C. The project is currently in the upright balance debugging phase.
- Use STM32CubeMX to configure STM32, and compile the project using gcc.
- Utilizes the ESP8266 as a communication module for the STM32, operating in STA Client mode for debugging.
- Most USART and I2C communications have been switched to DMA transfer mode to reduce CPU blocking.
- The drivers for the gyroscope and encoder are functional. Data is currently acquired via a timer at a frequency of 1ms. Given that the two share the same bus and need to read data alternately, it is not recommended to use a higher acquisition frequency to avoid bus conflicts

---

## Incompetent

- Due to personal skill limitations and the inability to achieve stable balance tuning via PID control after two months of debugging, this project has been suspended.
- If a friend is interested in researching, they can donate the supporting hardware for research.

---

## Contributors

- Mu Shibo
- Li Yufeng

Thanks to open source, I have learned a lot