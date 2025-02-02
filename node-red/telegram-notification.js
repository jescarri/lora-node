var replacements = [
    [/\*/g, '\\*', 'asterisks'],
    [/#/g, '\\#', 'number signs'],
    [/\//g, '\\/', 'slashes'],
    [/\(/g, '\\(', 'parentheses'],
    [/\)/g, '\\)', 'parentheses'],
    [/\[/g, '\\[', 'square brackets'],
    [/\]/g, '\\]', 'square brackets'],
    [/</g, '&lt;', 'angle brackets'],
    [/>/g, '&gt;', 'angle brackets'],
    [/_/g, '\\_', 'underscores'],
    [/`/g, '\\`', 'codeblocks'],
    [/\./g, '\\.', 'dots'],
    [/-/g, '\\-', 'dashes']
]

function clean(string, skips) {
    skips = skips || []
    return replacements.reduce(function (string, replacement) {
        var name = replacement[2]
        return name && skips.indexOf(name) !== -1
            ? string
            : string.replace(replacement[0], replacement[1])
    }, string)
}

var content = "🔔 🔔 🔔 *Lora Sensor Offline* 🔔 🔔 🔔 \n"
content += "*SensorName:* " + clean(msg.payload.name) + "\n"
content += "*LastRX:* " + clean(msg.payload.last_rx) + "\n"
content += "⚡*Voltage:* " + String(msg.payload.battery_voltage) + " Volts\n"
content += "🔋*Battery Percentage:*" + String(msg.payload.battery_percent) + "%\n"

var message = {
    payload: {
        chatId: "<TELEGRAM_CHAT_ID>",
        type: "message",

        content: content,
        options: {
            parse_mode: "MarkdownV2",
        },
    }
}


return message;
