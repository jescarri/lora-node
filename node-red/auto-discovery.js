if (msg.payload == "" ){
  return null

}

if (msg.payload.uplink_message.decoded_payload == undefined){
  return null
}

const d = new Date();
const dIso = d.toISOString();

var location_discovery = {
    payload: {
        name: "location",
        json_attributes_topic: "ttn/soil-conditions/devices/"+ msg.payload.end_device_ids.device_id+"/up",
        json_attributes_template:  "{{ value_json.uplink_message.locations.user | tojson }}",
        platform: "device_tracker",
        force_update: true,
        object_id: msg.payload.end_device_ids.device_id,
        unique_id: msg.payload.end_device_ids.dev_eui + "_location",
        device: {
           hw_version: "v1.0.0",
           identifiers:  msg.payload.end_device_ids.dev_eui,
           manufacturer: "VA7RCV",
           name: msg.payload.end_device_ids.device_id,
        },
        availability_topic: "homeassistant/device_tracker/" + msg.payload.end_device_ids.device_id +"/availability",
        payload_available: "online",
        availability_template: "online",
        availability_mode: "latest",
        state_topic: "homeassistant/device_tracker/" + msg.payload.end_device_ids.device_id + "/state",
    },
    topic: "homeassistant/device_tracker/"+ msg.payload.end_device_ids.device_id+"/config"
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
        state_topic: "ttn/soil-conditions/devices/"+ msg.payload.end_device_ids.device_id+"/up",
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
           identifiers:  msg.payload.end_device_ids.dev_eui,
           manufacturer: "VA7RCV",
           name: msg.payload.end_device_ids.device_id,
        },
    },
    topic: "homeassistant/sensor/"+ msg.payload.end_device_ids.device_id + "/battery/config",

}

var soil_moisture = {
   payload: {
        name: "soil_moisture",
        state_topic: "ttn/soil-conditions/devices/"+ msg.payload.end_device_ids.device_id+"/up",
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
           identifiers:  msg.payload.end_device_ids.dev_eui,
           manufacturer: "VA7RCV",
           name: msg.payload.end_device_ids.device_id,
        },
    },
    topic: "homeassistant/sensor/"+ msg.payload.end_device_ids.device_id + "/moisture/config",
}

var battery_voltage = {
   payload: {
        name: "voltage",
        state_topic: "ttn/soil-conditions/devices/"+ msg.payload.end_device_ids.device_id+"/up",
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
           identifiers:  msg.payload.end_device_ids.dev_eui,
           manufacturer: "VA7RCV",
           name: msg.payload.end_device_ids.device_id,
        },
    },
    topic: "homeassistant/sensor/"+ msg.payload.end_device_ids.device_id + "/voltage/config",
}

var sleep_time = {
   payload: {
        name: "sleep_time",
        state_topic: "ttn/soil-conditions/devices/"+ msg.payload.end_device_ids.device_id+"/up",
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
           identifiers:  msg.payload.end_device_ids.dev_eui,
           manufacturer: "VA7RCV",
           name: msg.payload.end_device_ids.device_id,
        },
    },
    topic: "homeassistant/sensor/"+ msg.payload.end_device_ids.device_id + "/duration/config",
}

var rx_time = {
   payload: {
        name: "last_rx",
        state_topic: "ttn/soil-conditions/devices/"+ msg.payload.end_device_ids.device_id+"/up",
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
           identifiers:  msg.payload.end_device_ids.dev_eui,
           manufacturer: "VA7RCV",
           name: msg.payload.end_device_ids.device_id,
        },
    },
    topic: "homeassistant/sensor/"+ msg.payload.end_device_ids.device_id + "/date/config",
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
    },
    topic: "lora/debug/"+msg.payload.end_device_ids.application_ids.application_id+ "/"+ msg.payload.end_device_ids.device_id + "/log",
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
    debug_data
    ];
