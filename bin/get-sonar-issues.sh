#!/usr/bin/env bash
#
# Export SonarCloud issues to sonar_issues.csv using curl + jq
#   – Public projects need no token
#   – For private projects:  export SONAR_TOKEN=<token>
#
# Requires: bash ≥4 (for associative arrays) and jq

set -euo pipefail

PROJECT="jescarri_lora-node"
STATUSES="OPEN,CONFIRMED"
PAGE_SIZE=500
API="https://sonarcloud.io/api"
CSV="sonar_issues.csv"

# ── Auth header if token supplied ───────────────────────────────────────────────
AUTH=()
[[ -n "${SONAR_TOKEN:-}" ]] && AUTH=(-u "${SONAR_TOKEN}:")

# ── CSV heading ────────────────────────────────────────────────────────────────
echo 'issue_key,type,severity,message,component,line,rule,how_to_fix' >"$CSV"

# ── Rule description memoization ───────────────────────────────────────────────
declare -A RULE_FIX

get_rule_fix()  {               # $1 = rule key
   local key="$1"
   if [[ -z "${RULE_FIX[$key]+_}" ]]; then
      local json
      json=$(curl -s "${AUTH[@]}" "$API/rules/show?key=$key")
      RULE_FIX["$key"]=$(
         jq -r '
          .rule.htmlDesc // .rule.description // ""            # take HTML or plain text
          | gsub("<[^>]+>";"")                                  # strip HTML tags
          | gsub("\n";" ")                                      # flatten new-lines
        ' <<<"$json"
      )
      sleep 0.1 # be gentle with the API
   fi
   printf '%s' "${RULE_FIX[$key]}"
}

# ── Page through issues ────────────────────────────────────────────────────────
page=1
while :; do
   resp=$(curl -s "${AUTH[@]}" \
          "$API/issues/search?componentKeys=$PROJECT&statuses=$STATUSES&ps=$PAGE_SIZE&p=$page")

   total=$(jq -r '.paging.total' <<<"$resp")
   jq -c '.issues[]' <<<"$resp" | while read -r issue; do
      key=$(jq -r '.key'      <<<"$issue")
      typ=$(jq -r '.type'     <<<"$issue")
      sev=$(jq -r '.severity' <<<"$issue")
      msg=$(jq -r '.message     | gsub("\n";" ")'          <<<"$issue")
      comp=$(jq -r '.component  | split(":") | .[-1]'      <<<"$issue")
      line=$(jq -r '.line // ""' <<<"$issue")
      rule=$(jq -r '.rule'     <<<"$issue")
      fix=$(get_rule_fix "$rule")

      # emit one CSV line via jq’s @csv
      jq -Rn --arg k "$key" --arg t "$typ" --arg s "$sev" --arg m "$msg" \
           --arg c "$comp" --arg l "$line" --arg r "$rule" --arg f "$fix" \
           '[$k,$t,$s,$m,$c,$l,$r,$f] | @csv' >>"$CSV"
   done

   ((page * PAGE_SIZE >= total))  && break
   ((page++))
done

printf '✅  %s written with %s issues\n' "$CSV" $(($( wc -l <"$CSV") - 1))
