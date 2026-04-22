### 🛠 Technology Stack
* **Languages:** C++ (Qt 6), Embedded C (Paho MQTT), Python (Scripting/Testing)
* **Protocols:** MQTT over SSL/TLS, I2C (Register-level), JSON
* **Hardware:** Raspberry Pi 4, ADXL345 (3-Axis Accelerometer)
* **Cloud:** HiveMQ Cloud Broker

---

### The Project Overview
Engineered a secure, end-to-end telemetry pipeline designed to monitor and synchronize high-G impact data from edge devices to a centralized monitoring station. This project bridges the gap between low-level sensor interfacing and high-level data visualization.

### Technical Highlights
* **IoT Cloud Integration:** Configured a secure **MQTT** pipeline to sync data to **HiveMQ Cloud**, enforcing industry-standard **SSL/TLS encryption** via port 8883 to prevent data sniffing.
* **Custom Qt Dashboard:** Developed a high-performance visualization suite in **C++ (Qt 6)**. It features an asynchronous JSON parser and a live-scrolling UI that renders 3-axis vibration data in real-time.
* **Asynchronous Event Handling:** Architected an interrupt-driven sensing system using **libgpiod**. By monitoring hardware interrupts from the **ADXL345**, the system eliminates CPU polling, drastically reducing power consumption and latency.
* **Hardware Driver Development:** Wrote custom I2C drivers from scratch, handling raw bit-manipulation and implementing **Two’s Complement** sign-extension for accurate acceleration processing.

---

### Key Learning Outcomes
* **End-to-End IoT Security:** Gained practical experience in implementing secure cloud handshakes and managing certificates for embedded devices.
* **Real-Time Data Streams:** Mastered the management of asynchronous data buffers and UI thread synchronization in Qt to prevent interface "freezing" during high-speed data bursts.
* **Embedded Linux API:** Deepened knowledge of Linux character device drivers and the `libgpiod` library for modern hardware interfacing.

---

### Hardware Target
The system is built on the **Raspberry Pi 4** platform, utilizing an **ADXL345** digital accelerometer for impact sensing. It is designed to be easily portable to other Linux-based Single Board Computers (SBCs).
