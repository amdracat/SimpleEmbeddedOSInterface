```mermaid
flowchart TD
    A[TemperatureSensor] -->|EVENT_TEMP_READY| B[MotorCtrl]
    A --> C[I2C HAL]
    B --> D[GPIO HAL]
    D --> G[Dummy]
    C --> G
    A --> F[最小OS抽象レイヤ]
    B --> F
```