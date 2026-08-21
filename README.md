# Smart-Plant-Care-Alert-System
MIC IOT Task

Overview:
The Smart Smart Plant Care Alert System is a simulated IoT device that monitor's a plant's environment using three sensors, decides what the plant needs, and reports status over MQTT.


Problem Statement:
Develop a simulated IoT Smart Plant Care Alert System using an ESP32 microcontroller in Wokwi that continuously monitors a plant's environmental conditions, processes multi-sensor decision logic, and publishes real-time status alerts and telemetry to a cloud MQTT broker over Wi-Fi.


Components:
  Simulator
      Wokwi
  Microcontroller
      ESP32
  Sensors
      DHT22 - Humidity and Temperature
      LDR - Ambient Light 
      Potentiometer - Soil Moisture
  Connectivity
      Wi-Fi - Wokwi-GUEST
      MQTT - HiveMQ
  Libraries
      DHTesp - DHT22 
      PubSubClient -  WI-fi and MQTT


Approach:
To build a simple, reliable plant monitor that doesn't freeze up or send false alarms.
   Sensor Ranges First: Instead of default threshold numbers, tested the sensors at their minimum and maximum settings. Using the observed range values of each                            sensor, threshold values are set.
   Priority Logic: The ESP32 reads all three sensors every few seconds. The code is structured so that if multiple conditions turn bad at the same time, the                                    system immediately catches it. By checking the multi-sensor emergency conditions first—like extreme heat paired with dry soil—the plant monitor                              raises a high-priority alert instead of getting distracted by a basic single-sensor warning.
   Smooth Code Flow: A tiny delay() during the initial setup() routine is used to allow sensor hardware to stabilize upon boot. For all continuous tasks inside                                   the main loop(), the non-blocking millis() timing is implemented to keep Wi-Fi and MQTT connectivity fully responsive without system lags.
   Organized Cloud Data: The ESP32 connects to Wi-Fi (Wokwi-GUEST) and sends data to the HiveMQ broker (broker.hivemq.com). The data has been split into clear                                        separate topics (plant/monitor/status, temp, soil, light) so anyone subscribing can choose to view just the alerts or the raw sensor                                          numbers. 
   Auto-Reconnect: If the Wi-Fi or MQTT broker drops, the code automatically tries to reconnect in the background without crashing the simulation. 


Challenges and Improvements:
   Freeing: The system required a way to slow down sensor measurements to prevent flooding the network with continuous data, but using delay() froze the                                 entire board and dropped the Wi-Fi connection—so the implementation was switched to millis() to send data every 3 seconds while keeping the board                            awake to maintain network tasks in the background.
   Multi-condition: The system initially evaluated sensors using simple sequential checks, but when two bad conditions occurred at once, it only triggered the                                   first alert and ignored the combined critical state—so a priority logic hierarchy was created to check for multi-variable emergencies first                                  before processing single-variable warnings.
   MQTT Connection Drops: The system initially tried to publish data continuously regardless of network status, but public cloud brokers drop connections during                                       brief internet hiccups, causing messages to disappear into the void—so an automatic reconnection loop was created to check the broker                                        status on every cycle and rejoin without freezing the system.


Images:
Circuit Layout: <img width="953" height="796" alt="circuit layout" src="https://github.com/user-attachments/assets/88142618-1eba-43a1-aed6-ced6c9dc5cee" />
Extreme Values: <img width="1012" height="457" alt="max min values" src="https://github.com/user-attachments/assets/35db02d8-f07b-4f60-8bcb-30b3d932b933" />
Terminal Output: <img width="996" height="715" alt="observed outputs" src="https://github.com/user-attachments/assets/c71cf268-0362-486f-87ad-ea1f267feff4" />
Ensuring Connection: <img width="928" height="522" alt="Output after wifi connection" src="https://github.com/user-attachments/assets/7b742055-9bc3-4732-b3ae-6f3e850bd58d" />
Published Payloads: <img width="772" height="846" alt="Published Payload" src="https://github.com/user-attachments/assets/7e379406-2203-4af7-b9cc-2af14678c6d6" />


Sensor Values:
  Observed (Min, Max)
      Soil Moisture (0, 4095)                Potentiometer
      Temperature (-40 °C, 80 °C)    DHT22  
      Light (4063,32)                        LDR
      Humidity (0 %, 100 %)                  DHT22

  Threshold Values
    Soil Moisture: 1500  
          Calculated to trigger at approximately **36% moisture depletion** from the fully saturated baseline.
            Formula: Threshold = Observed Min + ((Observed Max - Observed Min) * 0.366)
            Calculation: 0 + ((4095 - 0) * 0.366) = 1498.77 (rounded to **1500**)
            Rationale: Values below 1500 indicate that soil moisture has crossed into the depletion zone where irrigation is required before permanent wilting occurs.
    Light: 3000
          Calculated to trigger when ambient light drops into the bottom **25% of the observed light spectrum** (top 75% of the ADC darkness scale).
            Formula: Threshold = Observed Min + ((Observed Max - Observed Min) * 0.75)
            Calculation: 500 + ((3800 - 500) * 0.75) = 2975 (rounded to **3000**)
            Rationale: ADC values exceeding 3000 indicate low light conditions below the Photosynthetically Active Radiation (PAR) minimum required for healthy growth.
    Temperature: 35 °C
          Selected directly based on biological thermal stress limits.
            Rationale: Temperatures exceeding 35.0 °C induce heat stress and rapid transpiration in standard potted plants.
            
            
Custom Condition: 
"HEAT_WAVE_DROUGHT_RISK"
Unlike standard single-sensor alerts (NEEDS_WATER or TOO_HOT), this is a multi-variable rule that triggers only when two environmental stressors occur at the exact same     time.
  The Dual-Trigger Mechanism
    Condition 1: Soil moisture drops below the observed baseline (soilVal < 1500).
    Condition 2: Ambient temperature exceeds thermal limits (temperature > 35.0 °C).
    Logical Operator: && (AND) requires both conditions to be TRUE simultaneously. If the soil is dry at 25 °C, it defaults to NEEDS_WATER. If it is 40 °C but well-watered,     it defaults to TOO_HOT.

"FREEZING"
This is a critical thermal boundary rule that triggers when ambient temperatures drop below safe growing limits.
    Condition: Temperature drops below the baseline (temperature < 10 °C).


   
