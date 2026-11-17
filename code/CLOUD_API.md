# Cloud API design (thesis architecture)

The thesis also describes a cloud-connected variant: the NodeMCU sends sensor readings to a REST backend (PHP + MySQL) and receives a relay command in the response.

```
Sensor -> NodeMCU --HTTP POST--> PHP API --> MySQL
                  <--"ON"/"OFF"--
```

> The server address and API key used during testing are intentionally **not** published. Use your own.

## 1. Insert sensor data
`POST {BASE_URL}/api/Sensor/InsertSensorData`

Headers: `apiKey: <YOUR_API_KEY>`, `Content-Type: application/json`

```json
{
  "deviceId": "node_001",
  "temperature": "30",
  "humidity": "70"
}
```

## 2. Get relay status
`POST {BASE_URL}/api/Sensor/GetRelayStatus`

Headers: `apiKey: <YOUR_API_KEY>`, `Content-Type: application/json`

```json
{ "deviceId": "node_001" }
```

Response: relay state (`ON` / `OFF`), which the NodeMCU applies to the output pin.

## Data flow
1. NodeMCU reads the sensors every 30 s
2. Sends an HTTP POST with the readings
3. Server logs them in MySQL
4. Server replies with relay status
5. NodeMCU switches the relay/pump accordingly

## Sample log (week-long test)
| Timestamp | Temp (°C) | Humidity (%) | Relay |
|---|---|---|---|
| 2024-04-10 09:00:00 | 27.5 | 55 | OFF |
| 2024-04-10 09:00:30 | 28.0 | 56 | OFF |
| 2024-04-10 09:01:00 | 29.2 | 60 | ON |
