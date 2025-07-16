#!/usr/bin/env bash

# Usage: ./chunk2ttn.sh '{"u":"http://foo.bar/f.elf","m":"XXXXXXXXXXXXXXXXXXXX","v":"200","s":"NNNNNNNNNNNNNNNNNNNN"}' 51

set -e

JSON="$1"
RAW_CHUNK_SIZE="${2:-51}"  # Default to 51 bytes (typical LoRaWAN max payload for SF7 US915)

if [[ -z "$JSON" ]]; then
   echo "Usage: $0 '<json-string>' [raw_chunk_size]"
   exit 1
fi

# Remove whitespace for compactness
MINIFIED=$(echo "$JSON" | tr -d '\n' | tr -d ' ')

# Split into raw chunks
CHUNKS=()
i=0
while [[ $i -lt ${#MINIFIED} ]]; do
   CHUNK="${MINIFIED:$i:$RAW_CHUNK_SIZE}"
   # Base64 encode the chunk
   B64=$(echo -n "$CHUNK" | base64 | tr -d '\n')
   CHUNKS+=("$B64")
   i=$((i + RAW_CHUNK_SIZE))
done

# Build TTN MQTT JSON
echo '{'
echo '  "downlinks": ['
for idx in "${!CHUNKS[@]}"; do
   FPORT=$((idx + 1))
   PAYLOAD="${CHUNKS[$idx]}"
   echo '    {'
   echo "      \"f_port\": $FPORT,"
   echo "      \"frm_payload\": \"$PAYLOAD\","
   echo '      "priority": "NORMAL"'
   if [[ $idx -lt $((${#CHUNKS[@]} - 1)) ]]; then
      echo '    },'
   else
      echo '    }'
   fi
done
echo '  ]'
echo '}'
