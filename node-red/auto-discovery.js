// --- Firmware Version Sensor ---
// Get firmware version from generic_10, decode as float (divide by 100), fallback to 0.0.1 if missing
let fw_version_raw = msg.payload.uplink_message.decoded_payload.generic_10;
let fw_version = '0.0.1';
if (fw_version_raw !== undefined && fw_version_raw !== null) {
    // Convert to string with 2 decimals (e.g., 110 -> '1.10')
    fw_version = (fw_version_raw / 100).toFixed(2);
}

if (msg.payload == "") {
    return null

}

if (msg.payload.uplink_message.decoded_payload == undefined) {
    return null
}

const d = new Date();
const dIso = d.toISOString();

var location_discovery = {
    payload: {
        name: "location",
        json_attributes_topic: "ttn/soil-conditions/devices/" + msg.payload.end_device_ids.device_id + "/up",
        json_attributes_template: "{{ value_json.uplink_message.locations.user | tojson }}",
        platform: "device_tracker",
        force_update: true,
        object_id: msg.payload.end_device_ids.device_id,
        unique_id: msg.payload.end_device_ids.dev_eui + "_location",
        device: {
            hw_version: "v1.0.0",
            sw_version: fw_version,
            identifiers: msg.payload.end_device_ids.dev_eui,
            manufacturer: "VA7RCV",
            name: msg.payload.end_device_ids.device_id,
        },
        availability_topic: "homeassistant/device_tracker/" + msg.payload.end_device_ids.device_id + "/availability",
        payload_available: "online",
        availability_template: "online",
        availability_mode: "latest",
        state_topic: "homeassistant/device_tracker/" + msg.payload.end_device_ids.device_id + "/state",
    },
    topic: "homeassistant/device_tracker/" + msg.payload.end_device_ids.device_id + "/config"
};

var location_available = {
    payload: "online",
    topic: "homeassistant/device_tracker/" + msg.payload.end_device_ids.device_id + "/availability",
}

var state = {
    payload: "not_home",
    topic: "homeassistant/device_tracker/" + msg.payload.end_device_ids.device_id + "/state",
}

var battery_charge = {
    payload: {
        name: "battery",
        state_topic: "ttn/soil-conditions/devices/" + msg.payload.end_device_ids.device_id + "/up",
        value_template: "{{ value_json.uplink_message.decoded_payload.percentage_4 }}",
        platform: "sensor",
        device_class: "battery",
        force_update: true,
        state_class: "measurement",
        availability_topic: "homeassistant/device_tracker/" + msg.payload.end_device_ids.device_id + "/availability",
        payload_available: "online",
        availability_template: "online",
        availability_mode: "latest",
        unit_of_measurement: "%",
        object_id: msg.payload.end_device_ids.device_id,
        unique_id: msg.payload.end_device_ids.dev_eui + "_battery_charge",
        device: {
            hw_version: "v1.0.0",
            sw_version: fw_version,
            identifiers: msg.payload.end_device_ids.dev_eui,
            manufacturer: "VA7RCV",
            name: msg.payload.end_device_ids.device_id,
        },
    },
    topic: "homeassistant/sensor/" + msg.payload.end_device_ids.device_id + "/battery/config",

}

var soil_moisture = {
    payload: {
        name: "soil_moisture",
        state_topic: "ttn/soil-conditions/devices/" + msg.payload.end_device_ids.device_id + "/up",
        value_template: "{{ value_json.uplink_message.decoded_payload.percentage_2 }}",
        platform: "sensor",
        device_class: "moisture",
        force_update: true,
        state_class: "measurement",
        availability_topic: "homeassistant/device_tracker/" + msg.payload.end_device_ids.device_id + "/availability",
        payload_available: "online",
        availability_template: "online",
        availability_mode: "latest",
        unit_of_measurement: "%",
        object_id: msg.payload.end_device_ids.device_id,
        unique_id: msg.payload.end_device_ids.dev_eui + "_soil_moisture_percentage",
        device: {
            hw_version: "v1.0.0",
            sw_version: fw_version,
            identifiers: msg.payload.end_device_ids.dev_eui,
            manufacturer: "VA7RCV",
            name: msg.payload.end_device_ids.device_id,
        },
    },
    topic: "homeassistant/sensor/" + msg.payload.end_device_ids.device_id + "/moisture/config",
}

var battery_voltage = {
    payload: {
        name: "voltage",
        state_topic: "ttn/soil-conditions/devices/" + msg.payload.end_device_ids.device_id + "/up",
        value_template: "{{ value_json.uplink_message.decoded_payload.voltage_1 }}",
        platform: "sensor",
        device_class: "voltage",
        force_update: true,
        state_class: "measurement",
        availability_topic: "homeassistant/device_tracker/" + msg.payload.end_device_ids.device_id + "/availability",
        payload_available: "online",
        availability_template: "online",
        availability_mode: "latest",
        unit_of_measurement: "V",
        object_id: msg.payload.end_device_ids.device_id,
        unique_id: msg.payload.end_device_ids.dev_eui + "_battery_voltage",
        device: {
            hw_version: "v1.0.0",
            sw_version: fw_version,
            identifiers: msg.payload.end_device_ids.dev_eui,
            manufacturer: "VA7RCV",
            name: msg.payload.end_device_ids.device_id,
        },
    },
    topic: "homeassistant/sensor/" + msg.payload.end_device_ids.device_id + "/voltage/config",
}

var sleep_time = {
    payload: {
        name: "sleep_time",
        state_topic: "ttn/soil-conditions/devices/" + msg.payload.end_device_ids.device_id + "/up",
        value_template: "{{ value_json.uplink_message.decoded_payload.generic_8 }}",
        platform: "sensor",
        device_class: "duration",
        force_update: true,
        state_class: "measurement",
        availability_topic: "homeassistant/device_tracker/" + msg.payload.end_device_ids.device_id + "/availability",
        payload_available: "online",
        availability_template: "online",
        availability_mode: "latest",
        unit_of_measurement: "s",
        object_id: msg.payload.end_device_ids.device_id,
        unique_id: msg.payload.end_device_ids.dev_eui + "_deep_sleep_duration",
        device: {
            hw_version: "v1.0.0",
            sw_version: fw_version,
            identifiers: msg.payload.end_device_ids.dev_eui,
            manufacturer: "VA7RCV",
            name: msg.payload.end_device_ids.device_id,
        },
    },
    topic: "homeassistant/sensor/" + msg.payload.end_device_ids.device_id + "/duration/config",
}

var run_time = {
    payload: {
        name: "run_time",
        state_topic: "ttn/soil-conditions/devices/" + msg.payload.end_device_ids.device_id + "/up",
        value_template: "{{ value_json.uplink_message.decoded_payload.generic_9 }}",
        platform: "sensor",
        device_class: "duration",
        force_update: true,
        state_class: "measurement",
        availability_topic: "homeassistant/device_tracker/" + msg.payload.end_device_ids.device_id + "/availability",
        payload_available: "online",
        availability_template: "online",
        availability_mode: "latest",
        unit_of_measurement: "s",
        object_id: msg.payload.end_device_ids.device_id,
        unique_id: msg.payload.end_device_ids.dev_eui + "_run_time_duration",
        device: {
            hw_version: "v1.0.0",
            sw_version: fw_version,
            identifiers: msg.payload.end_device_ids.dev_eui,
            manufacturer: "VA7RCV",
            name: msg.payload.end_device_ids.device_id,
        },
    },
    topic: "homeassistant/sensor/" + msg.payload.end_device_ids.device_id + "/runtime/config",
}

var rx_time = {
    payload: {
        name: "last_rx",
        state_topic: "ttn/soil-conditions/devices/" + msg.payload.end_device_ids.device_id + "/up",
        value_template: dIso,
        platform: "sensor",
        device_class: "timestamp",
        force_update: true,
        state_class: "measurement",
        availability_topic: "homeassistant/device_tracker/" + msg.payload.end_device_ids.device_id + "/availability",
        payload_available: "online",
        availability_template: "online",
        availability_mode: "latest",
        object_id: msg.payload.end_device_ids.device_id,
        unique_id: msg.payload.end_device_ids.dev_eui + "_last_rx_payload",
        device: {
            hw_version: "v1.0.0",
            sw_version: fw_version,
            identifiers: msg.payload.end_device_ids.dev_eui,
            manufacturer: "VA7RCV",
            name: msg.payload.end_device_ids.device_id,
        },
    },
    topic: "homeassistant/sensor/" + msg.payload.end_device_ids.device_id + "/date/config",
}

var debug_data = {
    payload: {
        name: msg.payload.end_device_ids.device_id,
        soil_moisture_raw: msg.payload.uplink_message.decoded_payload.generic_0,
        c_air_value: msg.payload.uplink_message.decoded_payload.generic_6,
        c_water_value: msg.payload.uplink_message.decoded_payload.generic_7,
        deep_sleep_duration: msg.payload.uplink_message.decoded_payload.generic_8,
        soil_moisture_percentage: msg.payload.uplink_message.decoded_payload.percentage_2,
        battery_percent: msg.payload.uplink_message.decoded_payload.percentage_4,
        battery_discharge_rate: msg.payload.uplink_message.decoded_payload.percentage_5,
        battery_gauge_ok: msg.payload.uplink_message.decoded_payload.switch_3,
        battery_voltage: msg.payload.uplink_message.decoded_payload.voltage_1,
        dev_eui: msg.payload.end_device_ids.dev_eui,
        last_rx: dIso,
        runtime: msg.payload.uplink_message.decoded_payload.generic_9,
    },
    topic: "lora/debug/" + msg.payload.end_device_ids.application_ids.application_id + "/" + msg.payload.end_device_ids.device_id + "/log",
}



var firmware_version_sensor = {
    payload: {
        name: "firmware_version",
        state_topic: "ttn/soil-conditions/devices/" + msg.payload.end_device_ids.device_id + "/up",
        value_template: "{{ value_json.uplink_message.decoded_payload.generic_10 | default(1) | float / 100 | round(2) }}",
        platform: "sensor",
        force_update: true,
        state_class: "measurement",
        availability_topic: "homeassistant/device_tracker/" + msg.payload.end_device_ids.device_id + "/availability",
        payload_available: "online",
        availability_template: "online",
        availability_mode: "latest",
        unit_of_measurement: "",
        object_id: msg.payload.end_device_ids.device_id,
        unique_id: msg.payload.end_device_ids.dev_eui + "_firmware_version",
        device: {
            hw_version: "v1.0.0",
            sw_version: fw_version,
            identifiers: msg.payload.end_device_ids.dev_eui,
            manufacturer: "VA7RCV",
            name: msg.payload.end_device_ids.device_id,
        },
    },
    topic: "homeassistant/sensor/" + msg.payload.end_device_ids.device_id + "/firmware_version/config",
}

// Update soil_moisture sensor to clamp percentage to 100 using Jinja2 template
soil_moisture.payload.value_template = "{{ [value_json.uplink_message.decoded_payload.percentage_2, 100] | min }}";

// Update debug_data to use clamped value
if (debug_data && debug_data.payload) {
    let soil_moisture_pct = msg.payload.uplink_message.decoded_payload.percentage_2;
    if (soil_moisture_pct > 100) {
        soil_moisture_pct = 100;
    }
    debug_data.payload.soil_moisture_percentage = soil_moisture_pct;
}

return [
    location_discovery,
    location_available,
    state,
    battery_charge,
    battery_voltage,
    soil_moisture,
    sleep_time,
    rx_time,
    run_time,
    firmware_version_sensor,
    debug_data
];
